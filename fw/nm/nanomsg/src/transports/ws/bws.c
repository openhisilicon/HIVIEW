/*
    Copyright (c) 2012-2013 250bpm s.r.o.  All rights reserved.
    Copyright (c) 2014-2016 Jack R. Dunaway. All rights reserved.
    Copyright 2016 Garrett D'Amore <garrett@damore.org>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "bws.h"
#include "aws.h"

#include "../utils/port.h"
#include "../utils/iface.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../../utils/err.h"
#include "../../utils/cont.h"
#include "../../utils/alloc.h"
#include "../../utils/list.h"
#include "../../utils/fast.h"

#include <string.h>

#if defined NN_HAVE_WINDOWS
#include "../../utils/win.h"
#else
#include <unistd.h>
#include <netinet/in.h>
#endif

/*  The backlog is set relatively high so that there are not too many failed
    connection attempts during re-connection storms. */
#define NN_BWS_BACKLOG 100

#define NN_BWS_STATE_IDLE 1
#define NN_BWS_STATE_ACTIVE 2
#define NN_BWS_STATE_STOPPING_AWS 3
#define NN_BWS_STATE_STOPPING_USOCK 4
#define NN_BWS_STATE_STOPPING_AWSS 5

#define NN_BWS_SRC_USOCK 1
#define NN_BWS_SRC_AWS 2

struct nn_bws {

    /*  The state machine. */
    struct nn_fsm fsm;
    int state;

    struct nn_ep *ep;

    /*  The underlying listening WS socket. */
    struct nn_usock usock;

    /*  The connection being accepted at the moment. */
    struct nn_aws *aws;

    /*  List of accepted connections. */
    struct nn_list awss;
};

/*  nn_ep virtual interface implementation. */
static void nn_bws_stop (void *);
static void nn_bws_destroy (void *);
const struct nn_ep_ops nn_bws_ep_ops = {
    nn_bws_stop,
    nn_bws_destroy
};

/*  Private functions. */
static void nn_bws_handler (struct nn_fsm *self, int src, int type,
    void *srcptr);
static void nn_bws_shutdown (struct nn_fsm *self, int src, int type,
    void *srcptr);
static int nn_bws_listen (struct nn_bws *self);
static void nn_bws_start_accepting (struct nn_bws *self);

int nn_bws_create (struct nn_ep *ep)
{
    int rc;
    struct nn_bws *self;
    const char *addr;
    const char *end;
    const char *pos;
    struct sockaddr_storage ss;
    size_t sslen;
    int ipv4only;
    size_t ipv4onlylen;

    /*  Allocate the new endpoint object. */
    self = nn_alloc (sizeof (struct nn_bws), "bws");
    alloc_assert (self);
    self->ep = ep;

    nn_ep_tran_setup (ep, &nn_bws_ep_ops, self);
    addr = nn_ep_getaddr (ep);

    /*  Parse the port. */
    end = addr + strlen (addr);
    pos = strrchr (addr, ':');
    if (!pos) {
        return -EINVAL;
    }
    ++pos;
    rc = nn_port_resolve (pos, end - pos);
    if (rc < 0) {
        return -EINVAL;
    }

    /*  Check whether IPv6 is to be used. */
    ipv4onlylen = sizeof (ipv4only);
    nn_ep_getopt (ep, NN_SOL_SOCKET, NN_IPV4ONLY, &ipv4only, &ipv4onlylen);
    nn_assert (ipv4onlylen == sizeof (ipv4only));

    /*  Parse the address. */
    rc = nn_iface_resolve (addr, pos - addr - 1, ipv4only, &ss, &sslen);
    if (rc < 0) {
        return -ENODEV;
    }

    /*  Initialise the structure. */
    nn_fsm_init_root (&self->fsm, nn_bws_handler, nn_bws_shutdown,
        nn_ep_getctx (ep));
    self->state = NN_BWS_STATE_IDLE;
    self->aws = NULL;
    nn_list_init (&self->awss);

    /*  Start the state machine. */
    nn_fsm_start (&self->fsm);

    nn_usock_init (&self->usock, NN_BWS_SRC_USOCK, &self->fsm);

    rc = nn_bws_listen (self);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static void nn_bws_stop (void *self)
{
    struct nn_bws *bws = self;

    nn_fsm_stop (&bws->fsm);
}

static void nn_bws_destroy (void *self)
{
    struct nn_bws *bws = self;

    nn_assert_state (bws, NN_BWS_STATE_IDLE);
    nn_list_term (&bws->awss);
    nn_assert (bws->aws == NULL);
    nn_usock_term (&bws->usock);
    nn_fsm_term (&bws->fsm);

    nn_free (bws);
}

static void nn_bws_shutdown (struct nn_fsm *self, int src, int type,
    void *srcptr)
{
    struct nn_bws *bws;
    struct nn_list_item *it;
    struct nn_aws *aws;

    bws = nn_cont (self, struct nn_bws, fsm);

    if (src == NN_FSM_ACTION && type == NN_FSM_STOP) {
        if (bws->aws) {
            nn_aws_stop (bws->aws);
            bws->state = NN_BWS_STATE_STOPPING_AWS;
        }
        else {
            bws->state = NN_BWS_STATE_STOPPING_USOCK;
        }
    }
    if (bws->state == NN_BWS_STATE_STOPPING_AWS) {
        if (!nn_aws_isidle (bws->aws))
            return;
        nn_aws_term (bws->aws);
        nn_free (bws->aws);
        bws->aws = NULL;
        nn_usock_stop (&bws->usock);
        bws->state = NN_BWS_STATE_STOPPING_USOCK;
    }
    if (bws->state == NN_BWS_STATE_STOPPING_USOCK) {
       if (!nn_usock_isidle (&bws->usock))
            return;
        for (it = nn_list_begin (&bws->awss);
              it != nn_list_end (&bws->awss);
              it = nn_list_next (&bws->awss, it)) {
            aws = nn_cont (it, struct nn_aws, item);
            nn_aws_stop (aws);
        }
        bws->state = NN_BWS_STATE_STOPPING_AWSS;
        goto awss_stopping;
    }
    if (bws->state == NN_BWS_STATE_STOPPING_AWSS) {
        nn_assert (src == NN_BWS_SRC_AWS && type == NN_AWS_STOPPED);
        aws = (struct nn_aws *) srcptr;
        nn_list_erase (&bws->awss, &aws->item);
        nn_aws_term (aws);
        nn_free (aws);

        /*  If there are no more aws state machines, we can stop the whole
            bws object. */
awss_stopping:
        if (nn_list_empty (&bws->awss)) {
            bws->state = NN_BWS_STATE_IDLE;
            nn_fsm_stopped_noevent (&bws->fsm);
            nn_ep_stopped (bws->ep);
            return;
        }

        return;
    }

    nn_fsm_bad_action (bws->state, src, type);
}

static void nn_bws_handler (struct nn_fsm *self, int src, int type,
    void *srcptr)
{
    struct nn_bws *bws;
    struct nn_aws *aws;

    bws = nn_cont (self, struct nn_bws, fsm);

    switch (bws->state) {

/******************************************************************************/
/*  IDLE state.                                                               */
/******************************************************************************/
    case NN_BWS_STATE_IDLE:
        nn_assert (src == NN_FSM_ACTION);
        nn_assert (type == NN_FSM_START);
        bws->state = NN_BWS_STATE_ACTIVE;
        return;

/******************************************************************************/
/*  ACTIVE state.                                                             */
/*  The execution is yielded to the aws state machine in this state.          */
/******************************************************************************/
    case NN_BWS_STATE_ACTIVE:
        if (src == NN_BWS_SRC_USOCK) {
            nn_assert (type == NN_USOCK_SHUTDOWN || type == NN_USOCK_STOPPED);
            return;
        }

        /*  For all remaining events we'll assume they are coming from one
            of remaining child aws objects. */
        nn_assert (src == NN_BWS_SRC_AWS);
        aws = (struct nn_aws*) srcptr;
        switch (type) {
        case NN_AWS_ACCEPTED:

            /*  Move the newly created connection to the list of existing
                connections. */
            nn_list_insert (&bws->awss, &bws->aws->item,
                nn_list_end (&bws->awss));
            bws->aws = NULL;

            /*  Start waiting for a new incoming connection. */
            nn_bws_start_accepting (bws);
            return;

        case NN_AWS_ERROR:
            nn_aws_stop (aws);
            return;
        case NN_AWS_STOPPED:
            nn_list_erase (&bws->awss, &aws->item);
            nn_aws_term (aws);
            nn_free (aws);
            return;
        default:
            nn_fsm_bad_action (bws->state, src, type);
        }

/******************************************************************************/
/*  Invalid state.                                                            */
/******************************************************************************/
    default:
        nn_fsm_bad_state (bws->state, src, type);
    }
}

static int nn_bws_listen (struct nn_bws *self)
{
    int rc;
    struct sockaddr_storage ss;
    size_t sslen;
    int ipv4only;
    size_t ipv4onlylen;
    const char *addr;
    const char *end;
    const char *pos;
    uint16_t port;

    /*  First, resolve the IP address. */
    addr = nn_ep_getaddr (self->ep);
    memset (&ss, 0, sizeof (ss));

    /*  Parse the port. */
    end = addr + strlen (addr);
    pos = strrchr (addr, ':');
    nn_assert (pos);
    ++pos;
    rc = nn_port_resolve (pos, end - pos);
    if (rc < 0) {
        return rc;
    }
    port = (uint16_t) rc;

    /*  Parse the address. */
    ipv4onlylen = sizeof (ipv4only);
    nn_ep_getopt (self->ep, NN_SOL_SOCKET, NN_IPV4ONLY,
        &ipv4only, &ipv4onlylen);
    nn_assert (ipv4onlylen == sizeof (ipv4only));
    rc = nn_iface_resolve (addr, pos - addr - 1, ipv4only, &ss, &sslen);
    if (rc < 0) {
        return rc;
    }

    /*  Combine the port and the address. */
    if (ss.ss_family == AF_INET) {
        ((struct sockaddr_in*) &ss)->sin_port = htons (port);
        sslen = sizeof (struct sockaddr_in);
    }
    else if (ss.ss_family == AF_INET6) {
        ((struct sockaddr_in6*) &ss)->sin6_port = htons (port);
        sslen = sizeof (struct sockaddr_in6);
    }
    else
        nn_assert (0);

    /*  Start listening for incoming connections. */
    rc = nn_usock_start (&self->usock, ss.ss_family, SOCK_STREAM, 0);
    if (rc < 0) {
        return rc;
    }

    rc = nn_usock_bind (&self->usock, (struct sockaddr*) &ss, (size_t) sslen);
    if (rc < 0) {
        nn_usock_stop (&self->usock);
        return rc;
    }

    rc = nn_usock_listen (&self->usock, NN_BWS_BACKLOG);
    if (rc < 0) {
        nn_usock_stop (&self->usock);
        return rc;
    }
    nn_bws_start_accepting(self);

    return 0;
}

/******************************************************************************/
/*  State machine actions.                                                    */
/******************************************************************************/

static void nn_bws_start_accepting (struct nn_bws *self)
{
    nn_assert (self->aws == NULL);

    /*  Allocate new aws state machine. */
    self->aws = nn_alloc (sizeof (struct nn_aws), "aws");
    alloc_assert (self->aws);
    nn_aws_init (self->aws, NN_BWS_SRC_AWS, self->ep, &self->fsm);

    /*  Start waiting for a new incoming connection. */
    nn_aws_start (self->aws, &self->usock);
}

#ifndef LWS_SUB_H
#define LWS_SUB_H

#include <stdio.h>
#include <libwebsockets.h>

const char* lws_getReasonString(int reason){
    static const char* reason_strings[] = {
    "LWS_CALLBACK_ESTABLISHED",
    "LWS_CALLBACK_CLIENT_CONNECTION_ERROR",
    "LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH",
    "LWS_CALLBACK_CLIENT_ESTABLISHED",
    "LWS_CALLBACK_CLOSED",
    "LWS_CALLBACK_RECEIVE",
    "LWS_CALLBACK_CLIENT_RECEIVE",
    "LWS_CALLBACK_CLIENT_RECEIVE_PONG",
    "LWS_CALLBACK_CLIENT_WRITEABLE",
    "LWS_CALLBACK_SERVER_WRITEABLE",
    "LWS_CALLBACK_HTTP",
    "LWS_CALLBACK_HTTP_FILE_COMPLETION",
    "LWS_CALLBACK_FILTER_NETWORK_CONNECTION",
    "LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION",
    "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS",
    "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS",
    "LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION",
    "LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER",
    "LWS_CALLBACK_CONFIRM_EXTENSION_OKAY",
    "LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED",
    "LWS_CALLBACK_PROTOCOL_INIT",
    "LWS_CALLBACK_PROTOCOL_DESTROY",
    /* external poll() management support */
    "LWS_CALLBACK_ADD_POLL_FD",
    "LWS_CALLBACK_DEL_POLL_FD",
    "LWS_CALLBACK_SET_MODE_POLL_FD",
    "LWS_CALLBACK_CLEAR_MODE_POLL_FD",
    };
    return reason_strings[reason];
}

static void dump_handshake_info(struct lws *wsi){
    int n;
    static const char *token_names[WSI_TOKEN_COUNT] = {
        /*[WSI_TOKEN_GET_URI]		=*/ "GET URI",
        /*[WSI_TOKEN_HOST]		=*/ "Host",
        /*[WSI_TOKEN_CONNECTION]	=*/ "Connection",
        /*[WSI_TOKEN_KEY1]		=*/ "key 1",
        /*[WSI_TOKEN_KEY2]		=*/ "key 2",
        /*[WSI_TOKEN_PROTOCOL]		=*/ "Protocol",
        /*[WSI_TOKEN_UPGRADE]		=*/ "Upgrade",
        /*[WSI_TOKEN_ORIGIN]		=*/ "Origin",
        /*[WSI_TOKEN_DRAFT]		=*/ "Draft",
        /*[WSI_TOKEN_CHALLENGE]		=*/ "Challenge",

        /* new for 04 */
        /*[WSI_TOKEN_KEY]		=*/ "Key",
        /*[WSI_TOKEN_VERSION]		=*/ "Version",
        /*[WSI_TOKEN_SWORIGIN]		=*/ "Sworigin",

        /* new for 05 */
        /*[WSI_TOKEN_EXTENSIONS]	=*/ "Extensions",

        /* client receives these */
        /*[WSI_TOKEN_ACCEPT]		=*/ "Accept",
        /*[WSI_TOKEN_NONCE]		=*/ "Nonce",
        /*[WSI_TOKEN_HTTP]		=*/ "Http",
        /*[WSI_TOKEN_MUXURL]	=*/ "MuxURL",
    };
    char buf[256];

    for (n = 0; n < WSI_TOKEN_COUNT; n++) {
        if (!lws_hdr_total_length(wsi, (enum lws_token_indexes)n))
            continue;

        lws_hdr_copy(wsi, buf, sizeof buf, (enum lws_token_indexes)n);

        fprintf(stderr, "    %s = %s\n", token_names[n], buf);
    }
}

#endif

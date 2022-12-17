#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "nanomsg/src/nn.h"
#include "nanomsg/src/reqrep.h"
#include "nanomsg/src/pipeline.h"
#include "nanomsg/src/pubsub.h"
#include "nm.h"

int nm_version(void){
    printf("version:v1.0.0\r\n");
    return 0;
}

typedef struct{
    pthread_t t;
    int runflg;
    int sock;
    int eid;
    int (*recv)(char *msg, int size, int err);
    int (*recvrw)(char *in, int isize, char *out, int *osize, int err);
}stCbParam;
static void* pull_func(void *arg){
    stCbParam *p = (stCbParam *)arg;

    // able to be cancel
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    struct timeval time;
    char *buf=NULL;
    int buf_len = 0;

    printf("start recv\r\n");
    while(p->runflg){
        buf=NULL;
        buf_len = nn_recv(p->sock, &buf, NN_MSG, 0);
        //printf("buf_len = %d\r\n", buf_len);
        if(p->runflg && p->recv){
            p->recv(buf, buf_len, 0);
        }else if (p->runflg && p->recvrw){
            char *out = malloc(NM_REP_OSIZE_MAX);
            int out_size = 0;
            if (out != NULL){
                p->recvrw(buf, buf_len, out, &out_size, 0);
                buf_len = nn_send(p->sock, out, out_size, NN_DONTWAIT);
                free(out);
            }
        }else{
            ;
        }
        nn_freemsg(buf);
    }
    printf("exit id=%p\r\n", p);
    return NULL;
}
//req/rep;
void* nm_rep_listen(char *s, int tnum, int osize, int (*recv)(char *in, int isize, char *out, int *osize, int err)){
    if ((recv == NULL) || (s == NULL))return NULL;
    int sock = -1;
    int eid = -1;
    int ret = -1;
    stCbParam *p = NULL;
    
    do{
        sock = nn_socket(AF_SP, NN_REP);
        if (sock < 0)break;
        
        eid = nn_bind(sock, s);
        if (eid < 0)break;
            
        p = (stCbParam *)malloc(sizeof(stCbParam));
        memset((char *)p, 0, sizeof(stCbParam));
        if (p == NULL)break;
        p->sock = sock;
        p->eid = eid;
        p->recvrw = recv;
        p->runflg = 1;
        pthread_create(&p->t, NULL, pull_func, p);
        ret = 0;
    }while(0);
    
    printf("sock = %d ret=%d\r\n", p->sock, ret);
    if (ret < 0){
        if (p != NULL)free((char *)p);
        if (p->sock >= 0)nn_shutdown(p->sock, p->eid);
        if (p->runflg){
            p->runflg = 0;
            pthread_cancel(p->t);
            void *res;
            pthread_join(p->t, &res);
        }
    }
    return p;
}
int nm_rep_close(void* rep){
    stCbParam *p = (stCbParam *)rep;
    if (p == NULL)return -1;
    if (p->runflg){
        p->runflg = 0;
        if (p->sock >= 0)nn_shutdown(p->sock, p->eid);
        pthread_cancel(p->t);
        void *res;
        pthread_join(p->t, &res);
    }
    free((char *)p);
    return 0;
}

int nm_req_conn(char *s){
    int sock = nn_socket(AF_SP, NN_REQ);
    if (sock < 0)return -1;
    int ret = nn_connect(sock, s);
    //printf("ret = %d\r\n", ret);
    if (ret < 0){
        nn_close(sock);
        sock = -1;
    }
    //printf("sock = %d\r\n", sock);
    return sock;
}
int nm_req_close(int req){
    return nn_close(req);
}
int nm_req_send(int req, int to, char *in, int isize, char *out, int *osize){
    int millis = to;
    int rc = nn_setsockopt (req, NN_SOL_SOCKET, NN_SNDTIMEO, &millis, sizeof (millis));
    if (rc < 0)printf("set timeout = %d\r\n", rc);
    int t = nn_send(req, in, isize, 0);
    if (t <= 0)printf("send size = %d\r\n", t);
    return t;
}
int nm_req_recv(int req, char *out, int *osize, int to){
    int millis = to;
    int rc = nn_setsockopt (req, NN_SOL_SOCKET, NN_RCVTIMEO, &millis, sizeof (millis));
    if (rc < 0)printf("set timeout = %d\r\n", rc);
    int buf_len = nn_recv(req, out, *osize, 0);
    *osize = buf_len;
    return 0;
}
int nm_req_sendto(char *s, int to, char *in, int isize, char *out, int *osize){
    int sock = nm_req_conn(s);
    nm_req_send(sock, to, in, isize, out, osize);
    nm_req_recv(sock, out, osize, to);
    nm_req_close(sock);
    return 0;
}

//push/pull;
void* nm_pull_listen(char *s, int (*recv)(char *msg, int size, int err)){
    if ((recv == NULL) || (s == NULL))return NULL;
    int sock = -1;
    int eid = -1;
    int ret = -1;
    stCbParam *p = NULL;
    
    do{
        sock = nn_socket(AF_SP, NN_PULL);
        if (sock < 0)break;
        
        eid = nn_bind(sock, s);
        if (eid < 0)break;
            
        p = (stCbParam *)malloc(sizeof(stCbParam));
        memset((char *)p, 0, sizeof(stCbParam));
        if (p == NULL)break;
        p->sock = sock;
        p->eid = eid;
        p->recv = recv;
        p->runflg = 1;
        pthread_create(&p->t, NULL, pull_func, p);
        ret = 0;
    }while(0);
    
    printf("sock = %d ret=%d\r\n", p->sock, ret);
    if (ret < 0){
        if (p != NULL)free((char *)p);
        if (sock >= 0)nn_close(sock);
        if (p->sock >= 0)nn_shutdown(p->sock, p->eid);
        if (p->runflg){
            p->runflg = 0;
            pthread_cancel(p->t);
            void *res;
            pthread_join(p->t, &res);
        }
    }
    return p;
}
int nm_pull_close(void* pull){
    stCbParam *p = (stCbParam *)pull;
    if (p == NULL)return -1;
    if (p->runflg){
        p->runflg = 0;
        if (p->sock >= 0)nn_shutdown(p->sock, p->eid);
        pthread_cancel(p->t);
        void *res;
        pthread_join(p->t, &res);
    }
    free((char *)p);
    return 0;
}

int nm_push_conn(char *s){
    int sock = nn_socket(AF_SP, NN_PUSH);
    if (sock < 0)return -1;
    int ret = nn_connect(sock, s);
    printf("ret = %d\r\n", ret);
    if (ret < 0){
        nn_close(sock);
        sock = -1;
    }
    printf("sock = %d\r\n", sock);
    return sock;
}
int nm_push_close(int push){
    return nn_close(push);
}
int nm_push_send(int push, char *msg, int size){
    int t = nn_send(push, msg, size, NN_DONTWAIT);
    if (t <= 0)printf("send size = %d\r\n", t);
    return t;
}
int nm_push_send_wait(int push, char *msg, int size){
    int t = nn_send(push, msg, size, 0);
    if (t <= 0)printf("send size = %d\r\n", t);
    return t;
}

//pub/sub;
void* nm_pub_listen(char *s){
    if (s == NULL)return NULL;
    int sock = -1;
    int ret = -1;
    stCbParam *p = NULL;
    
    do{
        sock = nn_socket(AF_SP, NN_PUB);
        if (sock < 0)break;
        
        ret = nn_bind(sock, s);
        if (ret < 0)break;
            
        p = (stCbParam *)malloc(sizeof(stCbParam));
        memset((char *)p, 0, sizeof(stCbParam));
        if (p == NULL)break;
        p->sock = sock;
    }while(0);
    
    if(p!=NULL)printf("sock = %d ret=%d\r\n", p->sock, ret);
    if (ret < 0){
        if (p != NULL)free((char *)p);
        if (sock >= 0)nn_close(sock);
    }
    return p;
}
int nm_pub_close(void* pub){
    stCbParam *p = (stCbParam *)pub;
    if (p == NULL)return -1;
    if (p->sock >= 0)nn_close(p->sock);
    free((char *)p);
    return 0;
}
int nm_pub_send(void* pub, char *msg, int size){
    stCbParam *p = (stCbParam *)pub;
    int t = nn_send(p->sock, msg, size, NN_DONTWAIT);
    return t;
}

void* nm_sub_conn(char *s, int (*recv)(char *msg, int size, int err)){
    if ((recv == NULL) || (s == NULL))return NULL;
    int sock = -1;
    int eid = -1;
    int ret = -1;
    stCbParam *p = NULL;
    
    do{
        sock = nn_socket(AF_SP, NN_SUB);
        if (sock < 0)break;
        
        ret = nn_connect(sock, s);
        if (ret < 0)break;
        
        eid = nn_setsockopt(sock, NN_SUB, NN_SUB_SUBSCRIBE, "", 0);
        if (eid < 0)break;
            
        p = (stCbParam *)malloc(sizeof(stCbParam));
        memset((char *)p, 0, sizeof(stCbParam));
        if (p == NULL)break;
        p->sock = sock;
        p->eid = eid;
        p->recv = recv;
        p->runflg = 1;
        pthread_create(&p->t, NULL, pull_func, p);
    }while(0);
    
    printf("sock = %d ret=%d\r\n", p->sock, ret);
    if (ret < 0){
        if (p != NULL)free((char *)p);
        if (sock >= 0)nn_shutdown(p->sock, p->eid);
        if (p->runflg){
            p->runflg = 0;
            pthread_cancel(p->t);
            void *res;
            pthread_join(p->t, &res);
        }
    }
    return p;
}
int nm_sub_close(void* sub){
    stCbParam *p = (stCbParam *)sub;
    if (p == NULL)return -1;
    if (p->runflg){
        p->runflg = 0;
        if (p->sock >= 0)nn_shutdown(p->sock, p->eid);
        pthread_cancel(p->t);
        void *res;
        pthread_join(p->t, &res);
    }
    free((char *)p);
    return 0;
}
int nm_sub_recv(void* sub, char *msg, int *osize, int to){
    stCbParam *p = (stCbParam *)sub;
    int msglen = 0;
    if (osize)msglen = *osize;
    if (msglen <= 0)return -1;
    int ret = nn_recv(p->sock, msg, *osize, 0);
    if (osize)*osize = ret;
    return ret;
}


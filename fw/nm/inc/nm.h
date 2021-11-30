#ifndef __nm_h__
#define  __nm_h__

#ifdef __cplusplus
extern "C" {
#endif


//Get the OS file descriptor;
enum {
  NM_POLLIN = 1,
  NM_POLLOUT = 2,
};
int nm_getfd(int nmfd, int events);

//req/rep;
#define NM_REP_MAX_WORKERS (3)
#define NM_REP_OSIZE_MIN   (4*1024)
#define NM_REP_OSIZE_MAX   (128*1024)
void* nm_rep_listen(char *s, int tnum, int osize, int (*recv)(char *in, int isize, char *out, int *osize, int err));
int nm_rep_close(void* rep);

int nm_req_conn(char *s);
int nm_req_close(int req);
int nm_req_send(int req, int to, char *in, int isize, char *out, int *osize);
int nm_req_recv(int req, char *out, int *osize, int to);
int nm_req_sendto(char *s, int to, char *in, int isize, char *out, int *osize);

//push/pull;
void* nm_pull_listen(char *s, int (*recv)(char *msg, int size, int err));
int nm_pull_close(void* pull);

int nm_push_conn(char *s);
int nm_push_close(int push);
int nm_push_send(int push, char *msg, int size);
int nm_push_send_wait(int push, char *msg, int size);

//pub/sub;
void* nm_pub_listen(char *s);
int nm_pub_close(void* pub);
int nm_pub_send(void* pub, char *msg, int size);

void* nm_sub_conn(char *s, int (*recv)(char *msg, int size, int err));
int nm_sub_close(void* sub);
int nm_sub_recv(void* sub, char *msg, int *osize, int to);

//survey/vote;
void* nm_survey_listen(char *s, int (*recv)(char *msg, int size, int err));
int nm_survey_close(void* sur);
int nm_survey_send(void* sur, int to, char *msg, int size);

void* mm_vote_conn(char *s, int (*recv)(char *in, int isize, char *out, int *osize, int err));
int  nm_vote_close(void* vote);


#ifdef __cplusplus
}
#endif

#endif

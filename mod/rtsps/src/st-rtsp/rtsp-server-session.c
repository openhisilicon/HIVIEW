#include "rtsp-server-st.h"
#include "rtp-transport.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define TIMEOUT_RECV 20000
#define TIMEOUT_SEND 10000


void rtsp_session_ondestroy(struct rtsp_session_t* sess)
{
	struct rtsp_session_t *session;
	session = (struct rtsp_session_t *)sess;
	
  if (session->media)
	{
	  rtp_media_live_free(session->media);
	  session->media = NULL;
	}
	
	if (session->rtp.data)
	{
		assert(session->rtp.capacity > 0);
		free(session->rtp.data);
		session->rtp.data = NULL;
		session->rtp.capacity = 0;
	}
	
	if (session->rtsp)
	{
		rtsp_server_destroy(session->rtsp);
		session->rtsp = NULL;
	}
	
}

void rtsp_session_onrecv(struct rtsp_session_t* sess, int code, size_t bytes)
{
	size_t remain;
	const uint8_t* p, *end;
	struct rtsp_session_t *session;
	session = (struct rtsp_session_t *)sess;

	if (0 == code && 0 == bytes)
		code = ECONNRESET;

	if (0 == code)
	{
		p = session->buffer;
		end = session->buffer + bytes;
		do
		{
			if (0 == session->rtsp_need_more_data && ('$' == *p || 0 != session->rtp.state))
			{
				p = rtp_over_rtsp(&session->rtp, p, end);
			}
			else
			{
				remain = end - p;
				code = rtsp_server_input(session->rtsp, p, &remain);
				session->rtsp_need_more_data = code;
				if (0 == code)
				{
					// TODO: pipeline remain data
					printf("@@@@@@@@@@ sess:%p, remain:%d, left[%.*s] @@@@@@@@@\n", sess, remain, end-remain, p+remain);
					//maohw assert(bytes > remain);
					//maohw assert(0 == remain || '$' == *(end - remain));
				}
				p = end - remain;
			}
		} while (p < end && 0 == code);
		
		if (code >= 0)
		{
			// need more data
			return;
		}
	}

	// error or peer closed
	if (0 != code || 0 == bytes)
	{
		session->handler.onerror(session, code ? code : ECONNRESET);
		session->loop = 0; //exit loop;
	}
}


static int rtsp_session_send(void* ptr, const void* data, size_t bytes)
{
  int ret = 0, eto = 0;
	struct rtsp_session_t *session;
	session = (struct rtsp_session_t *)ptr;
	
	if(session->snderr)
	  return -1;
	
	#define WRITE_TIMEOUT 10 //sec
  st_mutex_lock(session->lock);
	ret = st_write(session->socket, data, bytes, WRITE_TIMEOUT*1000000LL/*ST_UTIME_NO_TIMEOUT*/);
	eto = (ret < 0 && errno == ETIME)?1:0;
	st_mutex_unlock(session->lock);
	
	if(eto)
	{
	  session->snderr = 1;
	  printf("session:%p, st_write timeout %d sec.\n", session, WRITE_TIMEOUT);
	}
	return (ret == bytes)?0:-1;
}


static int rtsp_session_onclose(void* ptr2)
{
	struct rtsp_session_t *session;
	session = (struct rtsp_session_t *)ptr2;
	return 0;
}


int rtsp_session_oncreate(struct rtsp_session_t* sess)
{
	struct rtsp_session_t *session = sess;
  
	char ip[SOCKET_ADDRLEN];
	unsigned short port;
	
	int r = socket_addr_to((struct sockaddr*)&sess->addr, sess->addrlen, ip, &port);
	if(r != 0)
	{
	  return -1;
	}
	
	session->rtp.param = sess;
	session->rtp.onrtp = sess->handler.onrtp;
	session->handler.base.send = rtsp_session_send;
	session->handler.base.close = rtsp_session_onclose;
  session->rtsp = rtsp_server_create(ip, port, &session->handler.base, session, session);

	return 0;
}

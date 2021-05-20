#include <stdint.h>

#include "st_timer.h"
#include "st.h"

#include "fw/comm/inc/list.h"


#define BASE_HANDEL 100
#define TIMER_LIST_HEAD_ADDR(stimer)  (timer_set_t*)(&(stimer)->timer_list.list)

#define TIMER_LIST_ADD(p_timer, stimer) \
    do{ \
        list_add_tail(&(p_timer)->list, &(stimer)->timer_list.list); \
    }while(0)

#define TIMER_LIST_DEL(p_timer, stimer) \
    do{ \
        (stimer)->timer_count--; \
        (p_timer)->handle = 0; \
        list_del(&(p_timer)->list); \
        free(p_timer); \
    }while(0)

typedef struct timer_set_s {
    struct list_head list;
    int handle;
    int interval;
    int times;					    // -1:循环运行; N(N>0):运行N次后停止 
    timer_cb func;					// 回调函数
    void *param;					  // 回调函数参数
    unsigned int start_time;// ms
    unsigned int end_time;	// ms
    char name[64];	        // name
}timer_set_t;

typedef struct simple_timer_s {
    timer_set_t timer_list;
    unsigned int timer_count;
    int base_handle;
    int timer_running;
    int curr_task_hdl;
    st_thread_t loop_tid;
    st_mutex_t timer_lock;
}simple_timer_t;

static void* _loop_event_process(void *parm);

static inline int _timer_init(simple_timer_t *s_timer)
{	
    memset(s_timer, 0, sizeof(simple_timer_t));
    s_timer->timer_lock = st_mutex_new();
    INIT_LIST_HEAD(&(s_timer->timer_list.list));
    s_timer->base_handle = BASE_HANDEL;

    s_timer->timer_running = 1;
    if ((s_timer->loop_tid = st_thread_create(_loop_event_process, s_timer, 1, 0)) == NULL)
    {
        printf("pthread_create error!\n");
        return -1;
    }

    return 0;
}

static void *_loop_event_process(void *parm)
{
    int ret = 0;
    struct timespec ts = {0, 0};
    unsigned int curr_times_ms = 0;

    simple_timer_t *s_timer = (simple_timer_t*)parm;

    while (s_timer->timer_running)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);		// 需要链接时钟库: -lrt
        curr_times_ms = ts.tv_sec*1000 + ts.tv_nsec/(1000*1000);

        st_mutex_lock(s_timer->timer_lock);
        timer_set_t *p_timer = NULL;
        timer_set_t *p_next  = NULL;

        list_for_each_entry_safe(p_timer, p_next, &s_timer->timer_list.list, list)
        {
            if (p_timer->times > 0)						// 运行N次(N>0)的定时器
            {
                if (curr_times_ms >= p_timer->end_time)
                {
                    p_timer->times--;
                    s_timer->curr_task_hdl = p_timer->handle;
                    // 更新开始时间和结束时间
                    p_timer->start_time = curr_times_ms;
                    p_timer->end_time = p_timer->start_time + p_timer->interval;

                    if (p_timer->func != NULL)
                    {
                        st_mutex_unlock(s_timer->timer_lock);
                        ret = p_timer->func(p_timer->param);
                        st_mutex_lock(s_timer->timer_lock);
                    }

                    if (0 == p_timer->times || ret != 0)
                    {
                        TIMER_LIST_DEL(p_timer, s_timer);
                    }
                }

            }
            else if(p_timer->times == -1)				// 循环运行
            {
                if (curr_times_ms >= p_timer->end_time)
                {
                    // 更新开始时间和结束时间
                    p_timer->start_time = curr_times_ms;
                    p_timer->end_time = p_timer->start_time + p_timer->interval;
                    s_timer->curr_task_hdl = p_timer->handle;

                    if (p_timer->func != NULL)
                    {
                        st_mutex_unlock(s_timer->timer_lock);
                        ret = p_timer->func(p_timer->param);
                        st_mutex_lock(s_timer->timer_lock);
                        if(ret != 0)
                        {
                            TIMER_LIST_DEL(p_timer, s_timer);
                        }
                    }
                }
            }
        }
        st_mutex_unlock(s_timer->timer_lock);

        st_usleep(1*1000);	//1ms
    }

    return 0;
}

static inline int _timer_start(simple_timer_t *s_timer
        , int times     /*-1:循环运行; N(N>0):运行N次后停止*/
        , int interval	/*ms*/
        , char *name
        , timer_cb func
        , void *param)
{
    unsigned int curr_times_ms;
    timer_set_t *p_timer = (timer_set_t *)calloc(1, sizeof(timer_set_t));
    if (NULL == p_timer)
    {
        printf("malloc error!\n");
        return -1;
    }
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    curr_times_ms = ts.tv_sec*1000 + ts.tv_nsec/(1000*1000);

    st_mutex_lock(s_timer->timer_lock);

    p_timer->handle = s_timer->base_handle++;
    s_timer->timer_count++;
    p_timer->param = param;
    p_timer->start_time = curr_times_ms;
    p_timer->interval = interval;
    p_timer->end_time = p_timer->start_time + p_timer->interval;
    p_timer->func = func;
    p_timer->times = times;
    strncpy(p_timer->name
            , name
            , sizeof(p_timer->name)-1);
    
    TIMER_LIST_ADD(p_timer, s_timer);
    
    st_mutex_unlock(s_timer->timer_lock);

    return p_timer->handle;
}

static inline int _timer_stop(simple_timer_t *s_timer, int handle)
{
    if(s_timer->curr_task_hdl = handle)
    {
      //can't be stop(timer have triggered or will be triggered)
      return -1;
    }
  
    st_mutex_lock(s_timer->timer_lock);
    timer_set_t *p_timer = NULL;
    timer_set_t *p_next  = NULL;
    list_for_each_entry_safe(p_timer, p_next, &s_timer->timer_list.list, list)
    {
        if (handle == p_timer->handle)
        {
            TIMER_LIST_DEL(p_timer, s_timer);
            st_mutex_unlock(s_timer->timer_lock);
            return 0;
        }
    }
    st_mutex_unlock(s_timer->timer_lock);

    if (TIMER_LIST_HEAD_ADDR(s_timer) == p_timer)
        printf("Handle error!\n");

    return -1;
}

static inline int _timer_deinit(simple_timer_t *s_timer)
{
    st_mutex_lock(s_timer->timer_lock);
    s_timer->timer_running = 0;
    st_mutex_unlock(s_timer->timer_lock);

    if(s_timer->loop_tid)
    {
        st_thread_join(s_timer->loop_tid, NULL);
    }

    timer_set_t *p_timer = NULL;
    timer_set_t *p_next  = NULL;
    list_for_each_entry_safe(p_timer, p_next, &s_timer->timer_list.list, list)
    {
        if (p_timer->handle)
        {
            TIMER_LIST_DEL(p_timer, s_timer);
        }
    }	
    st_mutex_destroy(s_timer->timer_lock);
    memset(s_timer, 0, sizeof(simple_timer_t));

    return 0;
}

void *st_timer_new()
{
    int ret = 0;
    simple_timer_t *t = (simple_timer_t*)calloc(1, sizeof(simple_timer_t));

    ret = _timer_init(t);

    ret = ret;
    return (void*)t;
}

int st_timer_add(void *timer, int n, int to, char *name, timer_cb func, void *param)
{
    return _timer_start((simple_timer_t*)timer, n, to, name, func, param);
}

int st_timer_del(void *timer, int handle)
{
    return _timer_stop((simple_timer_t*)timer, handle);
}

int st_timer_free(void *timer)
{
    if(timer)
    {
        _timer_deinit((simple_timer_t*)timer);
        free(timer);
    }
    return 0;
}

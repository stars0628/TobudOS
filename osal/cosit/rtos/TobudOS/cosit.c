#include "cosit.h"
#include <tos_k.h>

static cos_status_t errno_knl2cosit(k_err_t err)
{
    switch (err) {
        case K_ERR_NONE:    return COS_OK;
        case K_ERR_IN_IRQ:  return COS_ERR_ISR;
        default:            return COS_ERR;
    }
}

cos_tick_t cos_ms_to_tick(uint64_t ms)
{
    return tos_millisec2tick((k_time_t)ms);
}

uint64_t cos_tick_to_ms(cos_tick_t tick)
{
    return tos_tick2millisec((k_tick_t)tick);
}

void cos_task_attr_init(cos_task_attr_t *attr)
{
    if (!attr) {
        return;
    }

    attr->name = "default_task";
    attr->stack_size = 512;
    attr->priority = 5;
    attr->tick = 10;
}

cos_status_t cos_task_create(cos_task_t *task, cos_task_entry_t fn, void *arg, cos_task_attr_t *attr)
{
    k_err_t err;
    k_stack_t *task_stack;

    if (!attr) {
        return COS_ERR_PARAM;
    }

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
    err = tos_task_create_dyn((k_task_t **)task, attr->name, (k_task_entry_t)fn, arg,
                                (k_prio_t)attr->priority, attr->stack_size, 
                                (k_timeslice_t)attr->tick);

    return errno_knl2cosit(err);
#endif

    *task = (cos_task_t)tos_mmheap_alloc(sizeof(k_task_t));
    if (!task) {
        return COS_ERR_NOMEM;
    }

    task_stack = (k_stack_t *)tos_mmheap_alloc(attr->stack_size);
    if (!task_stack) {
        tos_mmheap_free(task);
        return COS_ERR_NOMEM;
    }

    err = tos_task_create((k_task_t *)*task, attr->name, (k_task_entry_t)fn, arg,
                            (k_prio_t)attr->priority, task_stack, attr->stack_size, 
                            (k_timeslice_t)attr->tick);

    if (err != K_ERR_NONE) {
        tos_mmheap_free(task);
        tos_mmheap_free(task_stack);
    }

    return errno_knl2cosit(err);
}

cos_status_t cos_task_delete(cos_task_t task)
{
    k_err_t err;
    k_task_t *the_task = (k_task_t *)task;

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
    err = tos_task_destroy_dyn(the_task);
    return errno_knl2cosit(err);
#endif

    err = tos_task_destroy(the_task);
    tos_mmheap_free(the_task->stk_base);
    tos_mmheap_free(task);

    return errno_knl2cosit(err);
}

cos_status_t cos_task_init(cos_task_t task, cos_task_entry_t fn, void *arg,
                           cos_task_attr_t *attr, void *stack_buf)
{
    k_err_t err;

    err = tos_task_create((k_task_t *)task, attr->name, (k_task_entry_t)fn, 
                            arg, attr->priority, (k_stack_t *)stack_buf, 
                            attr->stack_size, attr->tick);

    return errno_knl2cosit(err);
}

cos_status_t cos_task_deinit(cos_task_t task)
{
    return errno_knl2cosit(tos_task_destroy((k_task_t *)task));
}

void cos_task_exit(int32_t status)
{
    tos_task_destroy(K_NULL);
}

cos_status_t cos_task_suspend(cos_task_t task)
{
    return errno_knl2cosit(tos_task_suspend((k_task_t *)task));
}

cos_status_t cos_task_resume(cos_task_t task)
{
    return errno_knl2cosit(tos_task_resume((k_task_t *)task));
}

cos_status_t cos_task_yield(void)
{
    tos_task_yield();

    return COS_OK;
}

cos_task_t cos_task_self(void)
{
    return (cos_task_t)tos_task_curr_task_get();
}

cos_status_t cos_task_get_name(cos_task_t task, char *buf, size_t buf_size)
{
    k_task_t *the_task = task;

    if (!the_task || !buf) {
        return COS_ERR_PARAM;
    }

    strncpy(the_task->name, buf, buf_size);

    return COS_OK;
}

cos_task_t cos_task_find(const char *name)
{
    return (cos_task_t)tos_task_find(name);
}

cos_status_t cos_task_sleep(cos_tick_t tick)
{
    return errno_knl2cosit(tos_task_delay((k_tick_t)tick));
}

cos_status_t cos_task_wakeup(cos_task_t task)
{
    //todo
    return COS_ERR;
}

cos_status_t cos_task_get_priority(cos_task_t task, uint8_t *priority)
{
    k_task_t *the_task = task;

    if (!the_task || !priority) {
        return COS_ERR_PARAM;
    }

    *priority = the_task->prio;

    return COS_OK;
}

cos_status_t cos_task_set_priority(cos_task_t task, uint8_t priority)
{
    return tos_task_prio_change((k_task_t *)task, (k_prio_t)priority);
}

#if TOS_CFG_ROUND_ROBIN_EN > 0u

cos_status_t cos_task_get_time_slice(cos_task_t task, cos_tick_t *time_slice)
{

    k_task_t *the_task = task;

    if (!the_task || !time_slice) {
        return COS_ERR_PARAM;
    }

    *time_slice = the_task->timeslice;

    return COS_OK;
}

cos_status_t cos_task_set_time_slice(cos_task_t task, cos_tick_t time_slice)
{
    tos_robin_timeslice_set((k_task_t *)task, (k_timeslice_t)time_slice);
    return COS_OK;
}

#else

cos_status_t cos_task_get_time_slice(cos_task_t task, cos_tick_t *time_slice)
{
    return COS_ERR;
}

cos_status_t cos_task_set_time_slice(cos_task_t task, cos_tick_t time_slice)
{
    return COS_ERR;
}

#endif /* TOS_CFG_ROUND_ROBIN_EN */

#if TOS_CFG_TIMER_EN > 0u

cos_status_t cos_timer_create(cos_timer_t *timer, const char *name, cos_timer_cb_t cb,
    void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options)
{
    k_err_t err;
    k_timer_t **the_timer = (k_timer_t **)timer;

    err = tos_timer_create_dyn(the_timer, (k_tick_t)initial, (k_tick_t)period,
                            (k_timer_callback_t)cb, arg, TOS_OPT_TIMER_PERIODIC);

    if (err != K_ERR_NONE) {
        return errno_knl2cosit(err);
    }

    if (options == COS_TIMER_OPTION_ACTIVATE) {
        err = tos_timer_start(*the_timer);
        return errno_knl2cosit(err);
    }

    return COS_OK;
}

cos_status_t cos_timer_delete(cos_timer_t timer)
{
    return errno_knl2cosit(tos_timer_destroy_dyn((k_timer_t *)timer));
}

cos_status_t cos_timer_init(cos_timer_t timer, const char *name, cos_timer_cb_t cb,
    void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options)
{
    k_err_t err;
    k_timer_t *the_timer = (k_timer_t *)timer;

    err = tos_timer_create(the_timer, (k_tick_t)initial, (k_tick_t)period,
                            (k_timer_callback_t)cb, arg, TOS_OPT_TIMER_PERIODIC);

    if (err != K_ERR_NONE) {
        return errno_knl2cosit(err);
    }

    if (options == COS_TIMER_OPTION_ACTIVATE) {
        err = tos_timer_start(the_timer);
        return errno_knl2cosit(err);
    }

    return COS_OK;
}

cos_status_t cos_timer_deinit(cos_timer_t timer)
{
    return errno_knl2cosit(tos_timer_destroy((k_timer_t *)timer));
}

cos_status_t cos_timer_start(cos_timer_t timer)
{
    return errno_knl2cosit(tos_timer_start((k_timer_t *)timer));
}

cos_status_t cos_timer_stop(cos_timer_t timer)
{
    return errno_knl2cosit(tos_timer_stop((k_timer_t *)timer));
}

cos_status_t cos_timer_change(cos_timer_t timer, cos_tick_t initial, cos_tick_t period)
{
    k_err_t err;
    k_timer_t *the_timer = (k_timer_t *)timer;

    err = tos_timer_delay_change(the_timer, (k_tick_t)initial);
    if (err != K_ERR_NONE) {
        return errno_knl2cosit(err);
    }

    return errno_knl2cosit(tos_timer_period_change(the_timer, (k_tick_t)period));
}

cos_status_t cos_timer_get_time(cos_timer_t timer, cos_tick_t *remaining, cos_tick_t *period)
{
    //todo
    return COS_ERR;
}

#else

cos_status_t cos_timer_create(cos_timer_t *timer, const char *name, cos_timer_cb_t cb,
    void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options)
{
    return COS_ERR;
}

cos_status_t cos_timer_delete(cos_timer_t timer)
{
    return COS_ERR;
}

cos_status_t cos_timer_init(cos_timer_t timer, const char *name, cos_timer_cb_t cb,
    void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options)
{
    return COS_ERR;
}

cos_status_t cos_timer_deinit(cos_timer_t timer)
{
    return COS_ERR;
}

cos_status_t cos_timer_start(cos_timer_t timer)
{
    return COS_ERR;
}

cos_status_t cos_timer_stop(cos_timer_t timer)
{
    return COS_ERR;
}

cos_status_t cos_timer_change(cos_timer_t timer, cos_tick_t initial, cos_tick_t period)
{
    return COS_ERR;
}

cos_status_t cos_timer_get_time(cos_timer_t timer, cos_tick_t *remaining, cos_tick_t *period)
{
    return COS_ERR;
}

#endif /* TOS_CFG_TIMER_EN */

void *cos_malloc(size_t size)
{
    return tos_mmheap_alloc(size);
}

void *cos_realloc(void *ptr, size_t new_size)
{
    return tos_mmheap_realloc(ptr, new_size);
}

void *cos_calloc(size_t count, size_t size)
{
    return tos_mmheap_calloc(count, size);
}

cos_status_t cos_malloc_align(void **memptr, size_t size, size_t alignment)
{
#if TOS_CFG_MMHEAP_EN > 0u

    if (!memptr) {
        return COS_ERR_PARAM;
    }

    *memptr = tos_mmheap_aligned_alloc(size, alignment);

    return *memptr == NULL ? COS_ERR : COS_OK;

#else
    return COS_ERR;
#endif
}

void cos_free(void *ptr)
{
    tos_mmheap_free(ptr);
}

#if TOS_CFG_MESSAGE_QUEUE_EN > 0u

cos_status_t cos_mb_create(const char* name, size_t msg_size,
                    uint8_t flag, cos_mb_t *mb)
{

}

cos_status_t cos_mb_delete(cos_mb_t mb)
{

}

cos_status_t cos_mb_init(const char* name, size_t msg_size,
                    uint8_t flag, cos_mb_t *mb)
{
    //todo
    //tos适配需要用户传入pool
}

cos_status_t cos_mb_deinit(cos_mb_t mb)
{

}

cos_status_t cos_mb_send(cos_mb_t mb, uint32_t value)
{
    return errno_knl2cosit(tos_msg_q_post((k_msg_q_t *)mb, (void *)value));
}

cos_status_t cos_mb_recv(cos_mb_t mb, uint32_t* value,
        int32_t timeout)
{
    //note: value需要用户传入二级指针，这里是否应该使用二级指针明示？
    return errno_knl2cosit(tos_msg_q_pend((k_msg_q_t *)mb, (void**)value, (k_tick_t)timeout));
}

#else

cos_status_t cos_mb_create(const char* name, size_t msg_size,
                    uint8_t flag, cos_mb_t *mb)
{

}

cos_status_t cos_mb_delete(cos_mb_t mb)
{

}

cos_status_t cos_mb_init(const char* name, size_t msg_size,
                    uint8_t flag, cos_mb_t *mb)
{

}

cos_status_t cos_mb_deinit(cos_mb_t mb)
{

}

cos_status_t cos_mb_send(cos_mb_t mb, uint32_t value)
{

}

cos_status_t cos_mb_recv(cos_mb_t mb, uint32_t* value,
        int32_t timeout)
{
        
}

#endif

#if TOS_CFG_MAIL_QUEUE_EN > 0u

cos_status_t cos_mq_create(const char* name, size_t msg_size,
                    size_t max_msgs, uint8_t flag, cos_mq_t* mq)
{
    
}

cos_status_t cos_mq_delete(cos_mq_t mq)
{

}

cos_status_t cos_mq_init(const char* name, void *msgpool, size_t msg_size,
                    size_t max_msgs, cos_mq_t mq)
{
     return errno_knl2cosit(tos_mail_q_create((k_mail_q_t *)mq, msgpool, max_msgs, msg_size));
}

cos_status_t cos_mq_deinit(cos_mq_t mq)
{
    return errno_knl2cosit(tos_mail_q_destroy((k_mail_q_t *)mq));
}

cos_status_t cos_mq_send(cos_mq_t mq, void* buffer, size_t size)
{
    return errno_knl2cosit(tos_mail_q_post((k_mail_q_t *)mq, buffer, size));
}

cos_status_t cos_mq_recv(cos_mq_t mq, void* buffer,
        size_t size, int32_t timeout)
{
    //todo
    // tos中size是出参，用来传递接收到的内容大小，而COSIT API是入参，表示缓冲区大小.
    size_t recv_size;
    
    return errno_knl2cosit(tos_mail_q_pend((k_mail_q_t *)mq, buffer, &recv_size, (k_tick_t)timeout));
}

#else

cos_status_t cos_mq_create(const char* name, size_t msg_size,
                    size_t max_msgs, uint8_t flag, cos_mq_t* mq)
{
    return COS_ERR;
}

cos_status_t cos_mq_delete(cos_mq_t mq)
{
    return COS_ERR;
}

cos_status_t cos_mq_init(const char* name, void *msgpool, size_t msg_size,
                    size_t max_msgs, cos_mq_t mq)
{
    return COS_ERR;
}

cos_status_t cos_mq_deinit(cos_mq_t mq)
{
    return COS_ERR;
}

cos_status_t cos_mq_send(cos_mq_t mq, void* buffer, size_t size)
{
    return COS_ERR;
}

cos_status_t cos_mq_recv(cos_mq_t mq, void* buffer,
        size_t size, int32_t timeout)
{
    return COS_ERR;
}

#endif

cos_status_t cos_work_init(cos_work_t work, cos_work_handle_t work_func, void *work_data)
{
    return COS_ERR;
}

cos_status_t cos_work_dowork(cos_work_t work, cos_tick_t time)
{
    return COS_ERR;
}

cos_status_t cos_work_submit(cos_wq_t wq, cos_work_t work, cos_tick_t time)
{
    return COS_ERR;
}

cos_status_t cos_work_cancel(cos_wq_t wq, cos_work_t work)
{
    return COS_ERR;
}

cos_status_t cos_workqueue_create(cos_wq_t* wq, const char *name,  uint16_t stack_size, uint8_t priority)
{
    return COS_ERR;
}

cos_status_t cos_workqueue_destroy(cos_wq_t wq)
{
    return COS_ERR;
}

#if TOS_CFG_MUTEX_EN > 0u

cos_status_t cos_mutex_create(cos_mutex_t *mutex, char *name, uint32_t options)
{
    return errno_knl2cosit(tos_mutex_create_dyn((k_mutex_t **)mutex));
}

cos_status_t cos_mutex_delete(cos_mutex_t mutex)
{
    return errno_knl2cosit(tos_mutex_destroy_dyn((k_mutex_t *)mutex));
}

cos_status_t cos_mutex_init(cos_mutex_t mutex, char *name, uint32_t options)
{
    return errno_knl2cosit(tos_mutex_create((k_mutex_t *)mutex));
}

cos_status_t cos_mutex_deinit(cos_mutex_t mutex)
{
    return errno_knl2cosit(tos_mutex_destroy((k_mutex_t *)mutex));
}

cos_status_t cos_mutex_lock(cos_mutex_t mutex, cos_tick_t timeout)
{
    return errno_knl2cosit(tos_mutex_pend_timed((k_mutex_t *)mutex, (k_tick_t)timeout));
}

cos_status_t cos_mutex_unlock(cos_mutex_t mutex)
{
    return errno_knl2cosit(tos_mutex_post((k_mutex_t *)mutex));
}

#else

cos_status_t cos_mutex_create(cos_mutex_t *mutex, char *name, uint32_t options)
{
    return COS_ERR;
}

cos_status_t cos_mutex_delete(cos_mutex_t mutex)
{
    return COS_ERR;
}

cos_status_t cos_mutex_init(cos_mutex_t mutex, char *name, uint32_t options)
{
    return COS_ERR;
}

cos_status_t cos_mutex_deinit(cos_mutex_t mutex)
{
    return COS_ERR;
}

cos_status_t cos_mutex_lock(cos_mutex_t mutex, cos_tick_t timeout)
{
    return COS_ERR;
}

cos_status_t cos_mutex_unlock(cos_mutex_t mutex)
{
    return COS_ERR;
}

#endif /* TOS_CFG_MUTEX_EN */

#if TOS_CFG_SEM_EN > 0u

cos_status_t cos_sem_create(cos_sem_t *sem, char *name, uint32_t init_count, uint32_t max_count)
{
    return errno_knl2cosit(tos_sem_create_max_dyn((k_sem_t **)sem, (k_sem_cnt_t)init_count, (k_sem_cnt_t)max_count));
}

cos_status_t cos_sem_delete(cos_sem_t sem)
{
    return errno_knl2cosit(tos_sem_destroy_dyn((k_sem_t *)sem));
}

cos_status_t cos_sem_init(cos_sem_t sem, char *name, uint32_t init_count, uint32_t max_count)
{
    return errno_knl2cosit(tos_sem_create_max((k_sem_t *)sem, (k_sem_cnt_t)init_count, (k_sem_cnt_t)max_count));
}

cos_status_t cos_sem_deinit(cos_sem_t sem)
{
    return errno_knl2cosit(tos_sem_destroy((k_sem_t *)sem));
}

cos_status_t cos_sem_wait(cos_sem_t *sem, cos_tick_t timeout)
{
    return errno_knl2cosit(tos_sem_pend((k_sem_t *)sem, (k_tick_t)timeout));
}

cos_status_t cos_sem_release(cos_sem_t sem)
{
    return errno_knl2cosit(tos_sem_post((k_sem_t *)sem));
}

#else

cos_status_t cos_sem_create(cos_sem_t *sem, char *name, uint32_t init_count, uint32_t max_count)
{
    return COS_ERR;
}

cos_status_t cos_sem_delete(cos_sem_t sem)
{
    return COS_ERR;
}

cos_status_t cos_sem_init(cos_sem_t sem, char *name, uint32_t init_count, uint32_t max_count)
{
    return COS_ERR;
}

cos_status_t cos_sem_deinit(cos_sem_t sem)
{
    return COS_ERR;
}

cos_status_t cos_sem_wait(cos_sem_t *sem, cos_tick_t timeout)
{
    return COS_ERR;
}

cos_status_t cos_sem_release(cos_sem_t sem)
{
    return COS_ERR;
}

#endif /* TOS_CFG_SEM_EN */

#if TOS_CFG_EVENT_EN > 0

cos_status_t cos_event_create(cos_event_t *event, char *name, cos_event_flag_t init_flag)
{
    return errno_knl2cosit(tos_event_create_dyn((k_event_t **)event, (k_event_flag_t)init_flag));
}

cos_status_t cos_event_delete(cos_event_t event)
{
    return errno_knl2cosit(tos_event_destroy_dyn((k_event_t *)event));
}

cos_status_t cos_event_init(cos_event_t event, char *name, cos_event_flag_t init_flag)
{
    return errno_knl2cosit(tos_event_create((k_event_t *)event, (k_event_flag_t)init_flag));
}

cos_status_t cos_event_deinit(cos_event_t event)
{
    return errno_knl2cosit(tos_event_destroy((k_event_t *)event));
}

cos_status_t cos_event_wait(cos_event_t event, cos_event_flag_t expect_flag, cos_tick_t timeout, cos_event_flag_t *match_flag, uint32_t options)
{
    k_opt_t opt;

    if (options == COS_EVENT_WAIT_ANY) {
        opt |= TOS_OPT_EVENT_PEND_ANY;
    } else if (options == COS_EVENT_WAIT_ALL) {
        opt |= TOS_OPT_EVENT_PEND_ALL;
    }

    if (options == COS_EVENT_WAIT_CLR) {
        opt |= TOS_OPT_EVENT_PEND_CLR;
    }

    return errno_knl2cosit(tos_event_pend((k_event_t *)event, 
                             (k_event_flag_t)expect_flag, (k_event_flag_t *)match_flag, 
                             (k_tick_t)timeout, opt));
}

cos_status_t cos_event_release(cos_event_t event, cos_event_flag_t flag, uint32_t options)
{
    if (options == COS_EVENT_RELEASE_KEEP) {
        return errno_knl2cosit(tos_event_post_keep((k_event_t *)event, (k_event_flag_t)flag));
    } else {
        return errno_knl2cosit(tos_event_post((k_event_t *)event, (k_event_flag_t)flag));
    }
}

#else

cos_status_t cos_event_create(cos_event_t *event, char *name, cos_event_flag_t init_flag)
{
    return COS_ERR;
}

cos_status_t cos_event_delete(cos_event_t event)
{
    return COS_ERR;
}

cos_status_t cos_event_init(cos_event_t event, char *name, cos_event_flag_t init_flag)
{
    return COS_ERR;
}

cos_status_t cos_event_deinit(cos_event_t event)
{
    return COS_ERR;
}

cos_status_t cos_event_wait(cos_event_t event, cos_event_flag_t expect_flag, cos_tick_t timeout, cos_event_flag_t *match_flag, uint32_t options)
{
    return COS_ERR;
}

cos_status_t cos_event_release(cos_event_t event, cos_event_flag_t flag, uint32_t options)
{
    return COS_ERR;
}

#endif
/*
 * Copyright (C) XXXX
 *
 * SPDX-License-Identifier: XXXX
 *
 * Version: 1.0.0
 *
 * Change log:
 *
 * Date         Author      Note
 * 2022-07-12   all         first version
 */

#ifndef _COSIT_PORT_TYPES_H
#define _COSIT_PORT_TYPES_H

//#define COS_USE_AOS /* AliOS Things */
//#define COS_USE_TOS /* TencentOS-tiny */
//#define COS_USE_RTTHREAD /* RT-Thread */

/* 提供统一的内核对象类型名称，为调用静态内存API前先开辟合适大小的内存，同时屏蔽底层OS的差异，
 * 静态内存API：cos_task_init, cos_timer_init, cos_mb_init, cos_mq_init, cos_work_init,
 * cos_mutex_init, cos_sem_init, cos_event_init。
 * 只有当用户使用静态内存类型的API时需要此头文件， 否则不需要。
 *
 * 使用举例：
 * cos_sem_real_t test_sem;
 * ret = cos_sem_init((cos_sem_t)&test_sem, "test_sem", 1, 0xffff);
 * cos_sem_wait((cos_sem_t)&test_sem, COS_WAIT_FOREVER);
 */

#ifdef COS_USE_AOS
#include <k_api.h>
/* 提供底层OS内核对象的真实数据类型, 用于 */
typedef ktask_t  cos_task_real_t;
typedef ktimer_t cos_timer_real_t;
typedef kmutex_t cos_mutex_real_t;
typedef ksem_t   cos_sem_real_t;
typedef kevent_t cos_event_real_t;
typedef kbuf_queue_t cos_mq_real_t;
typedef kqueue_t cos_mb_real_t;
typedef kwork_t  cos_work_real_t;
typedef kworkqueue_t cos_wq_real_t;

#elif defined (COS_USE_TOS)
#include "tos_k.h"
typedef k_task_t  cos_task_real_t;
typedef k_timer_t cos_timer_real_t;
typedef k_mutex_t cos_mutex_real_t;
typedef k_sem_t   cos_sem_real_t;
typedef k_event_t cos_event_real_t;
typedef k_mail_q_t cos_mq_real_t;
typedef k_msg_q_t cos_mb_real_t;

#elif defined (COS_USE_RTTHREAD)

#else

#warning "The underlying OS is not defined, so you should take care of the real type of kernel object."

#endif

#endif /*_COSIT_PORT_TYPES_H*/

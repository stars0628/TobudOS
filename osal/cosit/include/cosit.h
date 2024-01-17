/*
 * Copyright (C) XXXX
 *
 * SPDX-License-Identifier: XXXX
 *
 * Version: 1.1.1
 *
 * Change log:
 *
 * Date         Author      Note
 * 2022-06-15   all         first version
 * 2022-07-12   Mculover666 Add cos_sem_release_all and cos_tick_get API.
 * 2022-07-26   Mculover666 Fix the type error of sem.
 */

#ifndef _COSIT_H
#define _COSIT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup cos_status
 * COSIT标准状态码
 * @{
 */
/**
 * 状态码格式说明：
 * 0:表示成功，错误码均为负值，错误码用16位（不算符号位）的16进制表示。
 * 其中高1~4位：标识模块， 其余为错误码（后续也可扩展为其他标识）。
 * 高1~4    模块名称
 *   0      通用错误
 *   1      任务管理
 *   2      内存管理
 *   3      软件定时器
 *   4      互斥量
 *   5      信号量
 *   7      事件
 *   8      消息队列
 *   9      邮箱
 *   A      工作队列
 */
#define COS_OK 0 /**< 成功 */

/* 通用错误码 */
#define COS_ERR         (-0x0001)  /**< 未知错误 */
#define COS_ERR_PARAM   (-0x0002)  /**< 非法参数 */
#define COS_ERR_NOMEM   (-0x0003)  /**< 内存不足 */
#define COS_ERR_TIMEOUT (-0x0003)  /**< 超时 */
#define COS_ERR_ISR     (-0x0004)  /**< 不允许在中断上下文中使用 */
#define COS_ERR_PERM    (-0x0005)  /**< 权限不允许 */

/* 互斥量特有错误码 */
#define COS_ERR_MUTEX_NOT_OWNER (-0x4001) /**< 不是互斥锁的所有者 */
#define COS_ERR_MUTEX_NESTING   (-0x4002) /**< 已经拥有互斥锁，导致嵌套但并未指定嵌套选项 */

/* 邮箱特有错误码 */
#define COS_ERR_MB_FULL (-0x9001)

/**
 * @}
 */


/**
 * 表示永久阻塞等待，直到获得系统资源（如互斥量，信号量，事件，队列消息等）才返回。
 */
#define COS_WAIT_FOREVER  ((cos_tick_t)-1)

/**
 * 表示非阻塞等待，当不能获得系统资源（如互斥量，信号量，事件，队列消息等）时不会等待，而是立即返回。
 */
#define COS_NO_WAIT  (0x0U)

/**
 * @addtogroup cos_timer
 * 提供软件定时器相关的标准定义
 * @{
 */
#define COS_TIMER_OPTION_ACTIVATE    (0x1u << 0) /**< 创建时激活选项 */
#define COS_TIMER_OPTION_DEACTIVATE  (0x1u << 1) /**< 创建时不激活选项 */
/**
 * @}
 */

/**
 * @addtogroup cos_mutex
 * 提供互斥锁相关的标准定义
 * @{
 */
#define COS_MUTEX_NEST    (0x1u << 0) /**< 开启嵌套锁支持 */
/**
 * @}
 */

/**
 * @addtogroup cos_sem
 * 提供信号量相关的标准定义
 * @{
 */

/**
 * 用来指定创建一个新的信号量时的最大值，表示信号量无最大值。
 */
#define COS_SEM_NO_MAX    ((uint32_t)-1)

/**
 * @}
 */

/**
 * @addtogroup cos_event
 * 提供事件相关的标准定义
 * @{
 */

/**
 * 用来指定等待事件和释放事件时的选项。
 */
#define COS_EVENT_WAIT_ANY        (0x1u << 0) /**< 等待任意一个事件 */
#define COS_EVENT_WAIT_ALL        (0x1u << 1) /**< 等待所有事件 */
#define COS_EVENT_WAIT_CLR        (0x1u << 2) /**< 等待到事件后清除该事件 */
#define COS_EVENT_RELEASE_KEEP    (0x1u << 3) /**< 释放事件后是否保留该事件 */

/**
 * @}
 */

typedef int32_t cos_status_t; /**< 状态码类型 */

typedef uint64_t cos_tick_t; /**< 系统tick类型 */


/**
 * @addtogroup cos_mb
 * 提供邮箱相关的标准定义
 * @{
 */
typedef void * cos_mb_t; /**< 邮箱句柄 */

#define COS_IPC_FLAG_FIFO 0
#define COS_IPC_FLAG_PRIO 1
/**
 * @}
 */

/**
 * @addtogroup cos_mq
 * 提供消息队列相关的标准定义
 * @{
 */
typedef void * cos_mq_t; /**< 邮箱句柄 */
/**
 * @}
 */

/**
 * @addtogroup cos_wq
 * 提供工作队列相关的标准定义
 * @{
 */
typedef void * cos_wq_t; /**< 工作队列句柄 */
typedef void * cos_work_t; /**< 工作项句柄 */
typedef void (*cos_work_handle_t) (void *); /**< 任务入口函数的原型。*/
/**
 * @}
 */

/**
 * 用来指定创建一个新任务时任务属性的options参数，表示任务创建后处于挂起状态，不会被调度执行，
 * 直到被恢复。如果没有指定此选项（默认），任务创建后即处于就绪状态，等待调度运行。
 */
#define COS_TASK_OPTION_NORUN  (0x1U << 0)

/**
 * 最大任务优先级。任务优先级从0 ~ COS_TASK_PRIORITY_MAX优先级别递减，即COS_TASK_PRIORITY_MAX优先级最低。
 * 如内核在实现时，若其优先级级别小于`COS_TASK_PRIORITY_MAX`, 当应用传入的优先级超过内核实现的优先级时，
 * 内核应调整为应用可设置的最低优先级。如果应用不关心任务的优先级，则可以使用`cos_task_attr_init`初始化为
 * 默认优先级即可，不同内核在实现时，其默认优先级可以不同。
 */
#define COS_TASK_PRIORITY_MAX  100

/**
 * 完全公平调度策略（非实时）。采用完全公平调度算法，对任务进行调度。适用于任务比较多的场景，
 * 开发者不需要设计每个任务的合理优先级，由调度算法以完全公平的策略调度任务，但开发者可以微调任务的权重，
 * 以相对增加或减少任务的运行时间片。
 */
#define COS_TASK_SCHED_CFS 0x0

/**
 * 实时，FIFO调度策略。当同优先级的多个任务同时就绪时，按先进先出的顺序依次执行。
 */
#define COS_TASK_SCHED_FIFO 0x01

/**
 * 实时，Round-Robin调度策略. 当同优先级的多个任务同时就绪时，多个任务按时间片轮流执行。
 */
#define COS_TASK_SCHED_RR 0x02

/**
 * @}
 */

/**
 * @addtogroup cos_task
 * 提供任务管理相关的标准定义
 * @{
 */

/**
 * 任务的句柄，代表一个任务，调用任务创建函数后返回。可以通过句柄对任务进行操作，如挂起，删除等。
 */
typedef void * cos_task_t;

typedef void (*cos_task_entry_t) (void *); /**< 任务入口函数的原型。*/

typedef struct cos_task_attr {
    const char *name;  /**< 任务名称(可以为NULL) */
    size_t stack_size; /**< 任务栈的大小（单位：Byte） */
    uint8_t priority;  /**< 任务的优先级 */
    cos_tick_t tick; /**< 任务使用RR调度策略时，占用的系统tick数 */
    uint8_t sched_policy; /**< 调度策略 @ref COS_TASK_SCHED_RR / @ref COS_TASK_SCHED_FIFO / @ref COS_TASK_SCHED_CFS */
    uint8_t cpu_bind; /**< 绑定的CPU号，适用于多核系统 */
    uint8_t inited;   /**< 标识该结构是否被初始化 */
    uint32_t options; /**< 任务创建时的选项 */
    void *reserved;   /**< 预留字段（可扩展）*/
} cos_task_attr_t;
/**
 * @}
 */


/**
 * @addtogroup cos_timer
 * 提供软件定时器相关的标准定义
 * @{
 */
typedef void * cos_timer_t; /**< 软件定时器句柄 */

typedef void (*cos_timer_cb_t)(void *arg); /**< 软件定时器的回调函数类型 */
/**
 * @}
 */

/**
 * @addtogroup cos_mutex
 * 提供互斥锁相关的标准定义
 * @{
 */
typedef void * cos_mutex_t; /**< 互斥锁句柄 */

/**
 * @}
 */

/**
 * @addtogroup cos_sem
 * 提供信号量相关的标准定义
 * @{
 */
typedef void * cos_sem_t; /**< 信号量句柄 */

/**
 * @}
 */

/**
 * @addtogroup cos_event
 * 提供事件相关的标准定义
 * @{
 */
typedef void * cos_event_t; /**< 事件句柄 */

typedef uint32_t cos_event_flag_t;  /**< 事件标志 */

/**
 * @}
 */

/**
 * @brief 毫秒数转为系统tick数
 *
 * 把毫秒数转成系统tick数。在物联网操作系统中系统tick一般用于内核调度的基本单位，
 * 其频率在系统中是可以配置的。根据系统tick一般只能达到毫秒精度。
 *
 * @param[in]  ms    毫秒数
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  总是成功，返回转换得到的系统tick数
 */
cos_tick_t cos_ms_to_tick(uint64_t ms);

/**
 * @brief 系统tick数转为毫秒数
 *
 * 把系统tick数转成毫秒数。在物联网操作系统中系统tick一般用于内核调度的基本单位，
 * 其频率在系统中是可以配置的。根据系统tick一般只能达到毫秒精度。
 *
 * @param[in]  tick    系统tick数
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  总是成功，返回转换得到的毫秒数
 */
uint64_t cos_tick_to_ms(cos_tick_t tick);

/**
 * @brief 获取当前系统tick数
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  总是成功，返回得到的当前系统tick数
 */
cos_tick_t cos_tick_get(void);

/**
 * @addtogroup cos_task
 * 提供任务管理相关的标准定义
 * @{
 */

/**
 * @brief 初始化一个任务属性
 *
 * 初始化一个任务属性，为任务属性设置默认值。
 *
 * @param[in]   attr   任务属性的句柄
 *
 * @note 可以在中断上下文中使用。
 * @note 此函数与创建任务同时使用。
 * @note 每个内核实现的默认值可能不同。
 *
 * @return  无
 */
void cos_task_attr_init(cos_task_attr_t *attr);

/**
 * @brief 创建新任务
 *
 * 创建一个新任务并初始化，新任务的句柄由参数 @c task 返回，通过任务句柄 @c task 对新任务进行后续的操作。
 * 新任务成功创建后，会进入就绪队列等待调度执行。如果选项 @ref COS_TASK_OPTION_NORUN 被指定，
 * 则任务创建后处于挂起状态，不会被调度执行，需要应用恢复该任务后，才能进入就绪队列等待调度执行。
 * 当被调度到时从入口函数fn开始运行。从任务入口函数返回，代表此任务退出，系统会清理该任务的资源。
 * 该函数在创建时内部会动态分配需要的内存，如果不想使用动态内存，请使用 @ref cos_task_init 。
 *
 * @param[out]  task   新任务的句柄，后续对此任务的操作通过该句柄进行
 * @param[in]   fn     新任务的入口函数，新任务从此函数开始运行
 * @param[in]   arg    新任务的入口函数的参数
 * @param[in]   attr   新任务的属性
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 *
 * @see cos_task_init
 */
cos_status_t cos_task_create(cos_task_t *task, cos_task_entry_t fn, void *arg, cos_task_attr_t *attr);

/**
 * @brief 删除任务
 *
 * 主动删除一个任务回收其资源。
 *
 * @param[in]  task       任务的句柄
 *
 * @note 不可以在中断上下文中使用。
 * @note 不能删除idle任务。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_delete(cos_task_t task);

/**
 * @brief 初始化一个任务
 *
 * 初始化一个任务。与创建任务类似，区别是调用者为新任务的控制块分配内存，其大小为任务控制块的数据结构大小，
 * 不同内核其任务控制块大小不同。通过参数 @c task 指定任务的控制块地址。 调用者还需要为新任务分配栈内存空间，
 * 并通过参数 @c stack_buf 指定栈内存空间地址，栈空间大小通过 @c attr 中的stack_size传递。
 *
 * @param[in]  task       新任务的句柄，后续对此任务的操作通过该句柄进行
 * @param[in]  fn         新任务的入口函数，新任务从此函数开始运行
 * @param[in]  arg        新任务的入口函数的参数
 * @param[in]  attr       新任务的属性
 * @param[in]  stack_buf  调用者指定的任务栈空间
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 *
 * @see cos_task_init
 */
cos_status_t cos_task_init(cos_task_t task, cos_task_entry_t fn, void *arg,
                           cos_task_attr_t *attr, void *stack_buf);

/**
 * @brief 反初始化一个任务
 *
 * 反初始化一个任务。与 @ref cos_task_init 配套使用，销毁其初始化过的任务，把此任务置为销毁状态。
 * 如果该任务销毁前处于就绪状态，会把该任务从系统就绪队列中移除。根据互斥锁的类型（鲁棒性互斥锁），
 * 还会释放其拥有的互斥锁，导致唤醒等待该锁的任务。
 * 但不会释放任务控制块task和任务栈的内存空间，这部分内存应由调用者负责释放。
 *
 * @param[in]  task       新任务的句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 *
 * @see cos_task_init
 */
cos_status_t cos_task_deinit(cos_task_t task);

/**
 * @brief 退出任务
 *
 * 主动退出当前任务，使系统回收该任务的资源。其作用与从该任务的入口函数返回相同。
 * 参数 @c status 作为后续扩展使用，目前并不支持获取一个任务退出时的状态码。
 *
 * @param[in]  status       任务退出状态码
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
void cos_task_exit(int32_t status);

/**
 * @brief 挂起任务
 *
 * 挂起一个任务，暂停该任务的执行。可以挂起自身或其他任务，但不允许挂起系统idle任务。
 * 当挂起系统其他内置任务时请谨慎，可能会引起系统错误。
 *
 * @param[in]  task       任务的句柄
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_suspend(cos_task_t task);

/**
 * @brief 恢复任务
 *
 * 恢复一个任务，使其处于就绪状态，等待调度执行。
 *
 * @param[in]  task       任务的句柄
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_resume(cos_task_t task);

/**
 * @brief 当前任务让出CPU
 *
 * 使当前正在运行的任务让出CPU，并把当前任务置于其静态优先级的就绪队列的末尾。
 * 重新调度，其他任务得到CPU运行。
 *
 * @param   无
 *
 * @note 可以在中断上下文中使用。
 * @note 根据调度策略，如果该任务是其静态优先级的就绪队列中唯一任务，它会被再次调度到，继续运行。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_yield(void);

/**
 * @brief 获取当前任务自身的句柄
 *
 * 返回当前任务自身的句柄，用于对自身任务的操作，如挂起和恢复等。
 *
 * @param   无
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  函数总是执行成功并返回当前任务自身的句柄。
 */
cos_task_t cos_task_self(void);

/**
 * @brief 获取一个任务的名称
 *
 * 返回一个任务的名称，调用者应该为存放任务名称分配足够的内存，
 * 并把内存首地址和大小通过参数 @c buf 和 @c buf_size 传入。
 *
 * @param[in]  task       任务的句柄
 * @param[out] buf        存放任务名称的内存
 * @param[in]  buf_size   存放任务名称的内存大小
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_get_name(cos_task_t task, char *buf, size_t buf_size);

/**
 * @brief 根据名称查找任务
 *
 * 根据任务名称 @c name 查找任务的句柄。
 *
 * @param[in]  name       任务的名称
 *
 * @note 可以在中断上下文中使用。
 * @note 当系统中存在多个具有相同名称的任务时，函数会执行成功，但返回哪一个任务
 *       的句柄是不确定的，不同的内核实现会有所不同。
 *
 * @return  成功则返回任务的句柄， 失败则返回 NULL
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_task_t cos_task_find(const char *name);

/**
 * @brief 使当前任务睡眠一段时间
 *
 * 使当前任务睡眠 @c tick 个系统tick的时间，等时间超期或被唤醒后再次进入就绪状态，等待调度执行。
 * 如果 @c tick 为0，则等同于 @ref cos_task_yield 。如果 @c tick 为 @ref COS_WAIT_FOREVER , 则等同于 @ref cos_task_suspend。
 *
 * @param[in]  tick       系统tick个数
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败，>0: 提前被唤醒时剩余的tick数
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_sleep(cos_tick_t tick);

/**
 * @brief 唤醒任务
 *
 * 唤醒一个任务。可以用来提前唤醒一个睡眠的任务。
 *
 * @param[in]  task       任务的句柄
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_wakeup(cos_task_t task);

/**
 * @brief 获取一个任务的优先级
 *
 * 获取一个任务的优先级，可以是自身或其他任务。获取的是任务此时的动态优先级，
 * 不一定与其创建时指定的静态优先级相同，如任务此时有优先级继承发生。
 *
 * @param[in]  task       任务的句柄
 * @param[out]  priority   获取到的任务动态优先级
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_get_priority(cos_task_t task, uint8_t *priority);

/**
 * @brief 设置一个任务的优先级
 *
 * 设置一个任务的优先级，可以是自身或其他任务。改变的是任务的静态优先级。
 *
 * @param[in]  task       任务的句柄
 * @param[in]  priority   获取到的任务动态优先级
 *
 * @note 可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_set_priority(cos_task_t task, uint8_t priority);

/**
 * @brief 获取任务的剩余时间片
 *
 * 获取一个使用RR调度策略的任务在本次轮转中剩余时间片。
 *
 * @param[in]  task       任务的句柄
 * @param[out] time_slice 获取到的任务的剩余时间片
 *
 * @note 可以在中断上下文中使用。
 * @note 此函数仅适用于采用RR调度策略的任务， 对于其他任务，剩余时间片返回0.
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_get_time_slice(cos_task_t task, cos_tick_t *time_slice);

/**
 * @brief 获取任务的剩余时间片
 *
 * 设置一个使用RR调度策略的任务在一次轮转中所分配的时间片。
 *
 * @param[in]  task       任务的句柄
 * @param[in]  time_slice 获取到的任务的剩余时间片
 *
 * @note 可以在中断上下文中使用。
 * @note 此函数仅适用于采用RR调度策略的任务，对于其他任务，返回错误。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_task_set_time_slice(cos_task_t task, cos_tick_t time_slice);

/**
 * @}
 */

/**
 * @addtogroup cos_timer
 * 提供软件定时器相关的标准定义
 * @{
 */

/**
 * @brief 创建软件定时器
 *
 * 创建一个软件定时器，当定时器超期时系统会自动执行回调函数 @c cb 并传入参数 @c arg 。
 * 该函数在创建时内部会动态分配需要的内存，如果不想使用动态内存，请使用 @ref cos_timer_init 。
 *
 * @param[out]  timer    定时器句柄
 * @param[in]   name     定时器名称
 * @param[in]   cb       定时器的回调函数
 * @param[in]   arg      定时器的回调函数的参数
 * @param[in]   initial  定时器的初始定时时间，即从定时器启动到第一次触发的时间间隔，单位：tick个数
 * @param[in]   period   定时器的周期定时时间，即周期性触发的时间间隔，单位：tick个数。0：代表单次触发
 * @param[in]   options  定时器的选项，如创建时激活 @ref COS_TIMER_OPTION_ACTIVATE，
 *                       或者不激活 @ref COS_TIMER_OPTION_DEACTIVATE
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR 定时器未知错误
 *
 * @see cos_timer_init
 */
cos_status_t cos_timer_create(cos_timer_t *timer, const char *name, cos_timer_cb_t cb,
    void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options);

/**
 * @brief 删除软件定时器
 *
 * 删除一个软件定时器。系统会先停止该定时器，然后删除回收其资源。
 *
 * @param[in]   timer    定时器句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 定时器未知错误
 */
cos_status_t cos_timer_delete(cos_timer_t timer);

/**
 * @brief 初始化软件定时器
 *
 * 初始化一个软件定时器，当定时器超期时系统会自动执行回调函数 @c cb ， 并传入参数 @c arg 。
 * 与创建一个软件定时器类似，区别是定时器的控制块内存时由调用者传入的，一般用于需要静态内存的场景。
 *
 * @param[in]   timer    定时器句柄
 * @param[in]   name     定时器名称
 * @param[in]   cb       定时器的回调函数
 * @param[in]   arg      定时器的回调函数的参数
 * @param[in]   initial  定时器的初始定时时间，即从定时器启动到第一次触发的时间间隔，单位：tick个数
 * @param[in]   period   定时器的周期定时时间，即周期性触发的时间间隔，单位：tick个数。0：代表单次触发
 * @param[in]   options  定时器的选项，如创建时激活 @ref COS_TIMER_OPTION_ACTIVATE，
 *                       或者不激活 @ref COS_TIMER_OPTION_DEACTIVATE
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR 定时器未知错误
 *
 * @see cos_timer_create
 */
cos_status_t cos_timer_init(cos_timer_t timer, const char *name, cos_timer_cb_t cb,
    void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options);

/**
 * @brief 反初始化软件定时器
 *
 * 反初始化一个定时器。系统会先停止该定时器，从系统定时器列表中移除。但不会释放 @c timer 的内存空间，应由调用者释放。
 *
 * @param[in]   timer    定时器句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 定时器未知错误
 */
cos_status_t cos_timer_deinit(cos_timer_t timer);

/**
 * @brief 启动软件定时器
 *
 * 启动一个定时器，系统开始计时，直到定时器超期时执行应用设置的回调函数。
 *
 * @param[in]   timer    定时器句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 定时器未知错误
 */
cos_status_t cos_timer_start(cos_timer_t timer);

/**
 * @brief 停止软件定时器
 *
 * 停止一个定时器，系统停止为该定时器计时，但不会删除它。
 *
 * @param[in]   timer    定时器句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 定时器未知错误
 */
cos_status_t cos_timer_stop(cos_timer_t timer);

/**
 * @brief 修改软件定时器
 *
 * 修改一个定时器。一般用于修改其周期性定时时间，如果该定时器创建后还没有启动，也可以修改其初始定时时间。
 *
 * @param[in]   timer    定时器句柄
 * @param[in]   initial  定时器的初始定时时间，即从启动到第一次触发的时间间隔，单位：tick个数
 * @param[in]   period   定时器的周期定时时间，即周期性触发的时间间隔，单位：tick个数。0：代表单次触发
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 定时器未知错误
 */
cos_status_t cos_timer_change(cos_timer_t timer, cos_tick_t initial, cos_tick_t period);

/**
 * @brief 获取定时器的时间
 *
 * 返回一个定时器的时间信息，包括离下次触发还剩余的时间（系统tick），和周期触发的时间间隔（系统tick）。
 *
 * @param[in]   timer     定时器句柄
 * @param[out]  remaining 离下次触发还剩余的时间，单位：tick个数
 * @param[out]  period    周期触发的时间间隔，单位：tick个数。0：代表单次触发
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 定时器未知错误
 */
cos_status_t cos_timer_get_time(cos_timer_t timer, cos_tick_t *remaining, cos_tick_t *period);

/**
 * @}
 */

/**
 * @addtogroup cos_memory
 * 提供内存管理相关的标准定义
 * @{
 */

/**
 * @brief 申请一块连续的内存
 *
 * 从系统堆中申请一块 @c size 大小连续的内存，成功返回申请到内存的首地址，失败返回NULL。
 *
 * @param[in]   size     想申请内存的大小（单位：Byte）
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  成功返回申请到内存的首地址，失败则返回NULL。
 */
void *cos_malloc(size_t size);

/**
 * @brief 改变一块内存的大小
 *
 * 改变一块内存 @c ptr 的大小， @c ptr 应该是之前动态申请内存的返回值。新的内存大小可能扩大或缩小，
 * 扩大时原有内存大小的内容保持不变且新增加的内存没有初始化。缩小时，被保留的内存的内容保持不变。
 * 如果参数 @c ptr 为NULL，不论new_size为多少，则等于 @c cos_malloc(new_size) ，
 * 如果参数 @c new_size 为0，则等于 @c cos_free(prt) 。如果该函数返回的新的内存首地址与原有地址不同，
 * 则原有内存被自动释放。
 *
 * @param[in]   ptr      之前动态申请到的内存首地址
 * @param[in]   new_size 内存新大小
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  成功返回新申请到内存的首地址，失败则返回NULL。
 */
void *cos_realloc(void *ptr, size_t new_size);

/**
 * @brief 申请多块连续的内存
 *
 * 从系统堆中申请 @c count 个长度为 @c size 的连续的内存，最终申请到内存大小为count * size个字节。
 *
 * @param[in]   count 申请内存块的个数
 * @param[in]   size  每个内存块的大小
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  成功返回申请到内存的首地址，失败则返回NULL。
 */
void *cos_calloc(size_t count, size_t size);

/**
 * @brief 申请内存并按要求地址对齐
 *
 * 从系统堆内存中申请 @c size 字节的连续内存，并按 @c alignment 字节对齐，
 * 申请到的内存首地址由 @c *memptr 返回。如果参数 @c size 为0，则函数立即返回错误码，
 * @c *memptr 为NULL。 参数 @c alignment 应该是sizeof(void*)的倍数且是2的幂，
 * 否则立即返回错误， @c *memptr 为NULL。 释放内存通过 @ref cos_free 函数释放。
 *
 * @param[out]  memptr 返回申请到的内存首地址
 * @param[in]   size   想申请内存的大小（单位：Byte）
 * @param[in]   alignment  内存地址对齐大小（单位：Byte）
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 内存管理未知错误
 */
cos_status_t cos_malloc_align(void **memptr, size_t size, size_t alignment);

/**
 * @brief 释放内存
 *
 * 释放由参数 @c ptr 指向的一块内存， @c ptr 应该是之前动态申请函数的返回值。
 *
 * @param[in]   ptr 动态申请到的内存首地址
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  无
 */
void cos_free(void *ptr);

/**
 * @}
 */

/**
 * @addtogroup cos_mb
 * 提供邮箱相关的标准定义
 * @{
 */

/**
 * @brief 创建邮箱
 *
 * 从系统堆内存中申请空间创建邮箱控制块
 *
 * @param[in]   name 邮箱名称
 *
 * @param[in]   size 邮箱容量
 *
 * @param[in]  flag 邮箱标志，如： COS_IPC_FLAG_FIFO 或 COS_IPC_FLAG_PRIO
 *
 * @param[out]  mb 邮箱句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mb_create(const char* name, size_t msg_size,
                    uint8_t flag, cos_mb_t *mb);

/**
 * @brief 创建邮箱
 *
 * 删除邮箱对象并释放系统堆内存空间
 *
 *
 * @param[in]  mb 邮箱句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mb_delete(cos_mb_t mb);

/**
 * @brief 创建邮箱
 *
 * 从系统堆内存中申请空间创建邮箱控制块
 *
 * @param[in]   name 邮箱名称
 *
 * @param[in]   msgpool 缓冲区指针
 *
 * @param[in]  msg_size 邮箱容量
 *
 * @param[in]  flag 邮箱工作方式
 *
 * @param[out]  mb 邮箱句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mb_init(const char* name, size_t msg_size,
                    uint8_t flag, cos_mb_t *mb);

/**
 * @brief 去初始化邮箱
 *
 * 去初始化一个邮箱
 *
 * @param[in]  mb 邮箱句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mb_deinit(cos_mb_t mb);

/**
 * @brief 发送邮件
 *
 * 发送邮件，当邮箱中的邮件已经满时，发送邮件的线程或者中断程序会收到 cos_ERROR_mb_FULL 的返回值。
 *
 * @param[in]  mb 邮箱句柄
 *
 * @param[in]  value 邮件内容
 *
 * @note    无
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mb_send(cos_mb_t mb, uint32_t value);

/**
 * @brief 接收邮件
 *
 * 当邮箱中有邮件时，接收者才能接收邮件，否则接收者会根据超时时间设置，或挂起在邮箱的等待线程队列上，或直接返回。
 *
 * @param[in]  mb 邮箱句柄
 *
 * @param[out]  value 邮件内容
 *
 * @param[in]  timeout 指定的超时时间
 *
 * @note    不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mb_recv(cos_mb_t mb, uint32_t* value,
        int32_t timeout);
/**
 * @}
 */

/**
 * @addtogroup cos_mq
 * 提供消息队列相关的标准定义
 * @{
 */
/**
 * @brief 创建消息队列
 *
 * 从系统堆内存中申请空间创建消息队列控制块
 *
 * @param[in]   name 消息队列名称
 *
 * @param[in]   msg_size 单条消息的最大长
 *
 * @param[in]   max_msgs 消息队列的最大个数
 *
 * @param[in]  flag 消息队列工作方式，如： COS_IPC_FLAG_FIFO 或 COS_IPC_FLAG_PRIO
 *
 * @param[out]  mq 消息队列句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mq_create(const char* name, size_t msg_size,
                    size_t max_msgs, uint8_t flag, cos_mq_t* mq);

/**
 * @brief 删除邮箱
 *
 * 从系统堆内存中删除邮箱控制块
 *
 * @param[out]  mq 邮箱句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mq_delete(cos_mq_t mq);

/**
 * @brief 初始化消息队列
 *
 * 初始化消息队列控制块
 *
 * @param[in]   name 消息队列名称
 *
 * @param[in]   msgpool 指向存放消息的缓冲区的指针
 *
 * @param[in]   msg_size 单条消息的最大长
 *
 * @param[in]  flag 消息队列工作方式，如： COS_IPC_FLAG_FIFO 或 COS_IPC_FLAG_PRIO
 *
 * @param[out]  mq 消息队列句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mq_init(const char* name, void *msgpool, size_t msg_size,
                    size_t max_msgs, cos_mq_t mq);

/**
 * @brief 去初始化消息队列
 *
 * 去初始化消息队列控制块
 *
 * @param[out]  mq 邮箱句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mq_delete(cos_mq_t mq);

/**
 * @brief 发送消息
 *
 * 发送消息，线程或者中断服务程序都可以给消息队列发送消息。
 *
 * @param[in]  mq 消息队列的句柄
 *
 * @param[in]  value 消息内容
 *
 * @param[in]  size 消息大小
 *
 * @note    无
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERROR_MQ_DELETED 线程挂起时消息队列已被删除
 * @retval  COS_ERROR_MQ_FULL 消息队列已满
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mq_send(cos_mq_t mq, void* buffer, size_t size);

/**
 * @brief 接收邮件
 *
 * 当邮箱中有邮件时，接收者才能接收邮件，否则接收者会根据超时时间设置，或挂起在邮箱的等待线程队列上，或直接返回。
 *
 * @param[in]  mq 消息队列的句柄
 *
 * @param[out]  buffer 消息内容
 *
 * @param[in]  size 消息大小
 *
 * @param[in]  timeout 指定的超时时间
 *
 * @note    不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERROR_MQ_DELETED 线程挂起时消息队列已被删除
 * @retval  COS_ERROR_MQ_FULL 消息队列已满
 * @retval  COS_ERR_ISR 在中断上下文中调用
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_mq_recv(cos_mq_t mq, void* buffer,
        size_t size, int32_t timeout);

/**
 * @}
 */

/**
 * @addtogroup cos_wq
 * 提供工作队列相关的标准定义
 * @{
 */
/**
 * @brief 初始化工作项
 *
 * 初始化工作项，并绑定工作项的回调函数，以及自定义用户数据
 *
 * @param[in]   work 工作项
 *
 * @param[in]   work_func 工作项的回调函数
 *
 * @param[in]   work_data 用户自定义数据，回调函数参数
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_work_init(cos_work_t work, cos_work_handle_t work_func, void *work_data);

/**
 * @brief 删除邮箱
 *
 * 从系统堆内存中删除邮箱控制块
 *
 * @param[in]  work 工作项
 *
 * @param[in]  time 提交延时，以 tick 为单位
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_work_dowork(cos_work_t work, cos_tick_t time);

/**
 * @brief 向工作队列提交工作项
 *
 * 将 work 指向的工作项提交到 queue 指向的工作队列中，若 time 大于 0，则该提交延时 time 个 tick 之后执行。
 *
 * @param[in]   wq 工作队列
 *
 * @param[in]   work 工作项
 *
 * @param[in]   time 提交延时，以 tick 为单位
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_work_submit(cos_wq_t wq, cos_work_t work, cos_tick_t time);

/**
 * @brief 取消指定工作项
 *
 * 从 queue 指向的工作队列中将 work 指向的工作项移除。
 *
 * @param[in]  queue 工作队列
 *
 * @param[in]  work 工作项
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_work_cancel(cos_wq_t wq, cos_work_t work);

/**
 * @brief 创建工作队列
 *
 * 该函数创建并初始化一个工作队列，利用参数 name，stack_size 和 priority 创建工作队列内部线程，最终返回创建的工作队列。下表描述了该函数的输入参数与返回值
 *
 * @param[out]  wq 工作队列句柄
 *
 * @param[in]  name 线程名字
 *
 * @param[in]  stack_size 线程栈大小
 *
 * @param[in]  priority 线程优先级
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_workqueue_create(cos_wq_t* wq, const char *name,  uint16_t stack_size, uint8_t priority);

/**
 * @brief 工作队列销毁函数
 *
 * 销毁指定工作队列
 *
 * @param[in]  wq 工作队列句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 未知错误
 */
cos_status_t cos_workqueue_destroy(cos_wq_t wq);
/**
 * @}
 */

/**
 * @addtogroup cos_mutex
 * 提供互斥锁相关的标准定义
 * @{
 */

/**
 * @brief 创建互斥锁
 *
 * 创建一个互斥锁。
 * 该函数在创建时内部会动态分配需要的内存，如果不想使用动态内存，请使用 @ref cos_mutex_init 。
 *
 * @param[out]  timer    互斥锁句柄
 * @param[in]   name     互斥锁名称
 * @param[in]   options  互斥锁的选项，如开启嵌套锁支持 @ref COS_MUTEX_NEST
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中使用
 * @retval  COS_ERR 互斥锁未知错误
 *
 * @see cos_mutex_init
 */
cos_status_t cos_mutex_create(cos_mutex_t *mutex, char *name, uint32_t options);

/**
 * @brief 删除互斥锁
 *
 * 删除一个互斥锁。系统会先唤醒所有等待该锁的任务，唤醒原因为互斥锁已被销毁，然后删除回收其资源。
 *
 * @param[in]   mutex    互斥锁句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 互斥锁未知错误
 */
cos_status_t cos_mutex_delete(cos_mutex_t mutex);

/**
 * @brief 初始化互斥锁
 *
 * 初始化一个互斥锁。
 * 与创建一个互斥锁类似，区别是互斥锁的控制块内存是由调用者传入的，一般用于需要静态内存的场景。
 *
 * @param[in]   mutex    互斥锁句柄
 * @param[in]   name     互斥锁名称
 * @param[in]   options  互斥锁的选项，如开启嵌套锁支持 @ref COS_MUTEX_NEST
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR_ISR 在中断上下文中使用
 * @retval  COS_ERR 互斥锁未知错误
 *
 * @see cos_mutex_create
 */
cos_status_t cos_mutex_init(cos_mutex_t mutex, char *name, uint32_t options);

/**
 * @brief 反初始化互斥锁
 *
 * 反初始化一个互斥锁。系统会先唤醒所有等待该锁的任务，唤醒原因为互斥锁已被销毁。但不会释放 @c mutex 的内存空间，应由调用者释放。
 *
 * @param[in]   mutex    互斥锁句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR 互斥锁未知错误
 */
cos_status_t cos_mutex_deinit(cos_mutex_t mutex);

/**
 * @brief 互斥锁上锁
 *
 * @param[in]   mutex      互斥锁句柄
 * @param[in]   timeout    等待互斥锁的超时时间，单位：tick个数
 *
 * @note 不可以在中断上下文中使用。
 *
 * @retval  COS_ERR 互斥锁未知错误
 * @retval  COS_ERR_TIMEOUT 申请上锁超时
 * @retval  COS_ERR_DESTROY 互斥锁已被销毁
 * @retval  COS_ERR_MUTEX_NESTING 未开启嵌套锁的情况下，进行嵌套申请
 */
cos_status_t cos_mutex_lock(cos_mutex_t mutex, cos_tick_t timeout);

/**
 * @brief 互斥锁解锁
 *
 * @param[in]   mutex      互斥锁句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR 互斥锁未知错误
 * @retval  COS_ERR_DESTROY 互斥锁已被销毁
 * @retval  COS_ERR_MUTEX_NOT_OWNER 未持有该互斥锁
 */
cos_status_t cos_mutex_unlock(cos_mutex_t mutex);

/**
 * @}
 */

/**
 * @addtogroup cos_sem
 * 提供信号量相关的标准定义
 * @{
 */

/**
 * @brief 创建信号量
 *
 * 创建一个信号量。
 * 该函数在创建时内部会动态分配需要的内存，如果不想使用动态内存，请使用 @ref cos_sem_init 。
 *
 * @param[out]  sem         信号量句柄
 * @param[in]   name        信号量名称
 * @param[in]   init_count  信号量初始值
 * @param[in]   max_count   信号量最大值。如果无最大值限制可使用 @ref COS_SEM_NO_MAX。
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR 信号量未知错误
 *
 * @see cos_sem_init
 */
cos_status_t cos_sem_create(cos_sem_t *sem, char *name, uint32_t init_count, uint32_t max_count);

/**
 * @brief 删除信号量
 *
 * 删除一个信号量。系统会先唤醒所有等待该信号量的任务，唤醒原因为信号量已被销毁，然后删除回收其资源。
 *
 * @param[in]   sem    信号量句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 信号量未知错误
 */
cos_status_t cos_sem_delete(cos_sem_t sem);

/**
 * @brief 初始化信号量
 *
 * 初始化一个信号量。
 * 与创建一个信号量类似，区别是信号量的控制块内存是由调用者传入的，一般用于需要静态内存的场景。
 *
 * @param[in]   sem      信号量句柄
 * @param[in]   name     信号量名称
 * @param[in]   init_count  信号量初始值
 * @param[in]   max_count   信号量最大值。如果无最大值限制可使用 @ref COS_SEM_NO_MAX。
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR 信号量未知错误
 *
 * @see cos_sem_create
 */
cos_status_t cos_sem_init(cos_sem_t sem, char *name, uint32_t init_count, uint32_t max_count);

/**
 * @brief 反初始化信号量
 *
 * 反初始化一个信号量。系统会先唤醒所有等待该信号量的任务，唤醒原因为信号量已被销毁。但不会释放 @c sem 的内存空间，应由调用者释放。
 *
 * @param[in]   sem      信号量句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 信号量未知错误
 */
cos_status_t cos_sem_deinit(cos_sem_t sem);

/**
 * @brief 等待信号量
 *
 * @param[in]   sem        信号量句柄
 * @param[in]   timeout    等待信号量的超时时间，单位：tick个数
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 信号量未知错误
 * @retval  COS_ERR_TIMEOUT 申请超时
 * @retval  COS_ERR_DESTROY 信号量已被销毁
 */
cos_status_t cos_sem_wait(cos_sem_t sem, cos_tick_t timeout);

/**
 * @brief 释放信号量
 *
 * 释放一个信号量。系统唤醒等待该信号量的最高优先级任务。
 *
 * @param[in]   sem        信号量句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR 信号量未知错误
 * @retval  COS_ERR_DESTROY 信号量已被销毁
 */
cos_status_t cos_sem_release(cos_sem_t sem);

/**
 * @brief 释放信号量，唤醒所有等待任务
 *
 * 释放一个信号量。系统唤醒等待该信号量的所有任务。
 *
 * @param[in]   sem        信号量句柄
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR 信号量未知错误
 * @retval  COS_ERR_DESTROY 信号量已被销毁
 */
cos_status_t cos_sem_release_all(cos_sem_t sem);

/**
 * @}
 */

/**
 * @addtogroup cos_event
 * 提供事件相关的标准定义
 * @{
 */
/**
 * @brief 创建事件
 *
 * 创建一个事件。
 * 该函数在创建时内部会动态分配需要的内存，如果不想使用动态内存，请使用 @ref cos_sem_init 。
 *
 * @param[out]  event       事件句柄
 * @param[in]   name        事件名称
 * @param[in]   init_flag   事件初始标志
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR 事件未知错误
 *
 * @see cos_event_init
 */
cos_status_t cos_event_create(cos_event_t *event, char *name, cos_event_flag_t init_flag);

/**
 * @brief 删除事件
 *
 * 删除一个事件。系统会先唤醒所有等待该事件的任务，唤醒原因为事件已被销毁，然后删除回收其资源。
 *
 * @param[in]   event    事件句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 事件未知错误
 */
cos_status_t cos_event_delete(cos_event_t event);

/**
 * @brief 初始化事件
 *
 * 初始化一个事件。
 * 与创建一个事件类似，区别是事件的控制块内存是由调用者传入的，一般用于需要静态内存的场景。
 *
 * @param[out]  event       事件句柄
 * @param[in]   name        事件名称
 * @param[in]   init_flag   事件初始标志
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR_NOMEM 内存不足
 * @retval  COS_ERR 事件未知错误
 *
 * @see cos_event_create
 */
cos_status_t cos_event_init(cos_event_t event, char *name, cos_event_flag_t init_flag);

/**
 * @brief 反初始化事件
 *
 * 反初始化一个事件。系统会先唤醒所有等待该事件的任务，唤醒原因为事件已被销毁。但不会释放 @c event 的内存空间，应由调用者释放。
 *
 * @param[in]   event      事件句柄
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 事件未知错误
 */
cos_status_t cos_event_deinit(cos_event_t event);

/**
 * @brief 等待事件
 *
 * @param[in]   event           事件句柄
 * @param[in]   expect_flag     期望期待的事件标志
 * @param[in]   timeout         等待事件的超时时间，单位：tick个数
 * @param[out]  match_flag      等待到的事件结果
 * @param[in]   options         等待事件的选项，如等待一个事件即可 @ref COS_EVENT_WAIT_ANY，
 *                              等待全部事件 @ref COS_EVENT_WAIT_ALL, 等待到事件后清除该事件 @ref COS_EVENT_WAIT_CLR
 *
 * @note 不可以在中断上下文中使用。
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR_PARAM 传入的参数非法
 * @retval  COS_ERR 事件未知错误
 * @retval  COS_ERR_TIMEOUT 等待超时
 * @retval  COS_ERR_DESTROY 事件已被销毁
 */
cos_status_t cos_event_wait(cos_event_t event, cos_event_flag_t expect_flag, cos_tick_t timeout, cos_event_flag_t *match_flag, uint32_t options);

/**
 * @brief 释放事件
 *
 * @param[in]   event           事件句柄
 * @param[in]   flag            要释放的事件标志
 * @param[in]   options         释放事件的选项，如释放后是否保留该事件 @ref COS_EVENT_RELEASE_KEEP
 *
 * @return  COS_OK: 成功， <0: 失败
 * @retval  COS_ERR 事件未知错误
 * @retval  COS_ERR_DESTROY 事件已被销毁
 */
cos_status_t cos_event_release(cos_event_t event, cos_event_flag_t flag, uint32_t options);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*_COSIT_H*/
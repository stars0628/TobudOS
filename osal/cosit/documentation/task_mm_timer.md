## 5.0 通用
物联网操作系统各模块都会用到的通用的常量、类型、函数定义。

### 5.0.1 常量
#### 5.0.1.1 COS_WAIT_FOREVER
```C
#define COS_WAIT_FOREVER  ((cos_tick_t)-1)
```
表示永久阻塞等待，直到获得系统资源（如互斥量，信号量，事件，队列消息等）才返回。

#### 5.0.1.2 COS_NO_WAIT
```C
#define COS_NO_WAIT  (0x0U)
```
表示非阻塞等待，当不能获得系统资源（如互斥量，信号量，事件，队列消息等）时不会等待，而是立即返回。

### 5.0.2 类型
#### 5.0.2.1 cos_tick_t
```C
typedef uint64_t cos_tick_t
```
系统tick数

### 5.0.3 函数
#### 5.0.3.1 cos_ms_to_tick
把毫秒数转为系统tick数

**原型**
```C
cos_tick_t cos_ms_to_tick(uint64_t ms);
```

**描述**
把毫秒数转成系统tick数。在物联网操作系统中系统tick一般用于内核调度的基本单位，其频率在系统中是可以配置的。根据系统tick一般只能达到毫秒精度。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| ms     | 入     | 毫秒数|

**返回值**
转换得到的系统tick数

**注意**
可以在中断上下文中使用

**例子**

#### 5.0.3.2 cos_tick_to_ms
把系统tick数转为毫秒数

**原型**
```C
uint64_t cos_tick_to_ms(cos_tick_t tick);
```

**描述**
把系统tick数转成毫秒数。在物联网操作系统中系统tick一般用于内核调度的基本单位，其频率在系统中是可以配置的。根据系统tick一般只能达到毫秒精度。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| tick     | 入     | 系统tick数|

**返回值**
转换得到的毫秒数

**注意**
可以在中断上下中使用

**例子**

#### 5.0.3.3 cos_tick_get
把系统tick数转为毫秒数

**原型**
```C
cos_tick_t cos_tick_get(void);
```

**描述**
获取当前系统tick数。

**参数**

**返回值**
获取到的当前系统tick数。

**注意**
可以在中断上下中使用

**例子**


## 5.1 任务管理
任务可以认为是一段独享CPU的运行程序，而应用是完成特定功能的多个任务的集合。任务管理就是为多任务环境中的每个任务分配一个上下文（context）（上下文（context）是指当任务被调度执行的所必不可少的一组数据，包括前任务的CPU指令地址（PC指针），当前任务的栈空间，当前任务的CPU寄存器状态等），在任务相继执行过程中，将切出任务的信息保存在任务上下文中，将切入任务的上下文信息恢复，使其得以执行。为维护任务上下文、状态、栈等相关信息，操作系统内核为每个任务定义了一组数据结构，即任务控制块（Task Control Block），来存放这些信息。 任务调度负责将处理器资源分配给关键任务，让其优先运行。所以系统中的每个任务需要根据关键程度分配不同的优先级，那些执行关键操作的任务被赋予高优先级，而一些非关键性任务赋予低优先级。当系统发生调度时，高优先级任务可以抢占低优先级任务的处理器资源得到调度执行。系统在无任务可调度时，就运行空闲任务，其优先级最低。 任务被创建时，需要为任务指定执行体入口地址、栈大小、优先级等信息，创建过程中内核为任务分配任务控制块（TCB）。任务栈空间可以在任务创建时由用户程序指定，也可以由内核根据用户指定大小来动态分配。操作系统内核提供基于软件的栈溢出检测方法，用户可根据需要配置或关闭该功能。

### 5.1.1 常量
#### 5.1.1.1 COS_TASK_OPTION_NORUN
```C
#define COS_TASK_OPTION_NORUN  (0x1U << 0)
```
用来指定创建一个新任务时任务属性的options参数，表示任务创建后处于挂起状态，不会被调度执行，直到被恢复。
如果没有指定此选项（默认），任务创建后即处于就绪状态，等待调度运行。

#### 5.1.1.2 COS_TASK_PRIORITY_MAX
```C
#define COS_TASK_PRIORITY_MAX  100
```
最大任务优先级。任务优先级从0 ~ COS_TASK_PRIORITY_MAX优先级别递减，即COS_TASK_PRIORITY_MAX优先级最低。如内核在实现时，若其优先级级别小于`COS_TASK_PRIORITY_MAX`, 当应用传入的优先级超过内核实现的优先级时，内核应调整为应用可设置的最低优先级。

如果应用不关心任务的优先级，则可以使用`cos_task_attr_init`初始化为默认优先级即可，不同内核在实现时，其默认优先级可以不同。

#### 5.1.1.4 COS_TASK_SCHED_CFS
```C
#define COS_TASK_SCHED_CFS 0x0
```
完全公平调度策略（非实时）。采用完全公平调度算法，对任务进行调度。适用于任务比较多的场景，开发者不需要设计每个任务的合理优先级，由调度算法以完全公平的策略调度任务，但开发者可以微调任务的权重，以相对增加或减少任务的运行时间片。

**注意：不是所有内核在实现时都支持CFS调度策略**

#### 5.1.1.5 COS_TASK_SCHED_FIFO
```C
#define COS_TASK_SCHED_FIFO 0x01
```
实时，FIFO调度策略。当同优先级的多个任务同时就绪时，按先进先出的顺序依次执行。

#### 5.1.1.6 COS_TASK_SCHED_RR
```C
#define COS_TASK_SCHED_RR 0x02
```
实时，Round-Robin调度策略. 当同优先级的多个任务同时就绪时，多个任务按时间片轮流执行。

### 5.1.2 类型
#### 5.1.2.1 cos_task_t
```C
typedef void * cos_task_t
```
任务的句柄，代表一个任务，调用任务创建函数后返回。可以通过句柄对任务进行操作，如挂起，删除等。

#### 5.1.2.2 cos_task_entry_t
```C
typedef void (*cos_task_entry_t) (void *);
```
任务入口函数的原型。

#### 5.1.2.3 cos_task_attr_t
```C
typedef struct cos_task_attr {
       const char *name;
       size_t stack_size;
       int32_t priority;
       cos_tick_t tick;
       uint8_t sched_policy;
       uint8_t cpu_bind;
       uint8_t inited;
       uint32_t options;
       void *reserved;
} cos_task_attr_t;
```
任务的属性类型，在创建任务时传递给内核，内核根据这些属性创建任务。

| 元素 | 描述 |
| -------- | -------- |
| name     | 任务名称(可以为NULL) |
| stack_size | 任务栈的大小（单位：Byte） |
| priority | 任务的优先级  |
| tick | 任务使用RR调度策略时，占用的系统tick数  |
| sched_policy | 调度策略，COS_TASK_SCHED_RR/COS_TASK_SCHED_FIFO/COS_TASK_SCHED_CFS  |
| cpu_bind | 绑定的CPU号，适用于多核系统  |
| inited | 标识该结构是否被初始化  |
| options | 任务创建时的选项  |
| reserved | 预留字段（可扩展） |

### 5.1.3 函数
#### 5.1.3.1 cos_task_attr_init
初始化一个任务属性

**原型**
```C
void cos_task_attr_init(cos_task_attr_t *attr);
```

**描述**
初始化一个任务属性，为任务属性设置默认值。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| attr     | 入     | 任务属性的句柄|

**返回值**
无

**注意**
此函数与创建任务同时使用。
不同内核实现的默认值可能不同。

**例子**

#### 5.1.3.2 cos_task_create
创建一个新的任务

**函数原型**
```C
cos_status_t cos_task_create(cos_task_t *task, cos_task_entry_t fn, void *arg, cos_task_attr_t *attr);
```

**描述**
创建一个新任务并初始化，新任务的句柄由参数task返回，通过任务句柄task对新任务进行后续的操作。
新任务成功创建后，新任务会进入就绪队列等待调度执行。如果选项 COS_TASK_OPTION_NORUN被指定，则任务创建后处于挂起状态，不会被调度执行，需要应用恢复该任务后，才能进入就绪队列等待调度执行。
当被调度到时从入口函数`fn`开始运行。从任务入口函数返回，代表此任务退出，系统会清理该任务的资源。
任务的属性`attr`应使用`cos_task_attr_init`初始化。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 出     | 新任务的句柄，后续对此任务的操作通过该句柄进行。|
| fn     | 入     | 新任务的入口函数，新任务从此函数开始运行。|
| arg     | 入     | 新任务的入口函数的参数。|
| attr     | 入     | 新任务的属性|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |
| COS_ERR_TASK_NOMEM     | 内存不足     |
| COS_ERR_ISR     | 在中断上下文中调用     |

**注意**
此函数不能在中断上下文中使用。

**例子**
```C
#include <stdio.h>
#include <stdint.h>
#include <cosit.h>
 
static void cos_task_test_entry(void *arg)
{
    printf("%s:%d:new task is running.\n", __func__, __LINE__);
    return;
}
 
int cos_task_create_example()
{
    cos_status_t ret;
    cos_task_t task;
    cos_task_attr_t attr;

    cos_task_attr_init(&attr); 
    ret = cos_task_create(&task, cos_task_test_entry, NULL, &attr)
    if (ret != COS_OK) {
        printf("%s:%d: create task fail ret:%d\n", __func__, __LINE__, ret);
        return -1;
    }
 
    return 0;
}
```

#### 5.1.3.3 cos_task_delete
删除一个任务

**原型**
```C
cos_status_t cos_task_delete(cos_task_t task);
```

**描述**
主动删除一个任务回收其资源。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |
| COS_ERR_ISR     | 在中断上下文中调用     |
| COS_ERR_PERM     | 没有权限删除任务，如删除idle任务 |

**注意**
此函数不能在中断上下文中使用。
不能删除系统idle任务。

**例子**

#### 5.1.3.4 cos_task_init
初始化一个任务

**函数原型**
```C
cos_status_t cos_task_init(cos_task_t task, cos_task_entry_t fn, void *arg, cos_task_attr_t *attr, void *stack_buf);
```

**描述**
初始化一个任务。与创建任务类似，区别是调用者为新任务的控制块分配内存，其大小为任务控制块的数据结构大小，不同内核其任务控制块大小不同。
通过参数`task`指定任务的控制块地址。 调用者还需要为新任务分配栈内存空间，并通过参数`stack_buf`指定栈内存空间地址，栈空间大小通过`attr`中的stack_size传递。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 新任务控制块句柄，函数对该任务进行初始化|
| fn     | 入     | 新任务的入口函数，新任务从此函数开始运行。|
| arg     | 入     | 新任务的入口函数的参数。|
| attr     | 入     | 新任务的属性|
| stack_buf     | 入     | 调用者指定的任务栈空间|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |
| COS_ERR_ISR     | 在中断上下文中调用     |

**注意**
此函数不能在中断上下文中使用。

#### 5.1.3.5 cos_task_deinit
反初始化一个任务

**原型**
```C
cos_status_t cos_task_deinit(cos_task_t task);
```

**描述**
反初始化一个任务。与`cos_task_init`配套使用，销毁其初始化过的任务，把此任务置为销毁状态。如果该任务销毁前处于就绪状态，会把该任务从系统就绪队列中移除。
根据互斥锁的类型（鲁棒性互斥锁），还会释放其拥有的互斥锁，导致唤醒等待该锁的任务。 但不会释放任务控制块task和任务栈的内存空间，这部分内存应由调用者负责释放。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |
| COS_ERR_ISR     | 在中断上下文中调用     |
| COS_ERR_PERM     | 没有权限反初始化任务，如idle任务 |

**注意**
此函数不能在中断上下文中使用。
不能反初始化系统idle任务。

#### 5.1.3.6 cos_task_exit
退出当前任务

**原型**
```C
void cos_task_exit(int32_t status);
```

**描述**
主动退出当前任务，使系统回收该任务的资源。其作用与从该任务的入口函数返回相同。参数`status`作为后续扩展使用，目前并不支持获取一个任务退出时的状态码。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| status     | 入     | 任务退出的状态码（未使用）|

**返回值**
此函数导致当前任务退出，不会返回。

**注意**
此函数不能在中断上下文中使用。
目前并不支持获取一个任务退出时的状态码。

**例子**

#### 5.1.3.7 cos_task_suspend
挂起一个任务

**原型**
```C
cos_status_t cos_task_suspend(cos_task_t task);
```

**描述**
挂起一个任务，暂停该任务的执行。可以挂起自身或其他任务，但不允许挂起系统idle任务。当挂起系统其他内置任务时请谨慎，可能会引起系统错误。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |
| COS_ERR_PERM     | 没有权限挂起任务，如挂起idle任务 |

**注意**
可以在中断上下文中使用

**例子**

#### 5.1.3.8 cos_task_resume
恢复一个任务

**原型**
```C
cos_status_t cos_task_resume(cos_task_t task);
```

**描述**
恢复一个任务，使其处于就绪状态，等待调度执行。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.9 cos_task_yield
当前任务让出CPU

**原型**
```C
cos_status_t cos_task_yield(void);
```

**描述**
使当前正在运行的任务让出CPU，并把当前任务置于其静态优先级的就绪队列的末尾。重新调度，其他任务得到CPU运行。注意根据调度策略，如果该任务是其静态优先级的就绪队列中唯一任务，它会被再次调度到，继续运行。

**参数**
无

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.10 cos_task_self
获取当前任务自身的句柄

**原型**
```C
cos_task_t cos_task_self(void);
```

**描述**
返回当前任务自身的句柄，用于对自身任务的操作，如挂起和恢复等。

**参数**
无

**返回值**
函数总是执行成功并返回当前任务自身的句柄。

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.11 cos_task_get_name
获取一个任务的名称

**原型**
```C
cos_status_t cos_task_get_name(cos_task_t task, char *buf, size_t buf_size);
```
**描述**
返回一个任务的名称，注意调用者应该为存放任务名称分配足够的内存，并把内存首地址和大小通过参数buf和buf_size传入。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|
| buf     | 出     | 存放任务名称的内存|
| buf_size     | 入     | 存放任务名称的内存大小|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.12 cos_task_find
根据名称查找任务

**原型**
```C
cos_task_t cos_task_find(const char *name);
```

**描述**
根据任务名称`name`查找任务，若能找到则返回任务的句柄，不能找到则返回`NULL`。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| name     | 入     | 任务的名称|

**返回值**
函数执行成功返回所找到的任务的句柄，失败则返回`NULL`。

**注意**
可以在中断上下文中使用。
当系统中存在多个具有相同名称的任务时，函数会执行成功，但返回哪一个任务的句柄是不确定的，不同的内核实现会有所不同。

**例子**

#### 5.1.3.13 cos_task_sleep
使当前任务睡眠一段时间

**原型**
```C
cos_status_t cos_task_sleep(cos_tick_t tick);
```

**描述**
使当前任务睡眠`tick`个系统tick的时间，等时间超期或被唤醒后再次进入就绪状态，等待调度执行。
如果`tick`为0，则等同于`cos_task_yield`。如果`tick`为`COS_WAIT_FOREVER`, 则等同于`cos_task_suspend`。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| tick     | 入     | 系统tick个数|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。被提前唤醒时则返回剩余的tick数。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.1.3.14 cos_task_wakeup
唤醒一个任务

**原型**
```C
cos_status_t cos_task_wakeup(cos_task_t task);
```

**描述**
唤醒一个任务。可以用来提前唤醒一个睡眠的任务。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.15 cos_task_get_priority
获取一个任务的优先级

**原型**
```C
cos_status_t cos_task_get_priority(cos_task_t task, uint8_t *priority);
```

**描述**
获取一个任务的优先级，可以是自身或其他任务。获取的是任务此时的动态优先级，不一定与其创建时指定的静态优先级相同，如任务此时有优先级继承发生。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|
| priority     | 出     | 获取到的任务动态优先级|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.16 cos_task_set_priority
设置一个任务的优先级

**原型**
```C
cos_status_t cos_task_set_priority(cos_task_t task, uint8_t priority);
```

**描述**
设置一个任务的优先级，可以是自身或其他任务。改变的是任务的静态优先级。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|
| priority     | 入     | 任务静态优先级|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.17 cos_task_get_time_slice
获取一个RR调度策略的任务的剩余时间片

**原型**
```C
cos_status_t cos_task_get_time_slice(cos_task_t task, cos_tick_t *time_slice);
```

**描述**
获取一个使用RR调度策略的任务在本次轮转中剩余时间片。此函数仅适用于采用RR调度策略的任务， 对于其他任务，剩余时间片返回0.

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|
| time_slice     | 出     | 获取到的任务的剩余时间片|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

#### 5.1.3.18 cos_task_set_time_slice
设置一个RR调度任务的时间片

**原型**
```C
cos_status_t cos_task_set_time_slice(cos_task_t task, cos_tick_t time_slice);
```

**描述**
设置一个使用RR调度策略的任务在一次轮转中所分配的时间片。此函数仅适用于采用RR调度策略的任务， 对于其他任务，返回错误。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| task     | 入     | 任务的句柄|
| time_slice     | 入     | RR调度任务的时间片|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
可以在中断上下文中使用。

**例子**

## 5.2 内存管理
内存管理实现从系统堆内存中动态分配和释放内存，向应用提供动态分配和释放内存的API。

### 5.2.1 cos_malloc
申请一块连续的内存

**原型**
```C
void *cos_malloc(size_t size)
```

**描述**
从系统堆中申请一块`size`大小连续的内存，成功返回申请到内存的首地址，失败返回NULL。

**参数**
| 参数名称 | 参数方向 | 描述 |
| -------- | -------- | -------- |
| size     | 入    | 想申请内存的大小（单位：Byte） |

**返回值**
函数执行成功返回申请到内存的首地址，失败则返回NULL。

**注意**
此函数不能在中断上下文中使用。

**例子**

### 5.2.2 cos_realloc
改变一块内存的大小

**原型**
```C
void *cos_realloc(void *ptr, size_t new_size)
```

**描述**
改变一块内存`ptr`的大小，`ptr`应该是之前动态申请内存的返回值。新的内存大小可能扩大或缩小，扩大时原有内存大小的内容保持不变且新增加的内存没有初始化。缩小时，被保留的内存的内容保持不变。如果参数`ptr`为NULL， 不论new_size为多少，则等于cos_malloc(new_size)，如果参数`new_size`为0，则等于cos_free(prt)。如果该函数返回的新的内存首地址与原有地址不同，则原有内存被释放。

**参数**
| 参数名称 | 参数方向 | 描述 |
| -------- | -------- | -------- |
| ptr     | 入    | 之前动态申请到的内存首地址 |
| new_size     | 入    | 内存新大小 |

**返回值**
函数执行成功返回新申请到内存的首地址，失败则返回NULL。

**注意**
此函数不能在中断上下文中使用。

**例子**

### 5.2.3 cos_calloc
申请多块连续的内存

**原型**
```C
void *cos_calloc(size_t count, size_t size)
```

**描述**
从系统堆中申请`count`个长度为`size`的连续的内存，最终申请到内存大小为`count * size`个字节。

**参数**
| 参数名称 | 参数方向 | 描述 |
| -------- | -------- | -------- |
| count     | 入    | 申请内存块的个数 |
| size     | 入    | 每个内存块的大小 |

**返回值**
函数执行成功返回申请到内存的首地址，失败则返回NULL。

**注意**
此函数不能在中断上下文中使用。

**例子**

### 5.2.4 cos_free
释放内存

**原型**
```C
void cos_free(void *ptr)
```

**描述**
释放由参数`ptr`指向的一块内存，ptr应该是之前动态申请函数的返回值。

**参数**
| 参数名称 | 参数方向 | 描述 |
| -------- | -------- | -------- |
| ptr     | 入    | 动态申请到的内存首地址 |

**返回值**
无

**注意**
此函数不能在中断上下文中使用。

**例子**

### 5.2.5 cos_malloc_align
申请内存并按要求地址对齐

**原型**
```C
int cos_malloc_align(void **memptr, size_t size, size_t alignment)
```

**描述**
从系统堆内存中申请`size`字节的连续内存，并按`alignment`字节对齐，申请到的内存首地址由\*memptr返回。如果参数size为0，则函数立即返回错误码，\*memptr为NULL。 参数alignment应该是`sizeof(void*)`的倍数且是2的幂，否则立即返回错误，\*memptr为NULL。 释放内存通过cos_free函数释放。

**参数**
| 参数名称 | 参数方向 | 描述 |
| -------- | -------- | -------- |
| memptr     | 出     | 返回申请到的内存首地址 |
| size     | 入    | 想申请内存的大小（单位：Byte） |
| alignment     | 入     | 内存地址对齐大小（单位：Byte） |

**返回值**
函数执行成功返回COS_OK, 所申请到内存的首地址由参数\*memptr返回。失败则返回错误码，同时参数\*memptr为NULL。

| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR_PARAM     | 非法参数 |
| COS_ERR_NOMEM     | 内存不足 |

**注意**
此函数不能在中断上下文中使用。

**例子**

## 5.3 软件定时器
软件定时器是由内核提供的隔一段时间单次或周期性触发执行应用函数的功能。应用可以通过设置回调函数，和定时器的周期等参数，让内核周期性地执行应用回调函数。 软件定时器一般由内核的定时器任务，基于系统tick，检查定时器是否超期，超期则在定时器任务的上下文中执行应用回调函数，这是其与硬件定时器不同之处。

### 5.3.1 常量
#### 5.3.1.1 COS_TIMER_OPTION_ACTIVATE
创建时激活选项

```C
#define COS_TIMER_OPTION_ACTIVATE  (0x1u << 0)
```
定时器被创建后即开始启动。

#### 5.3.1.2 COS_TIMER_OPTION_DEACTIVATE
创建时不激活选项（**默认**）

```C
#define COS_TIMER_OPTION_DEACTIVATE  (0x1u << 1)
```
定时器被创建后不会立即启动，需要被再次主动调用启动函数来启动。

### 5.3.2 类型
#### 5.3.2.1 cos_timer_t
软件定时器句柄

```C
typedef void * cos_timer_t
```
#### 5.3.2.2 cos_timer_cb_t
软件定时器的回调函数类型

```C
typedef void (*cos_timer_cb_t)(void *arg)
```

### 5.3.3 函数
#### 5.3.3.1 cos_timer_create
创建一个定时器

**原型**
```C
cos_status_t cos_timer_create(cos_timer_t *timer, const char *name, cos_timer_cb_t cb, void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options);
```

**描述**
创建一个定时器，当定时器超期时内核会执行回调函数`cb`， 并传入参数`arg`。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 出     | 定时器句柄|
| name     | 入     | 定时器名称(可以为NULL)|
| cb     | 入     | 定时器的回调函数|
| arg     | 入     | 定时器的回调函数的参数|
| initial     | 入     | 定时器的初始定时时间，即从启动到第一次触发的时间间隔，单位：tick个数|
| period     | 入     | 定时器的周期定时时间，即周期性触发的时间间隔，单位：tick个数。0：代表单次触发|
| options     | 入     | 定时器的选项，如创建时是否激活 `COS_TIMER_OPTION_ACTIVATE/COS_TIMER_OPTION_DEACTIVATE`|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |
| COS_ERR_NOMEM     | 内存不足     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.2 cos_timer_delete
删除一个定时器

**原型**
```C
cos_status_t cos_timer_delete(cos_timer_t timer);
```

**描述**
删除一个定时器。系统会先停止该定时器，然后删除回收其资源。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.3 cos_timer_init
初始化一个定时器

**原型**
```C
cos_status_t cos_timer_init(cos_timer_t timer, const char *name, cos_timer_cb_t cb, void *arg, cos_tick_t initial, cos_tick_t period, uint32_t options);
```

**描述**
初始化一个定时器，当定时器超期时内核会执行回调函数`cb`， 并传入参数`arg`。与创建一个定时器类似，区别是定时器的控制块内存时由调用者传入的，一般用于应用希望静态分配内存的场景。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|
| name     | 入     | 定时器名称|
| cb     | 入     | 定时器的回调函数|
| arg     | 入     | 定时器的回调函数的参数|
| initial     | 入     | 定时器的初始定时时间，即从启动到第一次触发的时间间隔，单位：tick个数|
| period     | 入     | 定时器的周期定时时间，即周期性触发的时间间隔，单位：tick个数。0：代表单次触发|
| options     | 入     | 定时器的选项，如创建时是否激活 `COS_TIMER_OPTION_ACTIVATE/COS_TIMER_OPTION_DEACTIVATE`|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.4 cos_timer_deinit
反初始化一个定时器

**原型**
```C
cos_status_t cos_timer_deinit(cos_timer_t timer);
```

**描述**
反初始化一个定时器。系统会先停止该定时器，从系统定时器列表中移除。但不会释放`timer`的内存空间，应由调用者释放。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.5 cos_timer_start
启动一个定时器

**原型**
```C
cos_status_t cos_timer_start(cos_timer_t timer);
```

**描述**
启动一个定时器，内核开始计时，直到定时器超期时执行应用设置的回调函数。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.6 cos_timer_stop
停止一个定时器

**原型**
```C
cos_status_t cos_timer_stop(cos_timer_t timer);
```

**描述**
停止一个定时器

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.7 cos_timer_change
修改一个定时器

**原型**
```C
cos_status_t cos_timer_change(cos_timer_t timer, cos_tick_t initial, cos_tick_t period);
```

**描述**
修改一个定时器。一般用于修改其周期性定时时间，如果该定时器创建后还还有启动，也可以修改其初始定时时间。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|
| initial     | 入     | 定时器的初始定时时间，即从启动到第一次触发的时间间隔，单位：tick个数|
| period     | 入     | 定时器的周期定时时间，即周期性触发的时间间隔，单位：tick个数。0：代表单次触发|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

#### 5.3.3.8 cos_timer_get_time
获取一个定时器的时间

**原型**
```C
cos_status_t cos_timer_get_time(cos_timer_t timer, cos_tick_t *remaining, cos_tick_t *period);
```

**描述**
返回一个定时器的信息，包括离下次触发还剩余的时间（系统tick），和周期触发的时间间隔（系统tick）。

**参数**
| 参数名称  | 参数方向 |描述 |
| -------- | -------- | -------- |
| timer     | 入     | 定时器句柄|
| remaining     | 出     | 离下次触发还剩余的时间，单位：tick个数|
| period     | 出     | 周期触发的时间间隔，单位：tick个数。0：代表单次触发|

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 错误码 | 描述 |
| -------- | -------- |
| COS_ERR     | 未知错误     |
| COS_ERR_PARAM     | 非法参数     |

**注意**
不可以在中断上下文中使用。

**例子**

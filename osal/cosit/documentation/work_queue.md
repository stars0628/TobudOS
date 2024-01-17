# 工作队列

工作队列（workqueue）是一种转移任务执行环境的工具，例如当系统产生一个中断时，我们可以在中断处理函数里做一些紧急地操作，然后将另外一些不那么紧急，而且需要一定时间的任务封装成函数交给工作队列执行，此时该函数的执行环境就从 中断环境 变成了 线程环境，即中断处理 “下半部”。

## 类型

```C
typedef struct cos_workqueue* cos_wq_t;
```

工作队列控制块，工作队列控制块是操作系统用于管理工作队列的一个数据结构，由结构体cos_workqueue表示，cos_wq_t是工作队列的句柄，工作队列结构体主要保存了当前系统工作队列的相关信息，如:当前工作项，工作队列的链表，工作队列所使用的线程等。

```C
typedef struct cos_work* cos_work_t;
```

工作项控制块，工作项控制块用于管理该任务，保存了该任务相关的信息，如：任务函数，用户数据等，当该工作项被执行时就会调用该任务函数。

```C
typedef void cos_work_handle_t(void *arg);
```

工作项控制块，工作项控制块用于管理该任务，保存了该任务相关的信息，如：任务函数，用户数据等，当该工作项被执行时就会调用该任务函数。

## 函数

### cos_work_init

初始化工作项

原型

```C

#include <cos_wq.h>
cos_status_t cos_work_init(cos_work_t work, cos_work_handle_t work_func, void *work_data);

```

**描述**
该接口初始化 work 指针指向的工作项，并绑定回调函数 work_func 以及用户自定义数据 work_data。

**参数**
| 参数名称      | 参数类型                    | 参数方向            | 描述                         |
| -----------  | -----------                | ------------------ | ------------------           |
| work         | cos_work_t               | 入参                | 工作项                       |
| work_func    | cos_work_handle_t        | 入参                | 工作项的回调函数              |
| work_data    | size_t                     | 入参                | 用户自定义数据，回调函数参数   |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| cos_ERROR_PARAM        | 非法参数            |

### cos_work_dowork

触发work

原型

```C

#include <cos_wq.h>
cos_status_t cos_work_dowork(cos_work_t work, cos_tick_t time);

```

**描述**
work触发函数（由系统默认工作队列来执行）。若 time 大于 0，则提交过程会延时 time 个 ticks 。

**参数**
| 参数名称      | 参数类型                    | 参数方向            | 描述                         |
| -----------  | -----------                | ------------------ | ------------------           |
| work         | cos_work_t               | 入参                | 工作项                       |
| time         | cos_tick_t               | 入参                | 提交延时，以 tick 为单位       |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| cos_ERROR_PARAM        | 非法参数            |

### cos_work_submit

向 queue 提交work

原型

```C

#include <cos_wq.h>
cos_status_t cos_work_submit(cos_wq_t wq, cos_work_t work, cos_tick_t time);

```

**描述**
将 work 指向的工作项提交到 queue 指向的工作队列中，若 time 大于 0，则该提交延时 time 个 tick 之后执行。

**参数**
| 参数名称      | 参数类型                    | 参数方向            | 描述                         |
| -----------  | -----------                | ------------------ | ------------------           |
| wq           | cos_wq_t                 | 入参                | 工作队列                     |
| work         | cos_work_t               | 入参                | 工作项                       |
| time         | cos_tick_t               | 入参                | 提交延时，以 tick 为单位       |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| cos_ERROR_PARAM        | 非法参数            |

### cos_work_cancel

取消指定工作项

原型

```C

#include <cos_wq.h>
cos_status_t cos_work_cancel(cos_wq_t queue, cos_work_t work);

```

**描述**
从 queue 指向的工作队列中将 work 指向的工作项移除。

**参数**
| 参数名称      | 参数类型                    | 参数方向            | 描述                         |
| -----------  | -----------                | ------------------ | ------------------           |
| queue        | cos_wq_t                 | 入参                | 工作队列                      |
| work         | cos_work_t               | 入参                | 工作项                       |


**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| cos_ERROR_PARAM        | 非法参数            |
| cos_ERROR_BUSY         | 该工作项正在执行     |

### cos_workqueue_create

工作队列创建函数

原型

```C

#include <cos_wq.h>
cos_status_t cos_workqueue_create(cos_wq_t* wq, const char *name， uint16_t stack_size, uint8_t priority);

```

**描述**
该函数创建并初始化一个工作队列，利用参数 name，stack_size 和 priority 创建工作队列内部线程，最终返回创建的工作队列。下表描述了该函数的输入参数与返回值

**参数**
| 参数名称      | 参数类型                    | 参数方向            | 描述                         |
| -----------  | -----------                | ------------------ | ------------------           |
| wq           | cos_wq_t                 | 出参                | 工作队列控制块                |
| name         | const char *               | 入参                | 线程名字                      |
| stack_size   | uint16_t                   | 入参                | 线程栈大小                    |
| priority     | uint8_t                    | 入参                | 线程优先级                    |


**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| cos_ERROR_PARAM        | 非法参数            |

### cos_workqueue_destroy

工作队列销毁函数

原型

```C

#include <cos_wq.h>
cos_status_t cos_workqueue_destroy(cos_wq_t wq);

```

**描述**
销毁指定工作队列

**参数**
| 参数名称      | 参数类型                    | 参数方向            | 描述                         |
| -----------  | -----------                | ------------------ | ------------------           |
| wq           | cos_wq_t                 | 入参                | 工作队列控制块                |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| cos_ERROR_PARAM        | 非法参数            |


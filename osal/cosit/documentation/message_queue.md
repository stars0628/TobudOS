# 消息队列

消息队列（Message Queue）是一种常见的异步任务间通讯方式，用于在任务与任务间，任务与中断间传递不定长的数据。任务能够从队列里面读取消息，当队列中的消息为空时，挂起读取任务；当队列中有新消息时，挂起的读取任务被唤醒并处理新消息。任务也能够往队列里写入消息，当队列已经写满消息时，挂起写入任务；当队列中有空闲消息节点时，挂起的写入任务被唤醒并写入消息。

## 类型

```C
typedef struct cos_messagequeue* cos_mq_t;
```

消息队列控制块，消息队列控制块是操作系统用于管理消息队列的一个数据机构，由结构体cos_messagequeue表示，cos_mq_t是消息队列的句柄。

## 函数

### cos_mq_create

创建消息队列

原型

```C

#include <cos_mq.h>
cos_status_t cos_mq_create(const char* name, size_t msg_size,
                    size_t max_msgs, uint8_t flag，cos_mq_t* mq);

```

**描述**
创建一个消息队列，由系统动态为消息队列申请内存空间，并返回消息队列的句柄。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| name        | const char* | 入参                | 消息队列的名称      |
| msg_size    | size_t      | 入参                | 单条消息的最大长度   |
| max_msgs    | size_t      | 入参                | 消息队列的最大个数   |
| flag        | uint8_t     | 入参                | 消息队列工作方式   COS_IPC_FLAG_FIFO 或 COS_IPC_FLAG_PRIO |
| mq          | cos_mq_t  | 出参                | 消息队列句柄        |

**返回值**
函数执行成功返回COS_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| COS_ERROR_PARAM        | 非法参数              |
| COS_ERR_NOMEM          | 内存不足              |
| COS_ERR_ISR            | 在中断上下文中调用     |
| COS_ERR                | 未知错误              |

### cos_mq_init

创建消息队列

init

```C

#include <cos_mq.h>
cos_status_t cos_mq_init(const char* name, void *msgpool, size_t msg_size,
                    size_t max_msgs, cos_mq_t mq);

```

**描述**
创建一个静态消息队列，由编译器提前为消息队列在编译时分配内存空间。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述                        |
| ----------- | ----------- | ------------------ | --------------------------- |
| name        | const char* | 入参                | 消息队列的名称               |
| msgpool     | void*       | 入参                | 指向存放消息的缓冲区的指针     |
| msg_size    | size_t      | 入参                | 单条消息的最大长度，单位字节   |
| flag        | uint8_t     | 入参                | 消息队列工作方式   cos_IPC_FLAG_FIFO 或 cos_IPC_FLAG_PRIO |
| mq          | cos_mq_t  | 出参                | 消息队列句柄                 |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| COS_ERROR_PARAM        | 非法参数            |

### cos_mq_delete

删除消息队列

原型

```C

#include <cos_mq.h>
cos_status_t cos_mq_delete(cos_mq_t mq);

```

**描述**
删除一个消息队列。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mq          | cos_mq_t  | 入参                | 消息队列的句柄      |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称            | 描述               |
| -----------        | ------------------ |
| COS_ERROR_PARAM  | 非法参数            |

### cos_mq_deinit

去初始化消息队列

原型

```C

#include <cos_mq.h>
cos_status_t cos_mq_deinit(cos_mq_t mq);

```

**描述**
去初始化一个消息队列。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mq          | cos_mq_t  | 入参                | 消息队列的句柄      |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称            | 描述               |
| -----------        | ------------------ |
| COS_ERROR_PARAM  | 非法参数            |
### cos_mq_send

发送消息

原型

```C

#include <cos_mq.h>
cos_status_t cos_mq_send(cos_mq_t mq，void* buffer, size_t size);

```

**描述**
发送消息，线程或者中断服务程序都可以给消息队列发送消息。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mq          | cos_mq_t  | 入参                | 消息队列的句柄      |
| buffer      | void*       | 入参                | 消息内容           |
| size        | size_t      | 入参                | 消息大小           |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                   | 描述                    |
| -----------               | ------------------      |
| COS_ERROR_PARAM         | 非法参数                 |
| COS_ERROR_MQ_DELETED    | 线程挂起时消息队列已被删除 |
| COS_ERROR_MQ_FULL       | 消息队列已满             |

### cos_mq_recive

接收消息

原型

```C

#include <cos_mq.h>
cos_status_t cos_mq_recv(cos_mq_t mq，void* buffer, 
        size_t size，int32_t timeout);

```

**描述**
当消息队列中有消息时，接收者才能接收消息，否则接收者会根据超时时间设置，或挂起在消息队列的等待线程队列上，或直接返回。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mq          | cos_mq_t  | 入参                | 消息队列的句柄      |
| buffer      | void*       | 入参                | 消息内容           |
| size        | size_t      | 入参                | 消息大小           |
| timeout     | int32_t     | 入参                | 指定的超时时间      |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                   | 描述                    |
| -----------               | ------------------      |
| COS_ERROR_PARAM         | 非法参数                 |
| COS_ERROR_MQ_TIMEOUT    | 超时                    |
| COS_ERROR_PARAM         | 消息队列已满             |
| COS_ERROR_MQ_DELETED    | 线程挂起时消息队列已被删除 |

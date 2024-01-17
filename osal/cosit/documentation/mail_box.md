# 邮箱

消息邮箱（Mail Box）是实时操作系统中一种典型的线程间通信方法，邮箱用于线程间通信，特点是开销比较低，效率较高。

## 类型

```C
typedef void * cos_mb_t;
```

邮箱控制块是操作系统用于管理邮箱的一个数据结构，cos_mb_t是邮箱的句柄。

## 函数

### cos_mb_create

创建邮箱

原型

```C

#include <cos_mb.h>
cos_status_t cos_mb_create(const char* name, size_t msg_size,
                    uint8_t flag，cos_mb_t* mb);

```

**描述**
创建一个邮箱，由系统动态为邮箱申请内存空间，并返回邮箱的句柄。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| name        | const char* | 入参                | 邮箱名称            |
| msg_size    | sizebt      | 入参                | 邮箱容量            |
| flag        | uint8_t     | 入参                | 邮箱标志，如： COS_IPC_FLAG_FIFO 或 COS_IPC_FLAG_PRIO |
| mb          | cos_mb_t  | 出参                | 邮箱句柄        |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| COS_ERROR_PARAM        | 非法参数            |
| COS_ERROR_MEM          | 内存不足            |

### cos_mb_delete

删除邮箱

原型

```C

#include <cos_mb.h>
cos_status_t cos_mb_delete(cos_mb_t mb);

```

**描述**
删除一个邮箱。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mb          | cos_mb_t  | 入参                | 邮箱的句柄      |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称            | 描述               |
| -----------        | ------------------ |
| COS_ERROR_PARAM  | 非法参数            |

### cos_mb_init

初始化邮箱

原型

```C

#include <cos_mb.h>
cos_status_t cos_mb_init(const char* name, void *msgpool, size_t msg_size,
                    size_t max_msgs, cos_mb_t mb);

```

**描述**
创建一个静态邮箱，由编译器提前为邮箱在编译时分配内存空间。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述                        |
| ----------- | ----------- | ------------------ | --------------------------- |
| name        | const char* | 入参                | 邮箱的名称               |
| msgpool     | void*       | 入参                | 缓冲区指针     |
| msg_size    | size_t      | 入参                | 邮箱容量   |
| flag        | uint8_t     | 入参                | 邮箱工作方式   cos_IPC_FLAG_FIFO 或 cos_IPC_FLAG_PRIO |
| mb          | cos_mb_t  | 出参                | 邮箱句柄                 |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                  | 描述               |
| -----------              | ------------------ |
| COS_ERROR_PARAM        | 非法参数            |

### cos_mb_deinit

删除邮箱

原型

```C

#include <cos_mb.h>
cos_status_t cos_mb_deinit(cos_mb_t mb);

```

**描述**
去初始化一个邮箱。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mb          | cos_mb_t  | 入参                | 邮箱的句柄      |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称            | 描述               |
| -----------        | ------------------ |
| COS_ERROR_PARAM  | 非法参数            |

### cos_mb_send

发送邮件

原型

```C

#include <cos_mb.h>
cos_status_t cos_mb_send(cos_mb_t mb，uint32_t value);

```

**描述**
发送邮件，当邮箱中的邮件已经满时，发送邮件的线程或者中断程序会收到 cos_ERROR_mb_FULL 的返回值。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mb          | cos_mb_t  | 入参                | 邮箱的句柄      |
| value      | uint32_t       | 入参                | 邮件内容           |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                   | 描述                    |
| -----------               | ------------------      |
| COS_ERROR_PARAM         | 非法参数                 |
| COS_ERROR_mb_FULL   | 邮箱中的邮已满 |

### cos_mb_recive

接收邮件

原型

```C

#include <cos_mb.h>
cos_status_t cos_mb_recv(cos_mb_t mb，uint32_t* value,
        int32_t timeout);

```

**描述**
当邮箱中有邮件时，接收者才能接收邮件，否则接收者会根据超时时间设置，或挂起在邮箱的等待线程队列上，或直接返回。

**参数**
| 参数名称     | 参数类型     | 参数方向            | 描述               |
| ----------- | ----------- | ------------------ | ------------------ |
| mb          | cos_mb_t  | 入参                | 邮箱的句柄      |
| value      | void*       | 入参                | 邮件内容           |
| timeout     | int32_t     | 入参                | 指定的超时时间      |

**返回值**
函数执行成功返回cos_OK，失败则返回错误码（均为负值）。
| 参数名称                   | 描述                    |
| -----------               | ------------------      |
| COS_ERROR_PARAM         | 非法参数                 |
| COS_ERROR_mb_TIMEOUT    | 超时                    |

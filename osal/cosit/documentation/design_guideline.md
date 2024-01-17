
定义的API/宏/枚举常量等统一采用前缀**cos**，以保证命名空间的独立性，不会与其他SDK/BSP内的名称重复。注意API采用小写cos_xxx， 宏/枚举采用大写COS_XXX.

# 设计目标
标准的设计目标要达到，开发者只需要查看一个模块的标准文档，就能了解模块的功能是什么，如何使用，使用时的注意事项。开发者不需要查看代码具体实现，甚至不需要查看其他资料，就可以进行应用的开发，并最终可以无缝迁移到支持该标准的平台。

# 命名规则
### 命名风格
统一采用linux命名风格，即API字母全小写，字段之间加下划线分割。如`cos_task_create/cos_mutex_create/cos_queue_send` 。
宏和枚举变量采用全大写和下划线分割的形式，如`COS_WAIT_FOREVER` 。

### API命名
遵循名词在前，动词在后。
`cos_<component>_[feature]_<action>`

cos：标准的统一前缀
component: 必选的，组件名称。如task/mutex/queue
feature: 可选的，组件内部子特性或子名称，如task组件内的name/priority
action: 必选的， API的动作，如get/set/send/recv/

命名举例
- cos_task_create
- cos_task_name_get
- cos_mutex_lock

### API术语
一些API命名时术语，可以参考行业各家OS的术语，及其他标准CMSIS/POSIX的术语。可以使用行业内标准缩写。
**create/delete**
每次调用都会创建一个新的实例的情形使用create，且句柄作为出参。删除实例使用delete。
```C
cos_status_t cos_task_create(cos_task_t *task, cos_task_entry_t fn, void *arg, cos_task_attr_t *attr)

cos_status_t cos_task_delete(cos_task_t task)
```

**init/deinit**
只能进行一次的初始化，如接口初始化，硬件初始化等，使用init，句柄作为入参，且句柄可省略。  反初始化使用deinit

```C
cos_status_t cos_init(void)
void cos_deinit(void)
```

# 数据类型
关于API使用的基本数据类型统一采用标准数据类型。
对于基本正数采用stdint.h中定义的数据类型
- int8_t
- uint8_t
- int16_t
- uint16_t
- int32_t
- uint32_t
- int64_t
- uint64_t
- uintptr_t
- intprt_t

和长度相关的数据类型
- size_t
- ssize_t

基本指针类型，不做限制如
- char *
- void *

内核对象的句柄
- cos_xxx_t *
如cos_task_t/cos_mutex_t/cos_sem_t/cos_queue_t等。

统一的错误码类型
- cos_status_t

其他
- bool

# 错误码
标准应采用统一的错误码，支持可扩展。采用`cos_status_t`数据类型，所有的错误码均为负值。返回0代表成功。
一些例外：如类似read/write API返回ssize_t类型，> 0 代表长度，< 0代表失败。

# 模块描述
标准中对一个功能模块的描述，应包含以下这些内容。
## 模块功能描述
包括解释模块功能是什么，阐述工作原理，使用功能的场景及其注意事项。

## 模块常量定义
包括模块定义的宏和枚举常量，命名全大写和下划线，统一**cos_**前缀。

## 模块数据类型定义
包括模块定义的复合数据类型。全小写，统一**cos_**前缀。

## 模块API描述内容
对一个API的描述需要完整，清晰，表达没有歧义。包括如下内容章节。

1. API名称和简短说明
2. API原型
API的原型申明和应该使用该API时需要包含的头文件。
3. API的详细描述
4. API的参数
包括参数名称，参数类型，参数方向，参数详细描述这几个要素。
5. API的返回值
包括API的返回值及其代表的含义。
6. API的使用注意
包括API的使用注意事项，例如能否在中断上下文中使用等。
7. 使用举例
给出使用该API的例子。

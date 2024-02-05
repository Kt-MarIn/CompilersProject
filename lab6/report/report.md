# Lab6

## Json数据类型介绍

我们了解到。JSON数据类型包括字符串、数字、布尔值、对象、数组、null。

- String: 字符串是由零个或多个字符组成的文本序列。它以双引号开头和结尾，例如"Hello World"。
- Number: 数字是一个十进制数，可以是正数、负数或零。例如12345.6789。
- Boolean: 布尔值表示真或假的信息，它只有两个值true或false。
- Object: 对象是一组无序的键/值对，键必须是字符串而值可以是任何JSON数据类型。例如{"name":"John", "age":30} 
- Array: 数组是一个有序的集合，其中的元素可以是任何JSON数据类型。例如["Apple", "Banana", "Orange"] 
- Null: null表示一个不存在的值，它不代表0或者false。

本次实验我的理解是type结构体中只需要存储数据类型即可，因此没有分配相应的value的空间



## Type结构体

我把这六种数据类型分为三个大类：BASIC, ARRAY, OBJECT作为kind，其中ARRAY, OBJECT为结构体，BASIC作为字符串、数字、布尔值、null的统称。type是三个kind中的一个，将他们放入union中进行存储，完成对type结构体的定义。在进行初始化时候先确认kind的value，再初始化他们的类型

```c
typedef struct Type {
    enum {
        BASIC,
        ARRAY,
        OBJECT
    } kind;
    union {
        enum {
            STRING,
            NUMBER,
            BOOL,
            VNULL
        } basic;
        struct ArrayList *ARRAYLIST;
        struct ObjectList *OBJECTLIST;
    };
} Type;
```



## ArrayList结构体

在json格式中的数组不受类型限制，可以为任何类型，因此它需要有一个type类型的值来记录其这一项的类型，还需要一个指向下一个节点的指针，定义如下

```c
typedef struct ArrayList {
    struct Type *ELEM_TYPE;
    struct ArrayList *next;
} ArrayList;
```



## ObjectList结构体

与数组不同的一点是对象类型的type记录方式是键值对的记录方式，需要一个字符串类型的值记录其key值，也就是需要一个Type恒为STRING的元素。还需要记录其value值的类型，以及一个指向下一个节点的指针，定义如下

```c
typedef struct ObjectList {
    struct Type *STRING_KEY_TYPE;
    struct Type *VALUE_TYPE;
    struct ObjectList *next;
} ObjectList;
```


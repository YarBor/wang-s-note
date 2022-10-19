来源: https://www.cnblogs.com/gdk-0078/p/5172941.html
阻塞与非阻塞是设备访问的两种方式。驱动程序需要提供阻塞（等待队列，中断）和非阻塞方式（轮询，异步通知）访问设备。在写阻塞与非阻塞的驱动程序时，经常用到等待队列。
## 阻塞与非阻塞 
阻塞调用是指**没有获得资源挂起的进程**,被挂起的进程进入休眠状态,**调用的函数只有在得到结果后才返回,进程继续**.  
非阻塞是指不能进行设备操作时不挂起,或返回,或反复查询,直到可以进行操作为止,被调用的函数不会iu阻塞当前进程,而会立即返回.  
> **对象是否处于阻塞模式和函数是不是阻塞调用有很强的相关性**，**但并不是一一对应的**。阻塞对象上可以有非阻塞的调用方式，我们可以通过一定的API去轮询 状态，在适当的时候调用阻塞函数，就可以避免阻塞。而对于非阻塞对象，调用的函数也可以进入阻塞调用。函数select()就是这样一个例子。
  
驱动程序常需要这种能力:   
* 当应用程序read()write()等系统调用时,若设备的资源不能获取,而用户又希望以阻塞的方式访问设备,驱动程序应在设备驱动程序的xxx_read(),xxx_write()等操作中将进程阻止到资源可以获取,以后，应用程序的read(),write()等调用返回，整个过程仍然进行了正确的设备访问，用户并没有感知到。  
* 若用户以非阻塞的方式访问设备文件，则当设备资源不可获取时，设备驱动的xxx_read(),xxx_write()等操作应立即返回，read()，write()等喜用调用也随即被访问

**阻塞不是低效率，如果设备驱动不阻塞，用户想获取设备资源只能不断查询，小号CPU资源，阻塞访问时，不能获取资源的进程将进入休眠，将CPU资源让给其他资源。**

阻塞的进程会进入休眠状态，因此，必须确保有一个地方能唤醒休眠的进程。唤醒进程的地方最大可能发生在终端里面，因为硬件资源的获得往往伴随着一个终端。  

## 等待队列
#### 定义

在linux驱动中,**可使用等待队列来实现阻塞进程的唤醒**,**以数列为基础结构,与进程调度机制紧密结合**用于视线内核的异步事件通知机制，也可用于同步对系统资源的访问。(信号量在内核中也依赖等待队列来实现)

在软件开发中任务经常由于某种条件没有得到满足而不得不进入睡眠状态，然后等待条件得到满足的时候再继续运行，进入运行状态。这种需求需要等待队列机制的支持。Linux中提供了等待队列的机制，该机制在内核中应用很广泛。

等待队列在linux内核中有着举足轻重的作用，很多linux驱动都或多或少涉及到了等待队列。因此，对于linux内核及驱动开发者来说，掌握等待队 列是必须课之

**Linux内核的等待队列是以双循环链表为基础数据结构**，与进程调度机制紧密结合，能够用于实现核心的异步事件通知机制。它有两种数据结构：**等待队列头 （wait_queue_head_t）和等待队列项（wait_queue_t）。等待队列头和等待队列项中都包含一个list_head类型的域作为”连接件”。它通过一个双链表和把等待task的头，和等待的进程列表链接起来。** *下面具体介绍。*
```c
struct __wait_queue_head {
      spinlock_t lock;                    /* 保护等待队列的原子锁 (自旋锁),在对task_list与操作的过程中，使用该锁实现对等待队列的互斥访问*/
      struct list_head task_list;          /* 等待队列,双向循环链表，存放等待的进程 */
};

/*__wait_queue，该结构是对一个等待任务的抽象。每个等待任务都会抽象成一个wait_queue，并且挂载到wait_queue_head上。该结构定义如下：*/

typedef struct __wait_queue wait_queue_t;

struct __wait_queue {
 unsigned int flags;
 #define WQ_FLAG_EXCLUSIVE   0x01
 void *private;                       /* 通常指向当前任务控制块 */
 wait_queue_func_t func;             
 struct list_head task_list;              /* 挂入wait_queue_head的挂载点 */
};

/* 任务唤醒操作方法，该方法在内核中提供，通常为auto remove_wake_function */
``` 
#### 操作
##### (1)定义并初始化"等待队列头"
```c
wait_queue_head_t my_queue;  
init_waitqueue_head(&my_queue);  //会将自旋锁初始化为未锁，等待队列初始化为空的双向循环链表。
//宏名用于定义并初始化，相当于"快捷方式"
DECLARE_WAIT_QUEUE_HEAD (my_queue);  
```
##### (2)定义"等待队列"

```c
定义并初始化一个名为name的等待队列 ,注意此处是定义一个wait_queue_t类型的变量name，并将其private与设置为tsk

DECLARE_WAITQUEUE(name,tsk);
 
 typedef struct __wait_queue wait_queue_t;

struct __wait_queue {
 unsigned int flags;
#define WQ_FLAG_EXCLUSIVE   0x01
 void *private;                       /* 通常指向当前任务控制块 */
 wait_queue_func_t func;             
 struct list_head task_list;              /* 挂入wait_queue_head的挂载点 */
};

```
其中flags域指明该等待的进程是互斥进程还是非互斥进程。其中0是非互斥进程，WQ_FLAG_EXCLUSIVE(0×01)是互斥进程。等待队列 (wait_queue_t)和等待对列头(wait_queue_head_t)的区别是等待队列是等待队列头的成员。也就是说等待队列头的task_list域链接的成员就是等待队列类型的(wait_queue_t)
##### (3)(从等待队列头中)添加/删除等待队列
```c
struct __wait_queue_head {
      spinlock_t lock;                    /* 保护等待队列的原子锁 (自旋锁),在对task_list与操作的过程中，使用该锁实现对等待队列的互斥访问*/
      struct list_head task_list;          /* 等待队列,双向循环链表，存放等待的进程 */
};

//设置等待的进程为非互斥进程，并将其添加进等待队列头(q)的队头中。
void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait)
{
    unsigned long flags;
    wait->flags &= ~WQ_FLAG_EXCLUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    __add_wait_queue(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
}
EXPORT_SYMBOL(add_wait_queue);

//下面函数也和add_wait_queue()函数功能基本一样，只不过它是将等待的进程(wait)设置为互斥进程。
void add_wait_queue_exclusive(wait_queue_head_t *q, wait_queue_t *wait)
{
    unsigned long flags;
    wait->flags |= WQ_FLAG_EXCLUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    __add_wait_queue_tail(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
    }
    EXPORT_SYMBOL(add_wait_queue_exclusive);

//在等待的资源或事件满足时，进程被唤醒，使用该函数被从等待头中删除。
void remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait)
{
     unsigned long flags; 
     spin_lock_irqsave(&q->lock, flags);
     __remove_wait_queue(q, wait);
     spin_unlock_irqrestore(&q->lock, flags);
}
EXPORT_SYMBOL(remove_wait_queue);
```
***
未看完
*** 
(4) 等待事件

```c
#define wait_event(wq, condition)                  
do {                                   
        if (condition)                         
         break;                         
        __wait_event(wq, condition);                   
} while (0)
wait_event(queue,condition);等待以queue为等待队列头等待队列被唤醒，condition必须满足，否则阻塞  
wait_event_interruptible(queue,condition);可被信号打断  
wait_event_timeout(queue,condition,timeout);阻塞等待的超时时间，时间到了，不论condition是否满足，都要返回  
wait_event_interruptible_timeout(queue,condition,timeout) 

```
wait_event()宏：

在等待会列中睡眠直到condition为真。在等待的期间，进程会被置为TASK_UNINTERRUPTIBLE进入睡眠，直到condition变量变为真。每次进程被唤醒的时候都会检查condition的值.

wait_event_interruptible()函数：

和wait_event()的区别是调用该宏在等待的过程中当前进程会被设置为TASK_INTERRUPTIBLE状态.在每次被唤醒的时候,首先检查 condition是否为真,如果为真则返回,否则检查如果进程是被信号唤醒,会返回-ERESTARTSYS错误码.如果是condition为真,则 返回0.

wait_event_timeout()宏:

也与wait_event()类似.不过如果所给的睡眠时间为负数则立即返回.如果在睡眠期间被唤醒,且condition为真则返回剩余的睡眠时间,否则继续睡眠直到到达或超过给定的睡眠时间,然后返回0.

wait_event_interruptible_timeout()宏

与wait_event_timeout()类似,不过如果在睡眠期间被信号打断则返回ERESTARTSYS错误码.

wait_event_interruptible_exclusive()宏

同样和wait_event_interruptible()一样,不过该睡眠的进程是一个互斥进程.

(5)唤醒队列

```c
/*
 __wake_up - wake up threads blocked on a waitqueue.
 @q: the waitqueue
 @mode: which threads
 @nr_exclusive: how many wake-one or wake-many threads to wake up
 @key: is directly passed to the wakeup function
*/
void __wake_up(wait_queue_head_t *q, unsigned int mode,
                int nr_exclusive, void *key)
{
    unsigned long flags;
    spin_lock_irqsave(&q->lock, flags);
    __wake_up_common(q, mode, nr_exclusive, 0, key);
    spin_unlock_irqrestore(&q->lock, flags);
}
    EXPORT_SYMBOL(__wake_up);
//唤醒等待队列.可唤醒处于TASK_INTERRUPTIBLE和TASK_UNINTERUPTIBLE状态的进程,和wait_event/wait_event_timeout成对使用

#define wake_up_interruptible(x)    __wake_up(x, TASK_INTERRUPTIBLE, 1, NULL)
//和wake_up()唯一的区别是它只能唤醒TASK_INTERRUPTIBLE状态的进程.,与wait_event_interruptible
wait_event_interruptible_timeout
wait_event_interruptible_exclusive成对使用

#define wake_up_all(x)          __wake_up(x, TASK_NORMAL, 0, NULL) 
#define wake_up_interruptible_nr(x, nr) __wake_up(x, TASK_INTERRUPTIBLE, nr, NULL)
#define wake_up_interruptible_all(x)    __wake_up(x, TASK_INTERRUPTIBLE, 0, NULL)
//这些也基本都和wake_up/wake_up_interruptible一样
```
wake_up()与wake_event()或者wait_event_timeout成对使用，
wake_up_intteruptible()与wait_event_intteruptible()或者wait_event_intteruptible_timeout()成对使用。

(6) 在等待队列上睡眠：

sleep_on()函数

```c
void __sched sleep_on(wait_queue_head_t *q)
{  
  sleep_on_common(q, TASK_UNINTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);
}

static long __sched
sleep_on_common(wait_queue_head_t *q, int state, long timeout)
{
        unsigned long flags;
        wait_queue_t wait;
        init_waitqueue_entry(&wait, current);
        __set_current_state(state);
        spin_lock_irqsave(&q->lock, flags);
        __add_wait_queue(q, &wait);
        spin_unlock(&q->lock);
        timeout = schedule_timeout(timeout);
        spin_lock_irq(&q->lock);
        __remove_wait_queue(q, &wait);
        spin_unlock_irqrestore(&q->lock, flags); 
        return timeout;
}
```
该函数的作用是定义一个等待队列(wait)，并将当前进程添加到等待队列中(wait)，然后将当前进程的状态置为 TASK_UNINTERRUPTIBLE，并将等待队列(wait)添加到等待队列头(q)中。之后就被挂起直到资源可以获取，才被从等待队列头(q) 中唤醒，从等待队列头中移出。在被挂起等待资源期间，该进程不能被信号唤醒。

```c
sleep_on_timeout()函数

long __sched sleep_on_timeout(wait_queue_head_t *q, long timeout)
{
       return sleep_on_common(q, TASK_UNINTERRUPTIBLE, timeout);
}
```
EXPORT_SYMBOL(sleep_on_timeout);
与sleep_on()函数的区别在于调用该函数时，如果在指定的时间内(timeout)没有获得等待的资源就会返回。实际上是调用 schedule_timeout()函数实现的。值得注意的是如果所给的睡眠时间(timeout)小于0，则不会睡眠。该函数返回的是真正的睡眠时间。
```c
interruptible_sleep_on()函数

 void __sched interruptible_sleep_on(wait_queue_head_t *q)
{
        sleep_on_common(q, TASK_INTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);
}
```
EXPORT_SYMBOL(interruptible_sleep_on);
该函数和sleep_on()函数唯一的区别是将当前进程的状态置为TASK_INTERRUPTINLE，这意味在睡眠如果该进程收到信号则会被唤醒。
```c
interruptible_sleep_on_timeout()函数：

long __sched
interruptible_sleep_on_timeout(wait_queue_head_t *q, long timeout)
{
    return sleep_on_common(q, TASK_INTERRUPTIBLE, timeout);
}
```
EXPORT_SYMBOL(interruptible_sleep_on_timeout);
类似于sleep_on_timeout()函数。进程在睡眠中可能在等待的时间没有到达就被信号打断而被唤醒，也可能是等待的时间到达而被唤醒。

以上四个函数都是让进程在等待队列上睡眠，不过是小有诧异而已。在实际用的过程中，根据需要选择合适的函数使用就是了。例如在对软驱数据的读写中，如果设 备没有就绪则调用sleep_on()函数睡眠直到数据可读(可写)，在打开串口的时候，如果串口端口处于关闭状态则调用 interruptible_sleep_on()函数尝试等待其打开。在声卡驱动中，读取声音数据时，如果没有数据可读，就会等待足够常的时间直到可读取。
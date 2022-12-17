<!-- 王康宁, [2022/12/16 11:58] -->
<!-- 好像是这个 -fsanitize=address -->
<!--  -->
<!-- 王康宁, [2022/12/16 12:01] -->
<!-- -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -->
<!--  -->
<!-- 王康宁, [2022/12/16 12:01] -->
<!-- 这两句好像都可以用来检查内存泄露。 -->
<!--  -->
<!-- 王康宁, [2022/12/16 12:01] -->
<!-- 可能内存泄露的形式有很多种，这个命令也有很多种不同侧重点的。 -->

## GDB 常用调试技巧
### 1.启动gdb
执行命令
- 首先,进行编译
```shell
$ gcc -g ***** -o test
```
- 运行gdb
```shell
$ gdb test
#或者执行 $ gdb -q test
# 不打印版本信息 界面较为简洁
```
![image.png](https://s2.loli.net/2022/11/03/MpcZU3QiRWBfDSk.png)

![image.png](https://s2.loli.net/2022/11/03/4rjbVe2986c7yIu.png)
- __如上 即打开了gdb__
### 2.查看源代码 -list
使用命令 `list` 或者 `l` 
gdb在默认条件下 一次显示10行 `按回车`或继续输入`list/l`继续查看
- `l + 函数名`  展示相应函数
- `l + 行数`  展示对应行数
- `l / list` 默认显示main函数附近
```c
(gdb) list
179		if(head==NULL)	return ;
180		if (!head->value)
181		{
182			return;
183			/* code */
184		}
185		
186		while (head)
187		{
188			printf("%d ", head->value);
(gdb) l
189			head = head->next;
190		}
191	}
192	
193	int main() {
194		list* wang = NULL;
195		HEAD_INSERT(wang, 2);
196		TAIL_INSERT(wang, 3);
197		TAIL_INSERT(wang, 4);
198		HEAD_INSERT(wang, 1);
(gdb) 
199		NODE_DELETE(wang,4);
200		NODE_DELETE(wang,1);
201		NODE_DELETE(wang,2);
202		NODE_DELETE(wang,3);
203		HEAD_INSERT(wang, 1);
204		TAIL_DELETE(wang);
205		TAIL_INSERT(wang, 4);
206		HEAD_DELETE(wang);
207		PRINT_LIST(wang);
208		LIST_NODE_CHANGE(wang, 3, 5);
(gdb) 
```
> 这里的代码默认从main函数附近开始显示
> 这里的源代码是C语言中的单链表操作

- l + 函数名 可查看相应函数的代码
```c
(gdb) l head_insert_
34	void head_insert(list**head,int value)
35	{
36	    head_insert_(head,value);
37	}
38	void head_insert_(list**head,int value)
39	{
40		if (!(*head))
41		{
42			(*head) = (list*)malloc(sizeof(list));
43			assert((*head));
(gdb) 
44			(*head)->next = NULL;
45			(*head)->value = value;
46			return;
47		}
48		else
49		{
50			list* a = (list*)malloc(sizeof(list));
51			assert(a);
52			a->next = (*head);
53			a->value = value;
(gdb) 
54			(*head) = a;
55			return;
56		}
57	
58	}
59	void tail_insert(list ** head,int value)
60	{
61	    tail_insert_(head,value);
62	}

```
- l + 行数 可以查看相应行数的代码 
```c
(gdb) l 1
1	#include <string.h>
2	#include <stdio.h>  
3	#include <stdlib.h>
4	#include <assert.h>
5	#include<stdbool.h>
6	typedef struct list {
7		int value;
8		struct list* next;
9	}list;
10	#define HEAD_INSERT(X,Y) head_insert(&X,Y)
(gdb) 
11	#define TAIL_INSERT(X,Y) tail_insert(&X,Y)
12	#define TAIL_DELETE(X) tail_delete(&X)
13	#define HEAD_DELETE(X) head_delete(&X)
14	#define LIST_FIND(X,Y) find_for_list(X,Y)
15	#define LIST_NODE_CHANGE(X,Y,Z) list_node_change(X,Y,Z)
16	#define NODE_DELETE(X,Y) specified_node_delete_list(&X,Y)
17	#define PRINT_LIST(X) print_list(X)
18	
19	void head_insert_(list**head,int value);
20	void tail_insert_(list** head, int value);
(gdb) 
```
### 打断点
info breakpoints / info b ：显示断点信息
#### 普通断点(break)
`break/b`进行标记断点操作
- `break/b + 函数名` 在函数执行时进行打断
- `break/b + 行数` 在执行到相应行数时进行打断
break(简写 b) ：格式 b 行号，在某行设置断点；
info (breakpoints/b) ：显示断点信息
Num： 断点编号
Disp：断点执行一次之后是否有效 kep：有效 dis：无效
Enb： 当前断点是否有效 y：有效 n：无效
Address：内存地址
What：位置
```c
(gdb) b 36
Breakpoint 1 at 0x11dc: file test.c, line 36.
(gdb) b head_delete_
Breakpoint 2 at 0x1425: file test.c, line 87.
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x00000000000011dc in head_insert at test.c:36
2       breakpoint     keep y   0x0000000000001425 in head_delete_ at test.c:87
(gdb)
(gdb) info breakpoints 
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x00000000000011dc in head_insert at test.c:36
2       breakpoint     keep y   0x0000000000001425 in head_delete_ at test.c:87
(gdb) 
```

#### 观察断点(watch)
`(gdb) watch cond`
其中 cond 即为要监控的表达式或者变量
- 其中，conde 指的就是要监控的变量或表达式。强调一下，watch 命令的功能是：只有当被监控变量（表达式）的值发生改变，程序才会停止运行。
- 和 watch 命令功能相似的，还有 rwatch 和 awatch 命令。其中：
    - rwatch 命令：只要**程序中出现读取目标变量**（表达式）的值的操作，**程序就会停止运行**；
    - awatch 命令：只要**程序中出现读取目标变量**（表达式）的值**或者改变值的操作**，**程序就会停止运行**。
- 查看观察断点`info watchpoints`
```c
> gdb -q test
Reading symbols from test...
(gdb) 
(gdb) l
1	#include<stdio.h>
2	#include<stdlib.h>
3	#include<string.h>
4	int main()
5	{
6		int n ;
7		scanf("%d",&n);
8		int * arr = (int *)malloc(sizeof(int)*(2*n));
9		memset(arr,0,8*n);
10		for(int a = 0;a<2*n-1;a++)
(gdb) 
11		{
12			scanf("%d",arr+a);
13		}
14		int tmp = 1;
15		for(int a = 0;a<2*n-1;a++)
16		{
17			tmp^=arr[a];
18		}
19		tmp ^= 1;
20		printf("%d",tmp);
(gdb) 
21	    free(arr);
22		return 0 ;
23	}

(gdb) b 5
Breakpoint 1 at 0x11f5: file test.c, line 5.

(gdb) r
Starting program: /home/wang/Desktop/object/test/test 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 1, main () at test.c:5
5	{
(gdb) watch tmp
Hardware watchpoint 2: tmp
(gdb) c
Continuing.

5
1 2 3 4 5 4 3 2 1

Hardware watchpoint 2: tmp

Old value = 100
New value = 1
main () at test.c:15
15		for(int a = 0;a<2*n-1;a++)
(gdb) info watchpoints 
Num     Type           Disp Enb Address            What
2       hw watchpoint  keep y                      tmp
	breakpoint already hit 1 time
(gdb) 


```

- 对于使用 watch（rwatch、awatch）命令监控 C、C++ 程序中变量或者表达式的值，有以下几点需要注意：
    - **当监控的变量（表达式）为局部变量（表达式）时，一旦局部变量（表达式）失效，则监控操作也随即失效；**
    - **如果监控的是一个指针变量（例如 *p），则 watch *p 和 watch p 是有区别的，前者监控的是 p 所指数据的变化情况，而后者监控的是 p 指针本身有没有改变指向；**
    - **这 3 个监控命令还可以用于监控数组中元素值的变化情况，例如对于 a[10] 这个数组，watch a 表示只要 a 数组中存储的数据发生改变，程序就会停止执行。**


#### 捕捉断点(catch)
咳咳 没弄明白 日后更新 
附上链接:>https://www.cnblogs.com/jkin/p/13829513.html

#### 删除断点
- 无论是普通断点、观察断点还是捕捉断点，都可以使用 clear 或者 delete 命令进行删除。

1. clear 
- `clear + 地点` 删除指定位置的断点
```c
(gdb) b 36
Breakpoint 4 at 0x11dc: file test.c, line 36.
(gdb) clear 36
Deleted breakpoint 4 
(gdb)_
```
- `clear + 函数` 删除函数入口位置的断点
```c
(gdb) clear head_delete_ 
Deleted breakpoint 2 
(gdb)_
```
2. delete 命令 
缩写为 d
`delete breakpointer (+ Num)`
`delete breakpointer`
```c
(gdb) info b
Num     Type           Disp Enb Address            What
5       breakpoint     keep y   0x0000000000001425 in head_delete_ at test.c:87
6       breakpoint     keep y   0x00000000000011dc in head_insert at test.c:36
(gdb) delete breakpoints 
Delete all breakpoints? (y or n) y  //会进行询问 //输入y会删除所有断点
(gdb) info b
No breakpoints or watchpoints.
```
```c
Num     Type           Disp Enb Address            What
7       breakpoint     keep y   0x00000000000011dc in head_insert at test.c:36
8       breakpoint     keep y   0x0000000000001425 in head_delete_ at test.c:87

(gdb) delete breakpoints 7
(gdb) info b
Num     Type           Disp Enb Address            What
8       breakpoint     keep y   0x0000000000001425 in head_delete_ at test.c:87
(gdb) delete breakpoints 8
(gdb) info b
No breakpoints or watchpoints.
```
#### 禁用断点
所谓禁用，就是使目标断点暂时失去作用，必要时可以再将其激活，恢复断点原有的功能。
`disable`命令
`disable [breakpoints] [num...]`
disable 命令中 `breakpointers` 选项可选
disable 命令中 `num` 参数未输入时 将禁用所有
```c
(gdb) info b
Num     Type           Disp Enb Address            What
9       breakpoint     keep y   0x0000000000001559 in find_for_list at test.c:124
10      breakpoint     keep y   0x0000000000001811 in main at test.c:193
(gdb)
(gdb) disable 9
(gdb)
(gdb) info b
Num     Type           Disp Enb Address            What
9       breakpoint     keep n   0x0000000000001559 in find_for_list at test.c:124
10      breakpoint     keep y   0x0000000000001811 in main at test.c:193
// ````````````````````    |^|     `````````````````````````````````````````
//                      在此处不一样
(gdb) 
```

#### 启用断点
enable命令 用法与 disable命令相同 
仅仅将**禁止**换位**启用**

### 运行程序 -run
输入`run/r` 运行程序至结束或遇到断点
- `run/r`  运行程序至结束或遇到断点
- `run/r + 参数` 中的"参数"可以作main的argv 运行程序

### 查看函数栈
`bt`命令
```c
Breakpoint 1, print_ls_of_one_RRR (path=0x7fffffff89a0 "//sys/kernel/slab/:0000512", aaaaa=4) at my_ls.c:831
831	{
(gdb) bt
#0  print_ls_of_one_RRR (path=0x7fffffff89a0 "//sys/kernel/slab/:0000512", aaaaa=4)
    at my_ls.c:831
#1  0x000055555555859a in print_ls_of_one_RRR (path=0x7fffffff9a80 "//sys/kernel/slab", 
    aaaaa=4) at my_ls.c:877
#2  0x000055555555859a in print_ls_of_one_RRR (path=0x7fffffffab60 "//sys/kernel", aaaaa=3)
    at my_ls.c:877
#3  0x000055555555859a in print_ls_of_one_RRR (path=0x7fffffffbc40 "//sys", aaaaa=2)
    at my_ls.c:877
#4  0x000055555555859a in print_ls_of_one_RRR (path=0x55555555c2a0 "/", aaaaa=1)
    at my_ls.c:877
#5  0x00005555555555c6 in print_ls (path=0x55555555c2a0 "/") at my_ls.c:14
#6  0x00005555555582c6 in main (argc=3, argv=0x7fffffffdde8) at my_ls.c:823

// 这里是 C 语言代码 
// 从 main 调用  print_ls 
//再进行递归调用 print_ls_of_one_RRR 函数
//如上 函数栈共六层

```
### 单步调试
- `next/n`命令进行单步操作 不进入函数
一次一步
- `step/s` 命令进行单步操作 进入函数
一次一步


### 查看变量
- `print whatis`命令
```c

(gdb) print count   
$1 = 1              //输出变量的值

(gdb) whatis count   
type = int          //输出变量类型

(gdb)_ 
```

- `info locals`命令输出当前函数栈的所有变量详细信息
```c
(gdb) info locals //仅为举例
count = 1
m = {{ins_num = "12 12", '\000' <repeats 24 times>, name = {
      fname = "2 ", '\000' <repeats 12 times>, mname = "2 ", '\000' <repeats 12 times>, 
      lname = "2", '\000' <repeats 13 times>}}, {
    ins_num = '\000' <repeats 22 times>, "\200\000\000\000\000\000\000@", name = {
      fname = "\000\000\000\000\000\000\000\b\000\000\000\000\000\000", 
      mname = "@\000\000\000\000\000\000\000@\000\000\000\000\000", 
      lname = "\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377"}}, {
    ins_num = "\377\377\303\357\377\377\377\177\000\000\000\000\000\000\000\000\000\000\031\000\000\000P\000\000\000\000\000\000", name = {
      fname = "\020", '\000' <repeats 11 times>, "\002\000", 
      mname = "\000\000\000\000\000\006\000\000\000\000\000\000\200\000", 
      lname = '\000' <repeats 14 times>}}, {ins_num = '\000' <repeats 29 times>, name = {
      fname = "\000\r\000\000\000\000\000\000\000\001\000\000\000\000", 
      mname = "\000\000\001\000\000\000\000\000\000\000\001\000\000\000", 
      lname = "\000\000\000@@UUUU\000\000\\(\376", <incomplete sequence \367>}}, {
    ins_num = "\377\177\000\000\060\r\000\000\000\000\000\000\211\341\377\377\377\177\000\000\000\020\374\367\377\177\000\000\000", name = {
      fname = "\000\001\001\001\000\000\002\000\000\000\000\000\000\000\377", 
      mname = "\373\213\027\000\000\000\000\231\341\377\377\377\177\000", 
      lname = "d\000\000\000\000\000\000\000\000\020\000\000\000\000"}}}

```

- `show args`查看运行时设置的参数
```c
(gdb) show args
Argument list to give program being debugged when it is started is "".//未输入

(gdb) show args
Argument list to give program being debugged when it is started is "-asdf".//输入参数为'-asdf'

```



---
### gdb基本使用命令
1、运行命令
run：简记为 r ，其作用是运行程序，当遇到断点后，程序会在断点处停止运行，等待用户输入下一步的命令。
continue （简写c ）：继续执行，到下一个断点处（或运行结束）
next：（简写 n），单步跟踪程序，当遇到函数调用时，也不进入此函数体；此命令同 step 的主要区别是，step 遇到用户自定义的函数，将步进到函数中去运行，而 next 则直接调用函数，不会进入到函数体内。
step （简写s）：单步调试如果有函数调用，则进入函数；与命令n不同，n是不进入调用的函数的
until：当你厌倦了在一个循环体内单步跟踪时，这个命令可以运行程序直到退出循环体。
until+行号： 运行至某行，不仅仅用来跳出循环
finish： 运行程序，直到当前函数完成返回，并打印函数返回时的堆栈地址和返回值及参数值等信息。
call 函数(参数)：调用程序中可见的函数，并传递“参数”，如：call gdb_test(55)
quit：简记为 q ，退出gdb

2、设置断点
break n （简写b n）:在第n行处设置断点
（可以带上代码路径和代码名称： b OAGUPDATE.cpp:578）
b fn1 if a＞b：条件断点设置
break func（break缩写为b）：在函数func()的入口处设置断点，如：break cb_button
delete 断点号n：删除第n个断点
disable 断点号n：暂停第n个断点
enable 断点号n：开启第n个断点
clear 行号n：清除第n行的断点
info b （info breakpoints） ：显示当前程序的断点设置情况
delete breakpoints：清除所有断点：

3、查看源码
list ：简记为 l ，其作用就是列出程序的源代码，默认每次显示10行。
list 行号：将显示当前文件以“行号”为中心的前后10行代码，如：list 12
list 函数名：将显示“函数名”所在函数的源代码，如：list main
list ：不带参数，将接着上一次 list 命令的，输出下边的内容。

4、打印表达式
print 表达式：简记为 p ，其中“表达式”可以是任何当前正在被测试程序的有效表达式，比如当前正在调试C语言的程序，那么“表达式”可以是任何C语言的有效表达式，包括数字，变量甚至是函数调用。
print a：将显示整数 a 的值
print ++a：将把 a 中的值加1,并显示出来
print name：将显示字符串 name 的值
print gdb_test(22)：将以整数22作为参数调用 gdb_test() 函数
print gdb_test(a)：将以变量 a 作为参数调用 gdb_test() 函数
display 表达式：在单步运行时将非常有用，使用display命令设置一个表达式后，它将在每次单步进行指令后，紧接着输出被设置的表达式及值。如： display a
watch 表达式：设置一个监视点，一旦被监视的“表达式”的值改变，gdb将强行终止正在被调试的程序。如： watch a
whatis ：查询变量或函数
info function： 查询函数
扩展info locals： 显示当前堆栈页的所有变量

5、查看运行信息
where/bt ：当前运行的堆栈列表； 
bt backtrace 显示当前调用堆栈
up/down 改变堆栈显示的深度
set args 参数:指定运行时的参数
show args：查看设置好的参数
info program： 来查看程序的是否在运行，进程号，被暂停的原因。

6、分割窗口
layout：用于分割窗口，可以一边查看代码，一边测试：
layout src：显示源代码窗口
layout asm：显示反汇编窗口
layout regs：显示源代码/反汇编和CPU寄存器窗口
layout split：显示源代码和反汇编窗口
Ctrl + L：刷新窗口

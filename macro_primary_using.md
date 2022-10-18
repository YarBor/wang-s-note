# C 宏魔法的初级理解 
参考连接 ***https://zhuanlan.zhihu.com/p/152354031***
## 简介
和模板元编程不一样，宏编程 没有类型 的概念，输入和输出都是 符号 —— 不涉及编译时的 C++ 语法，只进行编译前的 文本替换：
* 一个**宏参数** 是一个任意的 **符号序列** ,不同的宏参数之间用逗号分割
* 每个参数都可以是空序列 而且空白字符会被忽略 (`a + 1` == `a+1`)  
* 在参数中 , 尽量使用将每一个元素用()封装 以免编译器使用参数中的',' 把该参数划为两个参数
    * （例如`#define FOO(bool, std::pair<int, int>)` 被认为是 FOO() 有三个参数：bool / std::pair<int / int>） 
    *  建议是写成`FOO((bool),(std::pair<int,int>)) `
#### 建议
让编译器 仅输出预处理结果
gcc -E 让编译器 在预处理结束后停止，不进行 编译、链接
gcc -P 屏蔽编译器 输出预处理结果的 行标记 (linemarker)，减少干扰
另外，由于输出结果没有格式化，建议先传给 clang-format 格式化后再输出
屏蔽 无关的 头文件
临时删掉 不影响宏展开的 #include 行
避免多余的 引用展开，导致实际关注的宏代码 “被淹没”

## 初级初级用法
使用 **'#+参数' 将参数转化为字符串**  或用 **## 将参数粘和在一起** 
```C
#include<cstdio>
#include<climits>
using namespace std;

#define STR(s) #s
#define CONS(a,b) int(a##e##b) 
 
int main(int argc, char **argv)
{
    printf(STR(asdf));
    printf("%d",CONS(1,2));
    return 0;
}
```
第一句 STR(asdf) 在预处理阶段 转换为 "asdf" //即打印字符串 asdf  
第二句 CONS(1,2) 在预处理阶段 转换为 1e2 整体相当于 printf("%d",1e2); 即打印 100  

## 宏的参数是另一个宏的时候  
!!! 注意 **凡是宏定义参数里有"#"/"##"的地方宏参数是不会展开的** 
### 非"#"/"##"的情况 
```C
#define TOW (2)
#define MUL(a,b) (a*b)

printf("%d*%d=%d",TOW,TOW,MUL(TOW,TOW));
```
MUL()里的TOW是会被转换为(2)的 
即 **宏定义中无#时在源代码中是可以任意嵌套使用的**  
###  当有'#'或'##'的时候 
```C
#define A (2) 
#define STR(s) #s
#define CONS(a,b) (a##e##b)
//相当于在这里定义
//define INT_MAX  0x7fffffff

// 在此 我们要拿到字符串形式的 "0x7fffffff" 
printf("int max: %s  ", STR(INT_MAX));              // INT_MAX #include<climits>  
        //但拿到了"INT_MAX"    
        //printf("int max: %s  ", "INT_MAX");  

// 同理 要得到 "200"        
printf("%d  ", CONS(A, A)); // compile error   
        //得到了 int(AeA)
        //printf("%d  ", int(AeA));  
```
然而 为什么?
#### 符号拼接 
* 在宏编程中 , 符号拼接 通过 ## 将宏函数的参数 拼接为其他符号, 再进一步展开为目标结果, 这是实现宏编程的基础  

然而 一个宏参数用于拼接标识符 那么他不会被展开 (例如上面的CONS(A,A))。 

***一种通用的方法是延迟拼接操作***
```C
#define _STR(s) #s
#define STR(s) _STR(s)
//define INT_MAX 0x7fffffff

#define A(2)
#define _CONS(a,b) int(a##e##b)
#define CONS(a,b) _CONS(a,b)

printf("int max: %s  ", STR(INT_MAX));  //-> printf("int max: %s", "0x7fffffff");
printf("%d  ", CONS(A, A));             //-> printf("%d  ",2e2);
```
底层原理:>
* 在进入宏函数前，所有 宏参数 会先进行一次 预扫描 (prescan)，完全展开**未用于**拼接标识符 或 获取字面量 的所有参数
* 在宏函数展开时，用（预扫描展开后的）参数替换 展开目标里的 同名符号
* 在宏函数展开后，替换后的文本会进行 二次扫描(scan twice)，继续展开 结果里出现的宏
* 所以，CONS()先展开参数，再传递给 _CONS() 进行 实际拼接  

#### '#'和'##'的一些应用特例
##### 合并匿名变量名
```C
#define ___ANONYMOUS1(type, var, line) type var##line  
#define __ANONYMOUS0(type, line) ___ANONYMOUS1(type, _anonymous, line)  
#define ANONYMOUS(type) __ANONYMOUS0(type, __LINE__)  
```
例：ANONYMOUS(static int); 即: static int _anonymous70; 70表示该行行号；  
第一层：ANONYMOUS(static int); --> __ANONYMOUS0(static int,\_\_LINE\_\_);  
第二层： --> ___ANONYMOUS1(static int, _anonymous, 70);  
第三层： --> static int _anonymous70;  
即每次只能解开当前层的宏，所以__LINE__在第二层才能被解开； 
##### 填充结构
```C
#define FILL(a) {a, #a}  

enum IDD{OPEN, CLOSE};  
typedef struct MSG{  
  IDD id;  
  const char * msg;  
}MSG;  

MSG _msg[] = {FILL(OPEN), FILL(CLOSE)};  
```
这里的_msg[]相当于{{OPEN,"OPEN"},{CLOSE,"CLOSE"}};
##### 得到一个数值类型所对应的字符串缓冲大小  
```C
#define _TYPE_BUF_SIZE(type) sizeof#type 
#define TYPE_BUF_SIZE(type) _TYPE_BUF_SIZE(type)

char buf[TYPE_BUF_SIZE(INT_MAX)];
// == char buf[11]
```
这里相当于
  --> char buf[_TYPE_BUF_SIZE(0x7fffffff)];  
  --> char buf[sizeof "0x7fffffff"];  
两步 , 声明了一个char buf[11];
*** 
DONE
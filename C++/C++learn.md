## 变量
### 1. 声明和定义 
  - 变量可以多次声明,只能定义一次

变量的声明和定义是面向编译器的
   - **声明**
        是只告诉编译器 此处将有一个函数/变量 其为**类型 仅仅传递变量的信息而不会分配内存 但是分配的内存在其他地方定义，可能在同一个编译文件也可能在不同的文件中。
   - **定义**
        是告诉编译器 此处将有一个函数/变量 这个函数在哪 需要向内存申请多大的空间

在声明中，**变量的数据类型是已知的**	在定义中，**存储在变量中的值是指定的**。


### 2. 引用
引用不是定义一个新变量,而是给变量取一个别名,编译器不会为引用变量开辟内存空间,它和它引用的变量共用一块内存空间
- 引用变量的类型必须与它的实体类型一致
- 引用变量使用必须进行初始化
- 一个变量可以有多个引用
- 一旦引用一个实体,在不能引用其他实体(别名叫混了就ji)
- 引用不是指针

引用常量时要创建常量引用`const int & "name"` 

#### 左值和右值
https://nettee.github.io/posts/2018/Understanding-lvalues-and-rvalues-in-C-and-C/
<!-- 左值是可寻址的 ,具有持久性
而右值一般是不可寻址的常量,或在表达式求值中创建的无名临时对象,短暂性的.
左值和右值的唯一区别是左值可以被修改,右值不能 -->

- 左值 (lvalue, locator value) 表示了一个占据内存中某个可识别的位置（也就是一个地址）的对象。  
- 右值 (rvalue) 则使用排除法来定义。一个表达式不是 左值 就是 右值 。 那么，右值是一个 不 表示内存中某个可识别位置的对象的表达式。

- 左值引用和右值引用
  左值引用: 引用一个对象 
  右值引用: 就是必须绑定到右值的引用,C++11中右值引用可以实现"移动语意", 通过`&&`获得右值引用
  ```C
  int x = 6; // x是左值，6是右值
  int &y = x; // 左值引用，y引用x
  
  int &z1 = x * 6; // 错误，x*6是一个右值
  const int &z2 =  x * 6; // 正确，可以将一个const引用绑定到一个右值
  
  int &&z3 = x * 6; // 正确，右值引用
  int &&z4 = x; // 错误，x是一个左值
  ```
- 常量左值可以引用右值
虽然 C++98/03 标准不支持为右值建立非常量左值引用，但允许使用常量左值引用操作右值。也就是说，常量左值引用既可以操作左值，也可以操作右值
```cpp
int num = 10;
const int &b = num;
const int &c = 10;
```
##### C++右值引用
https://www.internalpointers.com/post/c-rvalue-references-and-move-semantics-beginners
C++0x 引入了一种称为右值引用&&的新类型，通过在某些类型后放置一个双符号来表示。这样的右值引用可以让你修改临时对象的值：就像const在上面第二行中删除属性一样！

让我们玩一下这个新玩具：
```cpp
  std::string   s1     = "Hello ";
  std::string   s2     = "world";
  std::string&& s_rref = s1 + s2;    // the result of s1 + s2 is an rvalue
  s_rref += ", my friend";           // I can change the temporary string!
  std::cout << s_rref << '\n';       // prints "Hello world, my friend"
```
在这里，我创建了两个简单的字符串s1和s2。我加入他们并将结果（一个临时字符串，即右值）放入std::string&& s_rref. Nows_rref是对临时对象的引用，或右值引用。它const周围没有，所以我可以根据需要自由修改临时字符串。如果没有右值引用及其双符号符号，这是不可能的。为了更好地区分它，我们将传统的 C++ 引用（单与号引用）称为左值引用。

乍一看，这似乎毫无用处。然而，右值引用为移动语义的实现铺平了道路，移动语义是一种可以显着提高应用程序性能的技术。

##### 移动语意（学完「类」来看）
https://www.internalpointers.com/post/c-rvalue-references-and-move-semantics-beginners
#### 引用和函数
1. 返回局部变量的引用时无法作为左值使用（包括函数的形参）
   - 当函数返回值为引用时, 若返回局部变量，不能成为其它引用的初始值，不能作为左值使用
    相当于仅仅返回值（右值）


2. 返回函数的引用形参作为引用, 可成为其他引用的初始值, 也可以作为左值, 也可作为右值
   - 传参可以使用引用接受(指针、变量都可)
   ```C
    void change(int *&a, int &b)
    {
        a = &(b);
        b++;
    }
    int main()
    {
        int a = 0;
        int *hhh = &a;
        int b = 10;
        change(hhh, b);
        std::cout << *hhh << " " << b << " " << a << std::endl;
        //            11            11          0
    }改变了指针指向 改变了变量b的大小（未通过传输指针） 
   ```
   用引用为参数时 即对传入**变量内存进行访问、修改**
3. 返回静态变量 或 全局变量的引用, 可成为其他引用的初始值, 也可以作为左值, 也可作为右值

---
   
### const关键字
https://www.cnblogs.com/jiabei521/p/3335676.html
常类型是指使用类型修饰符const说明的类型，常类型变量或是对象的值是不能被更新的。

const修饰符可以把对象转变成常数对象，
意思就是说利用const进行修饰的变量的值在程序的任意位置将不能再被修改，就如同常数一样使用！
任何修改该变量的尝试都会导致编译错误

**因为常量在定义之后就不能修改，所以const对象在定义时就必须初始化：**
对于**类中的const成员变量必须通过初始化列表进行初始化**，如下所示：
```cpp
class A
{
public:
  A(int i);
  void print();
  
  const int &r;
private:
  const int a;
  static int b;
};

const int A::b=10;//类列表初始化

A::A(int i):a(i),r(a)//类列表初始化
{
}
```
- **const对象默认为文件作用域** 
```cpp
//在全局的const变量
int const a = 10;
//仅仅在当前文件有效

// 若要在整个文件中使用当前定义的常量
// 需要在定义时加上extren关键字
extern const int BufferSize = 1024;
// 如此这个buffersize对整个程序可见
```
- const 对象的引用
```cpp
const int n = 16;
const int &nl = n;//合法
int &nll = n;     //error

int f = 0;
const int &fl = f;
const int &fll = f + n;
```
> 非const引用只能绑定到与该引用相同类型的对象。 const引用则可以绑定到不同但相关的类型的对象或绑定到右值。
```cpp
double a = 0.01;

const int &aa = a;
//合法 仅有整数位

int &aaa = a;
//不合法 报错
// 若不报错 在后续代码中万一修改了 aaa 对 a 的影响是未知的
```

- const对象的动态数组  
如果我们在自由存储区中创建的数组存储了内置类型的const对象，则必须为这个数组提供初始化： 因为数组元素都是const对象，无法赋值。实现这个要求的唯一方法是对数组做值初始化
```cpp
const int * p = new const int[100]
// error

const int * p2 = new const int[100]()
// OK
```
- C++允许定义类类型的const数组，但该类类型必须提供默认构造函数：
```cpp
const string * pstr = new string[100];
// 这里便会调用string类的默认构造函数初始化数组元素。
```

- 指针和const的关系
```cpp
int a = 1;
const int *b = &a;  // 底层const
int * const c = &a; // 顶层const

int h;
b = &h;
c = &h; // error

(*c)++;
(*b)++; // error 

```
底层const指针--不能通过指针改变指向数据的值 但可以改变指针的指向
顶层const指针--可以通过指针改变指向数据的值 但不能改变指针的指向
```cpp
const int f = 1;
const int * ff = &f;
const int * const fff = &f;
int * const ffff = &f;  //error
```
对于const对象 不能通过底层const指针指向 编译器报error

- typedef 定义的`指针`别名是常量
```cpp
typedef char * wsad;// !! w 是一个指针常量 》> 底层const
// wsad a;         // 不允许改变 a 的值（a是指针）
```
!!! 1 a 是一个指针常量 >> 底层const----不允许改变 a 的值(a是指针)
```cpp
int main(int argc, char** argv)
{
    char aa = 'a';
    char r = 'r';
    
                 wsad a = &aa; 
// 相当于 const (char *) a = &aa;

    (*a)++;// OK
    
    a = r;//error
}

```

### constexpr 
> 常量表达式：是指值不会改变的、并且在编译时期就可以计算得到结果的式子。
  
> 在C++中const可以由常量表达式进行声明 也可以用函数返回值进行声明（不确定的数）
> {即const的声明不一定用常量表达式}
> {即在运行时才会拿到结果}

constexpr 变量 
C++ 允许将变量声明为`constexpr`类型 以便由编译器来验证变量的值是否为一个常量表达式
**声明为 constexpr 类型的变量必须由常量进行初始化**

constexpr关键字只能用于修饰`字面值类型`（such：int、char、int *，int & 都属于）
不能修饰自定义类

**特别的 返回值为`constexpr`关键字修饰时 函数将隐式声明为`inline`类型**
        
## \<string\>
```cpp
#include<string>
using std::string;
```
### 定义和初始化string
```cpp
string s1;
string s2(s1); //通过 s1 的数据直接初始化 s2
string s2 = s1;//向 s2 拷贝 s1
string s3 = "hello world";// 拷贝
string s3("hello world");// 直接初始化
string s4(10,'w');// s4 == “wwwwwwwwww”
```
### string对象的操作
```cpp
string s()// 创建string s
os<<s//将s 写到输出流os中 返回os
is>>s//从is(输入流) 读取字符串到赋给 s ,字符串以空白字符分割 返回is 
getline(is,s)//从is 读入一行到s
s.empty()//将s晴空
s.size()//返回s串的长度
s1+s2
s1=s2
s1+=s2
s1==s2
< > <= >= != // 返回0/1 以字典序比较
```
string对象在读入的时候·会自动忽略串前面的空格
输入`“    hhh     ”`时 string对象只会储存 `“hhh”`

- getline(「输入流」，「string对象」);
  getline()遇到换行符停止，并丢弃换行符，将剩余数据存入string对象
  getline 不会忽略每次读入的空字符 仅仅靠`\n`分割。

- string.size()返回无符号数

- 在声明时 不能将两个字面值常量进行相加 · 
  ```cpp
  string s = " wang "; 
  string a = "fads" + "asfd" + s;//error
  string c = "fasdf" + ("wang" + s);//OK
  string b = string("fads") + "asfd" + s;//OK
  ```

#### 对于string的for遍历
- `for (declaration : expression) { statements }`
  
```cpp
string strings("fasdf"); 
for(auto x:strings) //这里的x是临时拷贝 // x-type: char 
  /* codings */

  // 这里对x的修改无法对string对象内的字符造成影响


for(auto &x:strings) //这里的x是引用 // x-type: char 
  /* codings */
```

- 下标遍历

`for(decltype(strings.size()) i = 0;i < strings.size();i++)`

## <vector\>
```cpp
#include<vector>
using std::vector;
```
### 定义和初始化
```cpp
vector<type> v1; // 创建一个空的容器
vector<type> v2(v1); // v2 中包含 v1 的所有副本
vector<type> v2 = v1; // 等价上面那个
vector<type> v3(m,value); // v3 中包含了 m 个 值为value的type类
vector<type> v4(i) // v4 中包含了 i 个默认初始化的type类
vector<type> v5{1,2,3,5} //v5 中包含了初始值依次为括号内部的值的类
vector<type> v5 = {1,2,3,5} // 等价上一个
```
使用「圆括号」则其数据使用来构造的
使用「大括号」则其数据是直接初始化的
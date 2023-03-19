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

- 左值 (lvalue, locator value) 表示了一个占据内存中某个可识别的位置(也就是一个地址)的对象。  
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
在这里，我创建了两个简单的字符串s1和s2。我加入他们并将结果(一个临时字符串，即右值)放入std::string&& s_rref. Nows_rref是对临时对象的引用，或右值引用。它const周围没有，所以我可以根据需要自由修改临时字符串。如果没有右值引用及其双符号符号，这是不可能的。为了更好地区分它，我们将传统的 C++ 引用(单与号引用)称为左值引用。

乍一看，这似乎毫无用处。然而，右值引用为移动语义的实现铺平了道路，移动语义是一种可以显着提高应用程序性能的技术。

##### 移动语意(学完「类」来看)
https://www.internalpointers.com/post/c-rvalue-references-and-move-semantics-beginners
#### 引用和函数
1. 返回局部变量的引用时无法作为左值使用(包括函数的形参)
   - 当函数返回值为引用时, 若返回局部变量，不能成为其它引用的初始值，不能作为左值使用
    相当于仅仅返回值(右值)


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
    }改变了指针指向 改变了变量b的大小(未通过传输指针) 
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
// wsad a;         // 不允许改变 a 的值(a是指针)
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
  
> 在C++中const可以由常量表达式进行声明 也可以用函数返回值进行声明(不确定的数)
> {即const的声明不一定用常量表达式}
> {即在运行时才会拿到结果}

constexpr 变量 
C++ 允许将变量声明为`constexpr`类型 以便由编译器来验证变量的值是否为一个常量表达式
**声明为 constexpr 类型的变量必须由常量进行初始化**

constexpr关键字只能用于修饰`字面值类型`(such：int、char、int *，int & 都属于)
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
1.顺序序列
顺序容器中的元素按照严格的线性顺序排序。可以通过元素在序列中的位置访问对应的元素。

2.动态数组
支持对序列中的任意元素进行快速直接访问，甚至可以通过指针算述进行该操作。提供了在序列末尾相对快速地添加/删除元素的操作。

3.能够感知内存分配器的(Allocator-aware)
容器使用一个内存分配器对象来动态地处理它的存储需求。
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

但如果大括号内的数据不能用于列表初始化类 则其数据将用来构造
```cpp
vector<string> v5{10};//将创建有用10个默认初始化的strings的容器
```
### 向vector内添加元素
```cpp
string word;
vector<string> text;
while (cin>>word)
  text.push_back(word)
```
增加函数
```cpp
void push_back(const T& x):向量尾部增加一个元素X
iterator insert(iterator it,const T& x):向量中迭代器指向元素前增加一个元素x
iterator insert(iterator it,int n,const T& x):向量中迭代器指向元素前增加n个相同的元素x
iterator insert(iterator it,const_iterator first,const_iterator last):向量中迭代器指向元素前插入另一个相同类型向量的`[first,last)`间的数据
```

删除函数
```cpp
iterator erase(iterator it):删除向量中迭代器指向元素
iterator erase(iterator first,iterator last):删除向量中[first,last)中元素
void pop_back():删除向量中最后一个元素
void clear():清空向量中所有元素
```

遍历函数
```cpp
reference at(int pos):返回pos位置元素的引用
reference front():返回首元素的引用
reference back():返回尾元素的引用
iterator begin():返回向量头指针，指向第一个元素
iterator end():返回向量尾指针，指向向量最后一个元素的下一个位置
reverse_iterator rbegin():反向迭代器，指向最后一个元素
reverse_iterator rend():反向迭代器，指向第一个元素之前的位置
```

判断函数
```cpp
bool empty() const:判断向量是否为空，若为空，则向量中无元素
```

大小函数
```cpp
int size() const:返回向量中元素的个数
int capacity() const:返回当前向量所能容纳的最大元素值
int max_size() const:返回最大可允许的vector元素数量值
```

其他函数
```cpp
void swap(vector&):交换两个同类型向量的数据
void assign(int n,const T& x):设置向量中前n个元素的值为x
void assign(const_iterator first,const_iterator last):向量中[first,last)中元素设置成当前向量元素
```

### 迭代器(iterable)
迭代器(iterator)是一种可以遍历容器元素的数据类型。迭代器是一个变量，相当于容器和操纵容器的算法之间的中介。
**C++更趋向于使用迭代器而不是数组下标操作，因为标准库为每一种标准容器(如vector、map和list等)定义了一种迭代器类型，而只有少数容器(如vector)支持数组下标操作访问容器元素。**可以通过迭代器指向你想访问容器的元素地址，通过*x打印出元素值。
这和我们所熟知的指针极其类似。

C语言有指针，指针用起来十分灵活高效。
C++语言有迭代器，迭代器相对于指针而言功能更为丰富。

- vector，是数组实现的，也就是说，只要知道数组的首地址，就能访问到后面的元素。
  - 我们可以通过访问vector的迭代器来遍历vector容器元素。
- List，是链表实现的，我们知道，链表的元素都存储在一段不是连续的地址空间中。我们需要通过next指针来访问下一个元素。
  - 我们也可以通过访问list的迭代器来实现遍历list容器元素。


#### 迭代器的使用
```
https://blog.csdn.net/weixin_47700137/article/details/119251703?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522167126453216782395356136%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=167126453216782395356136&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-119251703-null-null.142^v68^control,201^v4^add_ask,213^v2^t3_esquery_v1&utm_term=%E8%BF%AD%E4%BB%A3%E5%99%A8&spm=1018.2226.3001.4187

```
容器都有成员begin和end，其中begin成员复制返回指向第一个元素的迭代器，end指向容器尾部元素的下一个位置的迭代器。

迭代器的定义
```cpp
std::vector<int> ::iterator it;     //it能读写vector<int>的元素
std::vector<int>::const_iterator it;//it只能读vector<int>的元素，不可以修改vector<int>中的元素
```

遍历迭代器内容
```cpp
for(it = vector.begin(); it != vector.end(); ++it)
  cout << *it << endl;

// 逆序迭代
for(std::vector<int>::reverse_iterator it = v.rbegin();it != v.rend();it++)
  cout << *it << endl;
```

样例代码
```cpp
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
int main()
{
	//要使用的vector容器应该位于所有定义容器语句的最后一句，应在1而不是2 
	vector<int> c;      //1
	vector<int> vector;
	                    //2
	vector.push_back(1);//插入尾部 
	vector.push_back(9);
	vector.push_back(5); 
	
	sort(vector.begin(),vector.end());
	for( int i=0; i<vector.size(); i++ )
		cout<<"sort_result: "<<vector[i]<<endl;
	
	cout<<"头部元素为："<<vector.front()<<endl;//头部元素 
	cout<<"尾部元素为："<<vector.back()<<endl;//尾部元素
	cout<<"容器尺寸大小为："<<vector.size()<<endl;//容器尺寸大小
	 
	vector.front()=11;//修改容器头部元素值 
	vector.back()= 15;//修改容器尾部元素值
	
	cout<<"修改后头部元素为："<<vector.front()<<endl;//头部元素
	vector.pop_back();//删除尾部元素
	 
	cout<<"修改+删除后尾部元素为："<<vector.back()<<endl;//尾部元素
	vector.push_back(16);
	
	for( int i=0; i<vector.size(); i++ )
		cout<<"用数组输出vector["<<i<<"]："<<vector[i]<<endl;
	
	std::vector<int>::const_iterator it;
	for( it = vector.begin(); it != vector.end(); it++ )
		cout<<"用迭代器输出："<<*it<<endl;
	
	vector.insert(vector.begin(),100);//插入开始位置 
	for( int i=0; i<vector.size(); i++ )
		cout<<"insert_result:"<<vector[i]<<endl;
	cout<<"头部元素为："<<vector.front()<<endl;
	
	return 0;
}

```
#### 使用数组初始化vector对象
**使用数组来初始化vector对象只用指明拷贝区的首尾元素地址**
**vector对象不能用来初始化数组**
```cpp
#include<iostream>
#include<vector>
#include<iterator>
int main()
{
  int arrp[]={1,2,3,4,6};
  vector<int> arr(begin(arrp), end(arrp));
  for(auto a : arr)
    cout << a << endl;
}
```
> begin()在这里返回数组的地址 ？// 函数重载 //
上述vector容器中的元素和数组的元素一致

## 数组

下述是需要注意的地方

- for循环处理多维数组
```cpp
#include <iostream>
int main(int argc, char **argv)
{
	int arr[5][2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 2; j++)
			arr[i][j] = 10; // OK

	for (auto &i : arr)
		for (auto &j : i)
			j = 10; // OK

	for (auto &i : arr)
		for (auto j : i)
			j = 10; // OK？ // 但无法改变数组内的值

  for (auto i : arr)
		for (auto j : i)
			j = 10; // err // 编译器报错


}
```
> 范围for循环 ：前的变量不带引用时仅仅是形式参数
> 并且 ：后的对象必须是一个序列

## try语句块 异常处理表达式
异常是程序在执行期间产生的问题。C++ 异常是指在程序运行时发生的特殊情况，比如尝试除以零的操作。

异常提供了一种转移程序控制权的方式。C++ 异常处理涉及到三个关键字：try、catch、throw。

- throw: 当问题出现时，程序会抛出一个异常。这是通过使用 throw 关键字来完成的。
- catch: 在您想要处理问题的地方，通过异常处理程序捕获异常。catch 关键字用于捕获异常。
- try: try 块中的代码标识将被激活的特定异常。它后面通常跟着一个或多个 catch 块。

如果有一个快抛出一个异常，捕获异常的方法会使用try和catch关键字，try块中放置可能抛出异常的代码，try中的代码称为保护代码

- 使用try·catch语句的语法如下：
```cpp
try{
  //protect code 
}catch(exceptionName e1)
{
  //catch code
}catch(exceptionName e2)
{
  //catch code
}catch(exceptionName e3 )
{
  //catch code
}
```
如果try块在不同的情境下会抛出不同的异常，可以罗列多个catch语句，用于捕获不同类型的异常
在保护代码中的函数调用中可以 使用throw语句 抛出异常


- 抛出异常throw语句
  使用 throw 语句在代码块中的任何地方抛出异常。throw 语句的操作数可以是任意的表达式，表达式的结果的类型决定了抛出的异常的类型。
  - throw 抛出的异常会通过函数栈 找到最近的符合的异常处理程序 
    若无则会交给系统处理 强行打断程序

- catch括号内为(`类型`) 
```cpp
#include <iostream>
using namespace std;
 
double division(int a, int b)
{
   if( b == 0 )
   {
      throw "Division by zero condition!";
   }
   return (a/b);
}
 
int main ()
{
   int x = 50;
   int y = 0;
   double z = 0;
 
   try {
     z = division(x, y);
     cout << z << endl;
   }catch (const char* msg) {
     cerr << msg << endl;
   }
 
   return 0;
}
```


| 异常                  | 描述                                                                      |
| --------------------- | ------------------------------------------------------------------------- |
| std::exception        | 该异常是所有标准 C++ 异常的父类。                                         |
| std::bad_alloc        | 该异常可以通过 new 抛出。                                                 |
| std::bad_cast         | 该异常可以通过 dynamic_cast 抛出。                                        |
| std::bad_typeid       | 该异常可以通过 typeid 抛出。                                              |
| std::bad_exception    | 这在处理 C++ 程序中无法预期的异常时非常有用。                             |
| std::logic_error      | 理论上可以通过读取代码来检测到的异常。                                    |
| std::domain_error     | 当使用了一个无效的数学域时，会抛出该异常。                                |
| std::invalid_argument | 当使用了无效的参数时，会抛出该异常。                                      |
| std::length_error     | 当创建了太长的 std::string 时，会抛出该异常。                             |
| std::out_of_range     | 该异常可以通过方法抛出，例如 std::vector 和 std::bitset<>::operator[]()。 |
| std::runtime_error    | 理论上不可以通过读取代码来检测到的异常。                                  |
| std::overflow_error   | 当发生数学上溢时，会抛出该异常。                                          |
| std::range_error      | 当尝试存储超出范围的值时，会抛出该异常。                                  |
| std::underflow_error  | 当发生数学下溢时，会抛出该异常。                                          |

## class struct
http://c.biancheng.net/view/2235.html
C++中保留了C中的struct关键字，并加以扩充。
> 在C中 struct只能包含成员变量，不能包含成员函数
> 在Cpp中 struct类似class,既可以包含成员变量，又可以包含成员函数。

Cpp中的struct和class基本是通用的，唯有几个细节不同：
- 使用class时，类中的成员默认都是private属性的；而使用struct时，结构体中的成员默认都是public属性的。
- **class继承默认是private继承，而struct默认是punlic继承**
- class可以使用模板，而struct不能

**在编写C++代码时，我强烈建议使用 class 来定义类，而使用 struct 来定义结构体，这样做语义更加明确。**

### 类中的const
```cpp
#include <iostream>
#include <string>
class MyClassMate
{
public:
	std::string getname() const { return name; }
	std::string setname(std::string names)
	{
		name = names;
		return name;
	}
	float gethigh() const;
	float sethigh(float highs);

private:
	std::string name;
	float high;
};
float MyClassMate::gethigh() const
{
	using namespace std;
	return MyClassMate::high;
}
float MyClassMate::sethigh(float highs)
{
	using namespace std;
	MyClassMate::high = highs;
	return highs;
}
int main(int argc, char **argv)
{
	using namespace std;
	class MyClassMate a;
	string name("wang");
	a.setname(name);
	a.sethigh(1.8);
	cout << a.getname() << endl;
	cout << a.gethigh() << endl;
}
```
`const`意在不改变class中`private`成员
并且 **const对象只能调用const函数**
在类之外定义的常量函数也要带上`const`关键字 

同时 **定义的函数是否为常量函数也同时影响函数重载**
```cpp
class a {
public:
  int fun() const{;} //1
  int fun(){;}       //2
}
```
当 const对象调用时会调用1 非const对象调用时会调用2

1)const成员函数可以访问非const对象的非const数据成员、const数据成员，也可以访问const对象内的所有数据成员；
2)非const成员函数可以访问非const对象的非const数据成员、const数据成员，但不可以访问const对象的任意数据成员；
3)作为一种良好的编程风格，在声明一个成员函数时，若该成员函数并不对数据成员进行修改操作，应尽可能将该成员函数声明为const 成员函数。


**作为cpp的类&对象中 最重要的是所谓的OOP的思想**

### 友元
类的友元定义在类的外部,但有权访问类的私有(private)成员和保护(protected)成员。
尽管友元在类的定义中出现了，但友元并不是成员。

友元可以是一个函数，成为友元函数。友元同时也可以是一个类，称为友元类，友元类的所以成员都是该类的友元。

- 如果要声明函数成为一个类的友元，需要在类定义中该函数原型前使用关键字friend：

```cpp

#include <iostream>
using namespace std;
class Box
{
	double width;

public:
	friend void printWidth(Box box);
	void setWidth(double width);
};

void Box::setWidth(double wid){
	this->width = wid;
}

void printWidth(Box box){
	cout << "width is " << box.width << endl;
	// 因为是类的友元，所以可以直接访问类的private成员
}

int main(int argc, char* argv[]){
	Box box;
	box.setWidth(10.0);
	printWidth(box);
	return 0;
}
```


### 类的构造函数
- 类的构造是类的一种特殊成员函数，他会在每次创建类的新对象时进行。

构造函数的名称和类的名称是相同的，并且不会返回任何类型，也不会返回void。构造函数可用于某些成员变量设置初始值。
```cpp
#include<iostream>
using namespace std;

class Line{
public :
  void setLine(int len){len = lenth;};
  double getLine(){return lenth;};
  Line(int len);//构造函数

private: 
  int lenth;
};

Line::Line(int len){
  this->lenth=len;//在初始化时间执行，
}
```
- 列表初始化
```cpp
#include <iostream>
using namespace std;

class Line{
public:
  Line(int len,int wide,int high,int low):length(len),high(high),wide(wide),low(low){
    //codes
    //codes
    //codes
    //codes
  }
private:
  int length,high,wide,low;
};

```
类的构造函数同样支撑函数重载；

```cpp
class Sales_data{
  public:
  Sales_data()=default;
  Sales_data(const std::string &a):X(a){}
  Sales_data(const std::string &a,int b,unsigned c):X(a),Y(b),Z(c){}
  std::string X;
  int Y=0;
  unsigned Z=0;
}

```


### 析构函数
和构造函数类似，析构函数在类被del时进行，
析构函数名和类的名字相同，只是在前加上`～`作为前缀
析构函数有助于跳出程序(关闭文件、释放内存)前释放资源。
```cpp

#include <iostream>
 
using namespace std;
 
class Line
{
   public:
      void setLength( double len ){};
      double getLength( void ){};
      Line(){};   // 这是构造函数声明
      ~Line(){};  // 这是析构函数声明
 
   private:
      double length;
};
 
```
### 类的其他特性

```cpp
class Screen {
public:
  typedef std::string::size_type pos;
  Screen()=default;
  Screen(pos ht,pos wd,char c):height(ht),width(wd),contents(ht*wd,c){};
  char get() const { return contents[cursor];}
  inline char get(pos ht,pos wd)const ;
  Screen &move(pos r,pos e);
private:
  pos cursor = 0;// cursor n.光标
  pos height = 0;
  pos width = 0;
  std::string contents;//contents n.内容
};
```

- 关于内联函数 可以显式定义也可隐式定义 ：定义在函数体内部的函数是隐式内联函数 ，定义在类外部的函数不是「除非显式使用inline」
- `Screen() = default` 因为我们在类中定义了一个构造函数，所以如果要编译器自动生成·默认的构造函数· 则需要「 = default」  


- 可变数据成员「mutable」
  ```cpp
  class Screen {
    public:
      void some_member() const;
    private:
      mutable size_t access_ctr;//即使在一个const对象内也可以修改
  };
  void Screen::some_member()const{
    ++(this->access_ctr);//简单计数器 记录被调用次数
  }
  ```
  一个**可变数据成员**将永远不会之const,即使它是const对象的成员。
  一个const对象依旧可以改变其内部的「mutable」成员

- 返回*this的成员函数
```cpp
class Screen{
  public: 
    Screen &set(char);
    Screen &set(pos,pos,char);
    // 其他成员一样
};
inline Screen & Screen::set(char c)
{
  contents[cursor]=c;
  return *this;
}
inline Screen & Screen::set(pos r,pos col,char ch)
{
  contents[r*width+col]=ch;
  return *this;
}
```
> **和move操作一样，set函数的返回值是引用，这意味着函数的返回是左值的。若函数返回的为非引用，则返回的是调用对象this的副本「临时拷贝」**

- 基于const的重载
```cpp
class Screen {
  public:
    Screen & display(std::ostream &os)
                          {do_display(os);return *this;}
    const Screen & display(std::ostream &os) const 
                          {do_display(os);return *this;}
  private:
    void do_display(std::ostream &os) const
      { os << contents; }
} 
```
我们希望调用display时是const操作 但又希望可以`screen.set('*').display(cout).move(10,10).set('#');`此类的连续访问操作
所以针对const进行重载 并且在private成员中定义了「do_display()」函数进行真正的display。
> 这里的display们都是隐式的inline函数，所以他们并不会拥有所谓的函数调用的开销

- 关于友元 「**我友元的友元 · 不是我的友元**」
  友元类必须在「我」之前声明

- 在构造函数中「列表构造」 编译器将根据类中的变量名称出现的顺序进行初始化而不是根据列表的顺序 
## IO
- CPP IO 类型
  - iostream
    - istream，wistream 从流读入数据
    - ostream，wostream 向流写入数据
    - iostream，wiostream 读写流
  - fstream
    - ifstream，wifstream 从文件读取数据
    - ofstream，wofstream 向文件写入数据
    - fstream，wfstream 读写文件
  - sstream
    - istringstream，wistringstream 从string读取数据
    - ostringstream，wostringstream 从string写入数据
    - stringstream，wstringstream 读写string
  
IO类型因为有继承机制和模板，我们可以忽略不同流之间的差异，均可以通过`>> `进行读取数据

- **IO对象无拷贝或赋值**
  ```cpp
  ofstream out1, out2;
  out1 = out2;          //错误： 不能对流对象赋值
  ofstream print(out1); //错误：不能初始化 ofstream 参数
  out2 = print(out2);   //错误：不能拷贝流对象
  ```
  不能拷贝 不能将形参和返回类型设置为流类型。
  **进行IO操作的函数通常以引用的方式进行传递和返回流**。
  读写一个IO对象会改变其状态，因此传递和返回的引用不能是const的


- 条件状态
  5个状态标志位：
  - strm::iostate --- iostate 是一种机器相关的类型，提供表达条件状态的完整功能
  - strm::badbit --- strm::badbit 用来标识流已经崩溃。
  - strm::failbit --- strm::failbit 用来标识一个IO操作失败
  - strm::eofbit --- strm::eofbit 用来指出流已经到达了文件结束
  - strm::goodbit --- strm::goodbit 用来指出流未处于错误状态，此值保证为0

  查询流状态的接口
  - s.eof()  --- 若s的`eofbit`置位 return True
  - s.fail()  --- 若s的`failbit 或 badbit`置位 return True
  - s.bad() --- 若s的`badbit`置位 return True
  - s.good() --- 若流s处于有效状态 返回true
  - s.clear() --- 将流s中的所有条件状态复位，将流的状态设置为有效 返回void
  - s.clear(flags) --- 将流s的状态按照flag进行复位，flags的类型为strm::iostate 返回void
  - s.setstate(flags) --- 将流s的状态按照flag进行复位，flags的类型为strm::iostate 返回void 
  - s.rdstate() 返回流s当前的状态，返回的类型为strm::iostate

- 缓冲 
  - 刷新缓冲区
    endl操纵符 完成换行并刷新缓冲区的工作。
    flush 刷新缓冲区，不输出任何字符
    ends 向缓冲区插入一个空字符，然后刷新缓冲区
    ```cpp
    cout << "***" << endl;
    cout << "***" << flush;
    cout << "***" << ends; 
    ```
  - unitbuf 操纵符
如果我们想在每次输出操作之后，都刷新缓冲区，我们可以使用 unitbuf 操纵符。它告诉流，在接下里的每次写操作之后，都进行一次flush操作，而 nounitbuf 操纵符可以将流的刷新缓冲机制重置，回到正常的状态。
    ```cpp
    cout << unitbuf;//设置无缓冲
    cout << nounitbuf;//设置有缓冲
    ```
> 警告：如果程序崩溃，输出缓冲区不会被刷新
- 关联流 
  当一个输入流被关联到另一个输出流时，任何试图从输入流读入的操作都会刷新关联的输出流。标准库将`cin`和`cout`关联到一起
  - `tie()` 函数 使输入流绑定到输出流
     有两个重载的版本，
    一个不带参数，返回指向输出流的指针，若无则返回空指针
    另一个`「输入流.tie(输出流)」`进行绑定
    - 每个输入流只能绑定一个输出流，但多个流可以绑定到一个ostream

### 文件IO
文件流： 
ifstream、ofstream、fstream --- 读文件 、写文件 、读写文件

- 创建文件流
  文件流创建方式：
  1. `fstream file(filename)`// 在创建文件流时 进行文件绑定
  2. ```fstream file;file.open(filename);```  
  3. ```fstream file;file.open(filename,mode);```  


**文件使用结束时需要及时关闭**

**当一个fstream对象离开其作用域时，与之关联的文件都会自动关闭**


- 文件模式
  - in(只读)、
  - out(只写)、
  - app(追加/每次写操作均定位到文件末尾)、
  - ate(打开文件时定位到文件末尾)、
  - trunc(截断文件)、
  - binary(以二进制打开)

对文件流的输入输出 和 `cout cin` 类似 使用 「 << 」即可

## 容器
<!-- /////////////////////////////////////////////////////////////////////// -->

<!-- https://zhuanlan.zhihu.com/p/150838490 -->

<!-- /////////////////////////////////////////////////////////////////////// -->
STL具有容器概念和容器类型。概念是具有名称(如容器、序列容器、关联容器)的通用类别；
容器类型是可以用于创建具体对象的模板。**以前的11个容器分别是 dupue、list、queue、priority_queue、stack、vector、map、multimap、set、multiset、bitset**。C++11新增的有**forward_list、unordere_map、unordered_multimap、unordered_set和unordered_multiset，且不将bitset视为容器，而将其视为一种独立的类型。**

 
容器是存储其他对象的对象。被存储的对象必须是同一类型的，可以是OOP的对象，也可以是内置的类型。存储在容器中的对象归容器所有，所以当容器过期时，其内部的数据也将过期。

**容器并不能存储所有对象，具体讲，其存储类型必须是*可以复制构造和可赋值的***。
基本类型满足这些要求；同样，类定义没有将复制构造函数、赋值运算符声明为私有或保护时，也满足。

> 基本容器不能保证其元素都按特定的顺序存储，也不能保证元素的顺序不变，但对概念进行改进后，则可以增加这样的保证。
**复制构造和复制赋值以及移动构造和移动赋值之间的差别在于，复制操作保留源对象，而移动操作可修改源对象，还可能转让所有权，而不做任何复制。如果源对象是临时的，移动操作的效率将高于常规复制。**

### 序列
7种容器(deque、forward_list、list、queue、priority_queue、stack、vector)都是序列。**序列概念添加了迭代器至少是正向迭代器这样的要求**，保证了元素将按照特定顺序排列，不会在两次迭代之间发生变化。
> 序列还要求其元素按严格的线性顺序排列，即存在第一个元素、最后一个元素，除第一个元素和最后一个元素外，每个元素前后都分别有一个元素。数组和链表都是序列，但分支结构(其中每个节点都指向两个子节点)不是。
a[n]和a.at(n)都返回一个指向容器中第n个元素(从0开始编号)的引用。如果n落在容器的有效区间外，则a.at(n)将执行边界检查，并引发out_of_range异常。

#### vector
vector是数组的一种类表示，**它提供了自动内存管理功能**，**可以动态地改变vector对象的长度**，并随着元素的添加和删除而增大和缩小。它提供了对元素的随机访问。**在尾部添加和删除元素的时间是固定的，但在头部或中间插入或删除元素的时间复杂度是线性的。**

vector还是可反转容器(reversible container)概念的模型。这增加了两个类方法：rbegin()，rend()，前者返回一个指向反转序列的第一个元素的迭代器，后者返回反转序列的超尾迭代器。

**vector模板类是最简单的序列类型，除非其他类型的特殊优点能够更好的满足程序的要求，否则应默认使用这种类型。**



#### deque

deque模板类(在deque头文件中声明)表示双端队列(double-ended queue)，通常被简称为deque。**在STL中，其实现类似于vector容器，支持随机访问**。
**主要区别在于，从deque对象的开始位置插入和删除元素的时间是固定的，而不像vector中那样是线性时间的**。
所以，如果多数操作发生在序列的起始和结尾处，则应考虑使用deque数据结构。

为实现deque两端执行插入和删除操作的时间为固定的这一目的，deque对象的设计比vector对象更为复杂。因此，尽管二者都对元素的随机访问和在序列中部执行线性时间的插入和删除操作，但vector容器执行这些操作是速度要快些。


#### list

list模板类(在list头文件中声明)表示双向链表。除第一个元素和最后一个元素外，每个元素都与前后的元素相连接，这意味着可以双向遍历链表。**list和vector之间关键的区别在于，list在链表中任一位置进行插入和删除的时间都是固定的**(`vector强调提供了除结尾外的线性时间的插入和删除，在结尾处，它提供了固定时间的插入和删除`)。

> 因此，vector强调的是通过随机访问进行快速访问，而list强调的是元素的快速插入和删除。

- 与vector相似，list也是可反转容器。
- 与vector不同的是，list不支持数组表示法和随机访问。
- 与矢量迭代器不同，从容器中插入和删除元素之后，链表迭代器指向元素将不变。

void merge(list<T,Alloc>& x)——将链表x与调用链表合并。两个链表必须已经排序。合并后的经过排序的链表保存在调用链表中，x为空。这个函数的复杂度为线性时间。
void remove(const T & val)——从链表中删除val的所有实例。这个函数的复杂度为线性时间。
void sort()——使用< 运算符对链表进行排序；N个元素的复杂度为NlogN
void splice(iterator pos,list<T,Alloc>x)——将链表x的内容插入到pos的前面，x将为空，这个函数的复杂度为固定时间
void unique()——将连续相同的元素压缩为单个元素。这个函数的复杂度为线性时间。


insert()和splice()之间的主要区别在于：insert()将原始区间的副本插入到目标地址，而splice()则将原始区间移动到目标地址。splice()方法执行后，迭代器仍然有效，也就是说，如果将迭代器设置为指向one中的元素，则在splice()将它重新定位到元素three后，该迭代器仍然指向相同的元素。
注意，unique()只能将相邻的相同值压缩为单个值。
非成员sort()函数需要随机访问迭代器，又因为快速插入的代价是放弃随机访问功能，所以不能讲非成员函数sort()用于链表。

#### forward_list（C++11）

C++11新增了容器类forward_list，它实现了单链表。在这种链表中，每个节点都只链接到下一个节点，而没有链接到前一个节点。因此forward_list只需要正向迭代器，而不需要双向迭代器。因此，不同于vector和list，forward_list是不可反转容器。相比于list，forward_list更简单、更紧凑，但功能也更少。

#### queue

queue模板类（在头文件queue（以前为queue.h）中声明）是一个适配器类。queue模板让底层类（默认为deque）展示典型的队列接口。

queue模板的限制比deque更多。它不仅不允许随机访问队列元素，甚至不允许遍历队列。它把使用限制在定义队列的基本操作上，可以将元素添加到队尾、从队首删除元素、查看队首和队尾的值、检查元素数目和测试队列是否为空。

#### priority_queue

priority_queue模板类（在queue头文件中声明）是另一个适配器类，它支持的操作与queue相同。两者之间的主要区别在于，在priority_queue中，最大的元素被移到队首，内部区别在于，默认的底层类是vector。可以修改用于确定哪个元素放到队首的比较方式，方法是提供一个可选的构造函数参数。

#### stack

stack（在头文件stack（以前是stack.h）中声明）也是一个适配器类，它给底层类（默认情况下为vector）提供了典型的栈接口。

stack模板的限制比vector更多。它不仅不允许随机访问栈元素，甚至不允许遍历栈。它把使用限制在定义栈的基本操作上，即可以将压入推到栈顶、从栈顶弹出元素、查看栈顶的值、检查元素数目和测试栈是否为空。

#### array（C++11）

并非STL容器，因为其长度是固定的。因此，array没有定义调整容器大小的操作，如push_back()和insert（），但定义了对它来说有意义的成员函数，如operator[]（）和at（）。可将很多标准STL算法用于array对象，如copy（）和for_each（）。

--- 
## 智能指针 
`shared_ptr` 用来只能释放内存
c++11 条款21：尽量使用std::make_unique和std::make_shared而不直接使用new
`make_shared<T>(args)/make_unique(args)`进行初始化 
> 细节是
> 内部将直接使用args初始化new出的类型T 并给new出来的内存块分配智能指针  
```cpp
template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
```
---

explicit 关键字 修饰类的构造函数？ 阻止编译器在构造时进行隐式类型转换

---
## 类的继承机制
- 公有继承相当于将(基类的公有成员和保护成员)分别给予派生类的(公有成员和保护成员权限)

- 保护继承相当于将(基类的公有成员和保护成员)给予派生类的(保护成员权限)

- 私有继承相当于将(基类的公有成员和保护成员)给予派生类的(私有成员权限)

同时 **所有类的私有成员对派生类均不可见**
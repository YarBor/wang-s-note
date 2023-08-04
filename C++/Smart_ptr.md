# 智能指针

### 为什么不用裸指针
- 裸指针在声明中并没有指出，裸指针指涉到的是单个对象还是一个数组。
- 裸指针在声明中也没有提示在使用完指涉的对象以后，是否需要析构它。换言之，你从声明中看不出来指针是否拥有(own)其指涉的对象。
- 即使知道需要析构指针所指涉的对象，也不可能知道如何析构才是适当的。是应该使用delete运算符呢，还是别有它途（例如，可能需要把指针传入一个专门的、用千析构的函数）
- 即使知道了应该使用delete运算符，参见理由1'还是会发生到底应该用单个对象形式("`delete`")还是数组形式(`delete[]`)的疑问。一旦用错，就会导致未定义行为。
- 没有什么正规的方式能检测出指针是否空悬(dangle)，也就是说，它指涉的内存是否已经不再持有指针本应该指涉的对象。如果一个对象已经被析构了，而某些指针仍然指涉到它，就会产生空悬指针。

## c++ 中的智能指针
C++ll 中共有四种智能指针： 
- `std::auto_ptr` (弃用)
- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr `
所有这些智能指针都是为管理动态分配对象的生命期而设计的，
换言之，通过保证这样的对象在适当的时机以适当的方式析构（包括发生异常的场合），
来防止资源泄漏。


### 专属所有权 unique_ptr
std::unique_ptr实现的是专属所有权语义。一个非空的std::unique_ptr总是拥有其所指涉到的资源。移动一个std::unique_ptr会将所有权从源指针移至目标指针（源指针被置空）。
std::unique_ptr不允许复制，因为如果复制了一个std::unique_ptr,就会得到两个指涉到同一资源的std::unique_ptr,而这两者都认为且已拥有（因此应当析构）该资源。因而，std::unique_ptr是个只移型别。在执行析构操作时，由非空的std::unique_ptr析构其资源。默认地，资源的析构是通过对std::unique_ptr内部的裸指针实施delete完成的。

1. unique_ptr 是可以自定义析构函数的
```cpp
      /// Destructor, invokes the deleter if the stored pointer is not null.
      ~unique_ptr() noexcept
      {
	static_assert(__is_invocable<deleter_type&, pointer>::value,
		      "unique_ptr's deleter must be invocable with a pointer");
	auto& __ptr = _M_t._M_ptr();
	if (__ptr != nullptr)
	  get_deleter()(std::move(__ptr)); 
      /* 这里比较奇怪(看起来)  
      实际上是get_deleter()返回构造时传入的可调用对象 
      然后对可调用对象进行调用 
      这里的__ptr的类型是auto将由编译时编译器自行推断  */
	__ptr = pointer();
      }
```
> 在这个析构函数中，首先使用static_assert进行断言检查，确保deleter_type类型的对象可以通过指针调用。如果无法调用，则会触发编译错误。  
接下来，通过引用_M_t._M_ptr()获取存储的指针，并将其赋值给__ptr变量。然后检查__ptr是否为nullptr，即指针是否为空。  
如果__ptr不为空，则调用get_deleter()函数获取deleter对象，并将__ptr作为参数传递给它。这里使用std::move(__ptr)将__ptr转移所有权，以确保在调用完毕后__ptr被置空。  
最后，将__ptr赋值为pointer()，即将其置为空指针。


- 如果由 makelnvestment 创建的对象不应被直接删除，而是应该先写入一条日志，那么 makelnvestment 可以像下面这样实现
  
![image.png](https://s2.loli.net/2023/08/03/UP6e2hWFTGtdH8S.png)
这里的传入的是一个可调用对象 lambda OR class() OR func() 

在使用默认析构器（即 delete 运算符）的前提下，std::unique_ptr 和裸指针尺寸相同。自定义析构器现身以后，情况便有所不同了。若析构器是函数指针，那么 std::unique_ptr 的尺寸 一 般会增加 一 到两个字长 (word) 。而若析构器是函数对象，则带来的尺寸变化取决千该函数对象中存储了多少状态。无状态的函数对象（例如，无捕获的 lambda 表达式）不会浪费任何存储尺寸。

### 共享所有权 shared_ptr
通过`std::shared_ptr`这种智能指针访问的对象采用共享所有权来管理其生存期。没有哪个特定的`std::shared_ptr`拥有该对象。取而代之的是，所有指涉到它的`std::shared_ptr`共同协作，确保在不再需要该对象的时刻将其析构。当最后一个指涉到某对象的`std::shared_ptr`不再指涉到它时（例如，由于是该`std::shared_ptr`被析构，或使其指涉到另一个不同的对象），该`std::shared_ptr`会析构其指涉到的对象。

std::shared_ptr 的构造函数会使该计数递增（通常如此），而其析构函数会使该计数递减，而复制赋值运算符同时执行两种操作;

引用计数的存在会带来 一 些性能影响：
1. 内存 内存是裸指针的两倍
2. 引用技术需要动态分配
3. 计数的递增和递减必须是原子操作 (慢于非原子操作 即使计数只有一个字节)

#### 析构 
和 `unique_ptr` 相似 `shared_ptr` 同样支持自定义析构器 
**但支持的方式和`unique_ptr`并不相同** 
对于 `unique_ptr` 而言，析构器的型别是智能指针型别的一部分。

![image.png](https://s2.loli.net/2023/08/03/9exa2fbpmCl6R34.png)

---
#### 内存
![image.png](https://s2.loli.net/2023/08/03/q9HwXsfEmMiuYlc.png)

关于 shared_ptr 上述内存何时创建 
1. std::make_shared 总是创建一个控制块.
2. 从具备专属所有权的指针（即std::unique_ptr或std::auto_ptr指针）出发构造一个std::shared_ptr时，会创建—个控制块 。
   > 构造后 专属所有权的指针 的所有权转移 其被置空
3. 当 std::shared_ptr 构造函数使用裸指针作为实参来调用时
   > 使用裸指针创建时要注意 以免两个sharePtr指向同一内存
   进行错误析构 进而野指针

#### 注意
1. **在类内不要使用this去构造shared_ptr**
![image.png](https://s2.loli.net/2023/08/03/pJ1nqQuyWrsUiTI.png)
使用 
`class M : public std::enable_shared_from_this<M>` 进行继承操作
并使用 成员函数 shared_from_this() 获取此对象的 shared_ptr 
![image.png](https://s2.loli.net/2023/08/03/SIZ1wrKxPmhQJWu.png)
如上所示 
上述 仅仅作为演示 

---
![image.png](https://s2.loli.net/2023/08/03/unD9TvX5QaiMkrI.png)
2. **关于 `weak_ptr`**
  - 关于 weak_ptr 的使用场景 
  - 校验指针是否悬空 
  - 弱回调 
  - 缓存及其观察者
  - 避免shared_ptr循环引用

---


















3. **非必要 则使用std::make_ptr**
   优先选用 std::make_unique 和 std::make_shared, 而非直接使用 new

   ```cpp
    void processWidget(std::shared_ptr<Widget> sqw,int priority){
      // .....
    }
    int computerPriority(){
      // ....
      return 1;
    }

    int func(){
      processWidget(std::make_shared< Widget>( new Widget),
      computerPriority()); // 潜在的内存(资源)泄漏
    }

   ```
   原因与编译器从源代码到目标代码的翻译有关。在运行期，传递给函数的实参必须在函数调用被发起之前完成评估求值。下列事件必须在 processWidget 开始执行前发生：
    - 表达式 “new Widget" 必须先完成评估求值，即， 一个 Widget 对象必须先在堆上创建。
    - 由 new 产生的裸指针的托管对象 std::shared_ptr<Widget> 的构造函数必须执行。
    - computePriority 必须运行。

    编译器不必按上述顺序来生成代码。
    其有可能生成

    1. 实施 “ new Widget" 。
    2. 执行 computePriority 。
    3. 运行 std::shared_ptr 构造函数。
    
    如果生成了这样的代码，并且在运行期 computePriority 产生了异常，那么由第一步动态分配的Widget会被泄漏，因为它将永远不会被存储到在第三步才接管的std::shared_ptr中去。

    使用 std::make_shared 可以避免该问题。
  - BUT
    - 相对于直接使用 new 表达式，优先选用 make 函数  但是所有的make函数 不能自定 析构器 需要自定析构器的智能指针 还是得 `std::shared_ptr<T>(new T, ...)` 
    - 关于列表构造 
    `std::make_shared<std::vector<int>> (10,20)`
    `std::make_shared<std::vector<int>> {10,20}`
    其构造的 是 [10,20] 还是 [20,20,20,20,20...]
        答案是
        1. `std::make_shared<std::vector<int>> {10,20}` 编译不通过
        2. `std::make_shared<std::vector<int>> (10,20)` 构造的是[20,20,20,20,20...]

        为了使用列表构造+make函数 
        我们可以
        ```cpp
        auto i = {10,20};
        std::make_shared<std::vector<int>> (i);
        ```
    

4. std::shared_ptr 的 API 仅被设计用来处理指涉到单个对象的指针 但unique_ptr 可以是数组
5. 关于 shared_ptr 是 make 构造 还是 new 构造时 关于 weak_ptr 的一些差别
    ```cpp
    #include <iostream>
    #include <memory>

    class Resource {
    public:
        Resource() {
            std::cout << "Resource acquired." << std::endl;
        }
        ~Resource() {
            std::cout << "Resource released." << std::endl;
        }
        int value = 10;
    };
    int main() {
        std::weak_ptr<Resource> weakPtr;
        Resource * i = nullptr;
        {
            auto sharedPtr = std::make_shared<Resource>();
            weakPtr = sharedPtr;
            i = sharedPtr.get();
            if (auto lockedPtr = weakPtr.lock()) {
                std::cout << "Resource is still available." << std::endl;
            } else {
                std::cout << "Resource has been released." << std::endl;
            }
        }

        std::cout << i->value << std::endl;

        if (auto lockedPtr = weakPtr.lock()) {
            std::cout << "Resource is still available." << std::endl;
        } else {
            std::cout << "Resource has been released." << std::endl;
        }

        std::cout << i->value << std::endl;

        return 0;
    }
    ```
    ```cpp
    #include <iostream>
    #include <memory>

    class Resource {
    public:
        Resource() {
            std::cout << "Resource acquired." << std::endl;
        }
        ~Resource() {
            std::cout << "Resource released." << std::endl;
        }
        int value = 10;
    };
    int main() {
        std::weak_ptr<Resource> weakPtr;

        Resource * i = nullptr;
        {
            std::shared_ptr<Resource> sharedPtr (new Resource);
            weakPtr = sharedPtr;
            i = sharedPtr.get();
            if (auto lockedPtr = weakPtr.lock()) {
                std::cout << "Resource is still available." << std::endl;
            } else {
                std::cout << "Resource has been released." << std::endl;
            }
        }

        std::cout << i->value << std::endl;

        if (auto lockedPtr = weakPtr.lock()) {
            std::cout << "Resource is still available." << std::endl;
        } else {
            std::cout << "Resource has been released." << std::endl;
        }

        std::cout << i->value << std::endl;

        return 0;
    }
    ```


# 右值引用、移动语义和完美转发

<!-- 
## 左值 右值
在C++中，值类别用于描述表达式的类型和属性。下面是对不同值类别的解释：

1. 泛左值（glvalue）：泛左值是一种广义的左值，可以是左值或将亡值。它们具有身份，并且可以被引用。例如，变量、函数、静态成员、数组元素等都属于泛左值。

2. 右值（rvalue）：右值是指那些不具有身份的临时对象、将亡值或其子对象，以及不关联对象的值。右值可以出现在赋值表达式的右边，但不能出现在赋值表达式的左边。右值在表达式结束后就不再存在。

3. 左值（lvalue）：左值是指那些具有身份且持久存在的对象，可以被引用。左值可以出现在赋值表达式的左边，也可以出现在右边。函数或对象的名称、变量、非静态成员等都属于左值。

4. 将亡值（xvalue）：将亡值是指接近声明周期末尾的对象，即将要灭亡的对象。将亡值是某些涉及右值引用的表达式的结果，可以被移动。例如，通过`std::move`转换的对象就是将亡值。

5. 纯右值（prvalue）：纯右值是指非将亡值的右值，它们不具有身份，并且可以被移动。纯右值是一种临时对象，例如字面量、临时表达式的结果等。

这些值类别在C++中用于确定表达式的属性和可用操作。例如，只有右值引用可以绑定到右值或将亡值，而左值引用只能绑定到左值。




关系如下
![](https://pic1.zhimg.com/80/v2-51c8bf9b50bcbb9c70a0d1154dc47ac7_720w.webp?source=1940ef5c)

这些概念或许让你感到眼花，没关系，让我们总结一下左值和右值的属性

- 左值
  - 拥有身份(identity) : 可以拿到内存 安全的使用
  - 不可被移动
  - 常见的左值 :已命名的变量或常量、返回引用的函数等
- 右值(包含将亡值,纯右值) 
  - 不拥有身份
  - 可以移动
  - 常见的右值
    - 纯右值 : 运算表达式产生的临时变量、不和对象关联的原始字面量、非引用返回的临时变量、Lambda表达式等
    - 将亡值：将要被移动的对象、T&&函数的返回值、std::move函数的返回值、转换为T&&类型转换函数的返回值等


1. 泛左值（glvalue）：
```cpp
int x = 5; // 变量x是一个泛左值
int& ref = x; // 引用ref是x的别名，也是一个泛左值
```

2. 右值（rvalue）：
```cpp
int getResult() {
    return 10; // 返回一个右值
}

int&& rref = getResult(); // 右值引用绑定到返回的右值
```

3. 左值（lvalue）：
```cpp
int x = 5; // 变量x是一个左值
int* ptr = &x; // 指针ptr指向x，x是一个左值
```

4. 将亡值（xvalue）：
```cpp
std::vector<int> getVector() {
    std::vector<int> v = {1, 2, 3};
    return std::move(v); // 返回一个将亡值
}

std::vector<int>&& rvref = getVector(); // 将亡值引用绑定到返回的将亡值
```

5. 纯右值（prvalue）：
```cpp
int getResult() {
    return 10; // 返回一个纯右值
}

int value = getResult(); // 纯右值赋值给变量value
``` -->

 

---

```cpp
int & func(int && i )
{ 
  return i;  
}
int main(){
  int i1 = 0;
  
  int& i2 = func(std::move(i1));

  cout << i1 << endl;
  cout << i2 << endl;
  
  i2 = 2;

  cout << i1 << endl;
  cout << i2 << endl;
}

Q1 : 将会输出什么
Q2 : 为啥子
```
--- 


- **移动语义** 使得编译器 **得以使用不那么昂贵的移动操作，来替换昂贵的复制操作**。同复制构造函数、复制赋值运算符给予人们控制对象复制的具体意义的能力一样，移动构造函数和移动赋值运算符也给予人们控制对象移动语义的能力。移动语义也使得创建**只移型别对象**成为可能，这些型别包括std::unique_ptr、std::future和std::thread等。
- **完美转发** 使得人们可以撰写接受任意实参的函数模板，并将其转发到其他由数，目标函数会接受到与转发函数所接受的完全相同的实参。
- **右值引用** 右值引用是将这两个风马牛不相及的语言特性胶合起来的底层语言机制，正是它使得移动语义和完美转发成为了可能。

## 首先
- 需要知道 std::move 和 std::forward 不做什么
  出乎我们意料 std::move并不进行任何移动，std::forward也不进行任何转发。
  这两者在运行期都无所作为。它们不会生成任何可执行代码，连一个字节都不会生成。

std::move 做的仅仅是 **进行强制类型转换**  
无条件将任意类型转换为右值
```cpp
  /**
   *  @brief  Convert a value to an rvalue.
   *  @param  __t  A thing of arbitrary type.
   *  @return The parameter cast to an rvalue-reference to allow moving it.
  */
  template<typename _Tp>
    _GLIBCXX_NODISCARD
    constexpr typename std::remove_reference<_Tp>::type&&
    move(_Tp&& __t) noexcept
    { return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }

```
上面是标准库 
> 注：参数T &&t并不是代表右值引用，而是转发引用（也叫万能引用）。转发引用既能是右值引用，也可以是左值引用，这与类型推导和引用折叠有关。如果用右值初始化转发引用，那么得到的是右值引用。如果用左值初始化转发引用，那么得到的是左值引用 (通过引用折叠) 

右值引用使得移动语义、完美转发这两个毫不相干的特性产生了关联，从而确保C++可以在不影响已有代码的基础上提升代码性能。左值引用传参可以很好解决传值时带来的额外拷贝开销。而对于右值引用的情况和需要我们转移对象的情况，为了优化性能，C++11允许我们借助移动构造将资源“窃取”走，这在C++11中叫做移动语义。它避免了浅拷贝可能引发的悬空指针的问题，也避免了深拷贝昂贵的开销

---

而 std:: forward 则仅在某个特定条件满足时才执行同一个强制转换。

```cpp
template<typename T>
T&& forward(typename std::remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}
​
template<typename T>
T&& forward(typename std::remove_reference<T>::type&& t)
{
    return static_cast<T&&>(t);
}


```
上面是std::forward的简化版实现，可以看到C++实现了两个模板函数，一个用于接受左值，一个接受右值。它是如何做到有条件地强转的呢？答案还是引用折叠。

```cpp
template<typename T>
T&& forward(typename std::remove_reference<T>::type& t)
{
    return static_cast<T&&>(t);
}
​```
```cpp
假如 T 在编译时被推导成 int&

那么就是

int& && forward(typename std::remove_reference<int& >::type& t)
{
    return static_cast<int& &&>(t);
}
通过折叠 

int& forward(typename std::remove_reference<int& >::type& t)
{
    return static_cast<int&>(t);
}

同理 
假如 T 在编译时被推导成 int&&

那么就是

int&& && forward(typename std::remove_reference<int&& >::type& t)
{
    return static_cast<int&& &&>(t);
}
通过折叠 

int&& forward(typename std::remove_reference<int&& >::type& t)
{
    return static_cast<int&&>(t);
}

```
ok **完美**转发

下面验证一下
```cpp

void foo(int& x) { std::cout << "Lvalue: " << x << std::endl; }

void foo(int&& x) { std::cout << "Rvalue: " << x << std::endl; }

使用 std::forward 进行完美转发的模板函数
template <typename T>
void bar(T&& x) {
  foo(std::forward<T>(x));
}

int main() {
  int a = 42;
  // 传递左值
  bar(a);
  // 传递右值
  bar(123);
  return 0;
} 

```
```bash
Lvalue: 42
Rvalue: 123
```
Done @!




### 引用折叠

引用折叠是“引用的引用”。等等，引用的引用？C++中引用的引用难道不是非法的吗？是的，引用的引用是非法的，但实际上编译器在默认情况下允许了这种存在，即引用折叠的情况。引用折叠的作用是将双重引用折叠为单个引用（这并不违背引用的引用是非法的）。

解释一下折叠的含义。所谓的折叠，就是多个的意思。上面介绍引用分为左值引用和右值引用两种，那么将这两种类型进行排列组合，就有四种情况：
- 左值-左值 T& &
- 左值-右值 T& &&
- 右值-左值 T&& &
- 右值-右值 T&& &&

BUT

// ...
int a = 0;
int &ra = a;
int & &rra = ra;  // 编译器报错：不允许使用引用的引用！
// ...

原因就是：引用折叠的应用场景不在这里！！


利用boost库 进行打印 type 和 类型 
```cpp

#include <iostream>
#include <boost/type_index.hpp>

using namespace std;
using boost::typeindex::type_id_with_cvr;  

template<typename T>
void PrintType(T&& param)
{
    // 利用Boost库打印模板推导出来的 T 类型
	cout << "T type：" << type_id_with_cvr<T>().pretty_name() << endl; 
    
    // 利用Boost库打印形参的类型
	cout << "param type:" << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}

int main(int argc, char *argv[])
{
	int a = 0;                              // 左值
	PrintType(a);                           // 传入左值

	int &lvalue_refence_a = a;              // 左值引用
	PrintType(lvalue_refence_a);            // 传入左值引用

	PrintType(int(2));                      // 传入右值
}
```
输出是 
```bash
T type：int&
param type:int&
T type：int&
param type:int&
T type：int
param type:int&&
```

重点来了！编译器将T推导为 int& 类型。当我们用 int& 替换掉 T 后，得到 int & &&。
<!-- MD，编译器不允许我们自己把代码写成int& &&，它自己却这么干了 。 -->
那么 int & &&到底是个什么东西 
编译器在编译期 会对 模板参数进行引用折叠 

引用
所有的引用折叠最终都代表一个引用，要么是左值引用，要么是右值引用。
规则就是：
**如果任一引用为左值引用，则结果为左值引用。否则（即两个都是右值引用），结果为右值引用。**

通过引用折叠 便出现了万能引用 


```cpp

#include <iostream>
using namespace std;

// 万能引用，转发接收到的参数 param
template<typename T>
void PrintType(T&& param)
{
	f(param);  // 将参数param转发给函数 void f()
}

// 接收左值的函数 f()
template<typename T>
void f(T &)
{
	cout << "f(T &)" << endl;
}

// 接收右值的函数f()
template<typename T>
void f(T &&)
{
	cout << "f(T &&)" << endl;
}

int main(int argc, char *argv[])
{
	int a = 0;
	PrintType(a);//传入左值
	PrintType(int(0));//传入右值
}

```


我们执行上面的代码，按照预想，在main中我们给 PrintType 分别传入一个左值和一个右值。PrintType将参数转发给 f() 函数。f()有两个重载，分别接收左值和右值。
正常的情况下,PrintType(a);应该打印f(T&),PrintType(int());应该打印f(T&&)。
但是 
```bash
f(T &);
f(T &);
```

传入了不同类型的值，但是void f()函数只调用了void f(int &)的版本。这说明，不管我们传入的参数类型是什么，在void PrintType(T&& param)函数的内部，param都是一个左值引用！

**引用都是左值的**
当外部传入参数给 PrintType 函数时，param既可以被初始化为左值引用，也可以被初始化为右值引用，取决于我们传递给 PrintType 函数的实参类型。但是，当我们在函数 PrintType 内部，将param传递给另一个函数的时候，此时，param是被当作左值进行传递的。

如何 将 传入的参数 原封不动的传出去?
如此便 std::forward 
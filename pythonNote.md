# pythonLearningNotes
## py--type 
- 文本类型(字符串):(**Text**)
  -  str
- 数字类型:(**Numeric**) 
  - int, float, complex(复数)
- 序列类型:(**Sequence**)
  -  list,tuple(元组), range
    - list 
      - list是一种有序的集合
    - tuple
      - 元组 和列表一样,是一种有序对像的集合,大部分和列表一样,不同之处在于元组元素是不可改变的(但元组和列表一样是可以添加元素的)
    - range
      - range是一种序列类型，range类型用于表示不可变的整数序列序列类型（包括列表等）可以使用运算符in和not in检查 
        -  Python 3 中range( ) 函数返回的是一个range 对象（可迭代对象）。 
        -  Python 3 中range( ) 函数常常要配合list( ) 函数或者for 循环语句使用。
- 映射类型:(**Mapping**)
  - dict  <!-- #dictionary#字典#类似散列表 -->
- 设置类型:(**set**)
  - set
  - frozenset
- 布尔类型:(**boolean**)
  - bool
- 二进制类型:(**Binary**)
  - bytes 
  - bytearray
  - memoryview
- 空类型:(**None**)  
  - Nonetype  

关于get任意对象数据类型可以用 `type()`函数

在py中初始化数据类型时一般是自动赋值的 不过也可以手动
```py
x = "wang"  # x = str(wang)
y = int(5)  # y = 5
z = range(10)   # 这个只能使用range()函数
a = complex(1j) #  a = 1j
```
以上初始化的方式都是可行的

### Number
**在py中** 
#### 数字类型
- **int** 
  - 是整数类型 可以是正数负数 无小数部分 长度不受限
- **flaot**
  - 有理数 正数或负数 包含一个或多个小数位 
  - 浮点数是可以带有'e'的科学数字 表示10的幂
- **complex**
  - 虚数包含字符'j'
  - `3+5j`表示 实部为3 虚部为5
  - `complex`类型不能被转换
- **random**(no)
  - 随机数 
    - py没有`random()`来生成随机数的函数 
    - 但py有一个内置的模块来生成随机数 
    - ```py
        import random
        print(random.randrange(1,10)) 
      ```
#### Casting/类型转换(译: 铸造)
**在py中** 类型转换是使用构造函数完成的
通过这三个 [ **int ( ) / float ( ) / str ( )** ] 函数转换

### string/字符串
#### 基本操作
1. "wang" == 'wang'
2. ` a = "wang" ` 可将字符串赋给变量
3. 多行字符串
    ```py
     a = """wang
     wang
     wang
     wang"""
     # 单引号也行
     ```
4. 字符串本质上是数组(和C一样)
5. 循环遍历字符串
   ```py
    for x in "banana":
        fun(x) #在此 x 将本别为'b''a''n''a''n''a'
   ```
6. 字符串长度 使用`len()`函数
    `len(string)`将返回`string`的长度
7. 检查字符串 `in` / `not in'`关键字
   ```py
    txt = "The best things in life are free!"
    if "free" in txt:   #在字符串中
        #code...
    if "free" not in txt: # 不在字符串中
        #code...
   ```
#### 字符串切片(slicing strings)
```py
#!bin/python3
wang = "wang's string"
print(wang[1:3])#output "an"
print(wang[:10])#output "wang's stri"
print(wang[6:])#output " string"
```
 [ : ]内的引索从0开始 不包括':'后的元素
#### 修改字符串/创造修改后的字符串
str类的内置函数 //不会修改原本的字符串
```py
常用的
a = " Wang's Daily Notes   "
b = a.upper() # 将字符串以大写字母返回
c = a.lower() # 将字符串以小写字母返回
d = a.strip() # 去除字符串开头和结尾的空字符
e = a.replace("s",'G') # 将字符串中的字符's'替换为'G'
f = a. split("\'") # 将字符串以字符' \' '分割 返回list结构
```
#### String Connatenation
```py
a = "wang"
b = "wangs"
c = a + b # c == "wangwangs"
# b += a //可以
d = a + "wwwww " + b # d == "wangwwwww wangs
```
#### string format()/格式化输出字符串
format()函数允许格式化字符串的选定部分
> 文本的某些部分或许是无法控制的或许来自用户的输入?  
运用`{}`来控制此类 在文本中添加占位符{} 并通过format()函数运行这些值
```py
price = 49 
string = "The price is {} "
print(string.format(price)) # output "The price is 49"

string = "The price is {:.2f} "# 化为两位小数的数字

string = "The price is {}/{}/{} "
print(string.format(10,100,1000))# 按位置输出

string = "The price is {a}/{b}/{c} "
print(string.format(b=10,a=100,c=1000))# 命名引所输出

string = "The price is {0}/{0}/{1} "
print(string.format(10,"wang",1000))# 按引索号输出 引所括号内的变量

```
#### 转义字符
**不同的**:> 
- `\ooo` 八进制输出
- `\xhh` 十六进制输出
- `\f`   换页

#### strings-Methods
**https://www.jianshu.com/p/7bc9e608aba3**
- capitalize() // 无参 // convert the first character to upper case
- casefold() // 无参 // Converts string into lower case
- center() // 变量.center(20) // Returns a centered string
  ```py
  txt = "banana"
  x = txt.center(20)
  print(x) # "       banana"
  ```
- count() // 有参 // return the number of sprcified value occurs in string
  ```py
  txt = "I love apples, apple are my favorite fruit"
  x = txt.count("apple") # x == 2
  print(x) # 2
  ```
- encode() // 无参 //return an encode version of the string
  
- endswith() //return true if the string ends with the special value  

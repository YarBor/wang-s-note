## 检索数据 

### SELECT

检索单个列 
```sql
select prod_name from products;
```
检索 `products` 的单个 prod_name 列中的 所有数据 
数据没有过滤和排列

检索多个列
```sql
select prod_id prod_name 
from products ; 
```
检索 `products` 的 prod_id prod_name 列中的 所有数据 
数据没有过滤和排列

可以使用完全限定的名字 
```sql
select products.prod_name
from crashcourse.products;
```

## 排序检索数据

```sql
select nameid_
from tables_ 
order by nameid_ ;
```
```sql
select prod_id,prod_price,prod_name
from products
order by prod_price , prod_id;
```
在选择多个列时，一定要在列名之间加上逗号


```sql
select nameid_
from tables_
order by nameid_ desc ; //降序
```

```sql
select prod_id,prod_price,prod_name
from products
order by prod_price desc, prod_id ;
```

## 过滤数据 
### 避免重复 distinct
```sql
select distinct vend_id
from products;
```
### 限制结果 limit
```sql
select * from products limit 5 ;
```
限制结果返回5条内 

```sql
select * from products limit 5,5;
```
返回从第五条开始的5条数据

> 可以使用完全限定的表名 类似 products.prod_name

## 过滤数据 
### where 字句
> where字句出现的位置在from字句之后  用于指定特殊的搜索条件进行过滤

```sql
select prod_name,prod_price 
from products
where prod_price = 2.50;
```
上面的语句 返回两列 拥有name 和 price的信息 并且 price == 2.50

### where字句操作符 
和c语言类似 
不同的是 
1. 相等 `=` 
2. 不等 `!=` 或 `<>`
3. `between __ and ___`

### where 查 空「NULL」
```sql
select prod_name,prod_price
from products 
#1 where prod_price is null ;
#2 where prod_price is not null;
```

### 组合where字句  
- where 字句中的 and or 操作符
```sql
select prod_name,prod_price 
from products 
#1 where prod_price is not null AND prod_name = "tea" ; 
#2 where prod_price <= 100 OR prod_price > 1000;
```

> 符合筛选条件可以用`()`圈起来 

```sql
select prod_name , prod_price
from products 
where (vend_id = 1003 or vend_id = 1002) and (price between 100 and 200);

```

### where 中的 in 操作符

在SQL中，`IN`操作符用于筛选在一个给定列表中的值。可以将其用于`SELECT`语句中的`WHERE`子句来过滤数据。

下面是`IN`操作符的语法：

```sql
SELECT column_name(s)
FROM table_name
WHERE column_name IN (value1, value2, ...);
```

在这个语法中，`IN`后面跟着一对括号，括号内包含要匹配的值的列表，这些值之间用逗号隔开。如果查询结果中的某一列包含了列表中的任何一个值，那么该行就会被返回。

例如，假设我们有一个名为`customers`的表，其中有名字（`name`）和地址（`address`）两列数据。要查找名字为Alice、Bob、Cathy和David的客户的地址，可以使用以下语句：

```sql
SELECT address
FROM customers
WHERE name IN ('Alice', 'Bob', 'Cathy', 'David');
```

此时，只有满足条件的记录（名字为Alice、Bob、Cathy和David）的地址数据会被返回，其他记录的地址数据不会被返回。

### 通配符进行过滤 
在SQL中，`LIKE`操作符是用来搜索与指定模式匹配的数据的。它通常用于SELECT语句的WHERE子句中。

下面是`LIKE`操作符的一般语法：

```sql
SELECT column1, column2, ...
FROM table_name
WHERE columnN LIKE pattern;
```

其中，`columnN`是要搜索的列名，`pattern`是一个包含通配符的字符串，表示要搜索的模式。

在SQL中，有两种通配符可以用于`LIKE`操作符：

- 百分号（%）：代表任意字符串（包括空字符串）
- 下划线（_）：代表任意单个字符

例如，如果要查找`customers`表中所有地址以“Street”结尾的客户，可以使用以下`SELECT`语句：

```sql
SELECT *
FROM customers
WHERE address LIKE '%Street';
```

此时，`%`代表任意长度的字符串，因此这条SELECT语句会返回所有地址以“Street”结尾的客户记录。

再例如，如果要查找`orders`表中所有订单ID的第三、四位是“12”的订单，可以使用以下SELECT语句：

```sql
SELECT *
FROM orders
WHERE order_id LIKE '__12%';
```

此时，`__`代表任意两个字符，因此这条SELECT语句会返回所有订单ID的第三、四位是“12”的订单记录。

总之，`LIKE`操作符和通配符可以帮助我们更灵活地执行数据库查询，提高查询的准确性和效率。


## 创建计算字段 
### concat()函数 实现拼接字段
```sql
SELECT CONCAT(prod_id, '|', prod_name) AS prod_data 
FROM productions 
WHERE prod_id IN (1003,1004,1005) OR prod_name = "wang";

```

> MySQL的不同之处多数DBMS使用 + 或 || 来实现拼接，MySQL则使用 Concat() 函数来实现。当把SQL语句转换成MySQL语句时一定要把这个区别铭记在心

### 执行算数计算 
```sql
select prod_id, prod_name , prod_number*prod_price as total_price
from productions
where prod_id IN (1003,1004,1005) ;
``` 

## 数据处理函数 
### 文本处理函数
| 函数| 描述| 参数|
| -- | -- | -- |
| left()   | 返回一个串中左边指定长度的字符| 原始字符串和要返回的字符数|
| right()  | 返回一个串中右边指定长度的字符| 原始字符串和要返回的字符数|
| length() | 返回一个串的长度| 原始字符串|
| locate() | 在一个串中查找另一个字符串第一次出现的位置| 要查找的字符串和被搜索的字符串|
| lower()  | 将一个串转换为小写形式| 原始字符串  |
| ltrim()  | 去除一个字符串左侧的空格| 原始字符串|
| rtrim()  | 去除一个字符串右侧的空格| 原始字符串|
| soundex()| 计算一个字符串的SOUNDEX值，用于比较发音相似单词的编码系统    | 原始字符串|
| substring()| 返回一个字符串的子串  | 原始字符串、子串的起始位置和子串的长度|
| upper()  | 将一个串转换为大写形式  | 原始字符串|

### 时间和日期处理函数

| 函数名 | 描述 | 参数 |
| --- | --- | --- |
| ADDTIME() | 增加一个时间(时、分等) | 原始时间，增加的时间 |
| HOUR() | 返回一个时间的小时部分 | 时间 |
| MINUTE() | 返回一个时间的分钟部分 | 时间 |
| SECOND() | 返回一个时间的秒部分 | 时间 |
| TIME() | 返回一个时间日期的时间部分 | 时间日期 |
| ADDDATE() | 增加一个日期(天、周等) | 原始日期，增加的值，单位 |
| DAY() | 返回一个日期的天数部分 | 日期 |
| MONTH() | 返回一个日期的月份 | 日期 |
| YEAR() | 返回一个日期的年份部分 | 日期 |
| DATE() | 返回日期时间的日期部分 | 日期时间 |
| DAYOFWEEK() | 对一个日期返回对应的星期几 | 日期 |
| DATEDIFF() | 计算两个日期只差 | 第一个日期，第二个日期，单位 |
| DATE_ADD() | 高度灵活的日期运算函数 | 日期，间隔时间，间隔时间的类型 |
| DATE_FORMAT() | 返回一个格式化的日期或时间串 | 日期时间和格式字符串 |
| CURDATE() | 返回当天的日期 | 无参数 |
| CURTIME() | 返回当前的时间 | 无参数 |
| NOW() | 返回当前的日期和时间 | 无参数 |


日期的格式 :> `YYYY-MM-DD HH:MI:SS`

#### ADDTIME()

```sql
SELECT ADDTIME('12:30:00', '02:15:45') AS 'New Time';
```
结果将会是 `'14:45:45'`

####  HOUR()

```sql
SELECT HOUR('09:45:30') AS 'Hour Part';
```
结果将会是 `'9'`

#### MINUTE()

```sql
SELECT MINUTE('09:45:30') AS 'Minute Part';
```
结果将会是 `'45'`

#### SECOND()

```sql
SELECT SECOND('09:45:30') AS 'Second Part';
```
结果将会是 `'30'`

#### TIME()

```sql
SELECT TIME('2022-04-25 14:30:22') AS 'Time Part';
```
结果将会是 `'14:30:22'`

#### ADDDATE()

```sql
SELECT ADDDATE('2022-01-01', INTERVAL 5 DAY) AS 'New Date';
```
结果将会是 `'2022-01-06'`

#### DAY()

```sql
SELECT DAY('2022-05-30') AS 'Day Part';
```
结果将会是 `'30'`

#### MONTH()

```sql
SELECT MONTH('2022-05-30') AS 'Month Part';
```
结果将会是 `'5'`

#### YEAR()

```sql
SELECT YEAR('2022-05-30') AS 'Year Part';
```
结果将会是 `'2022'`

#### DATE()

```sql
SELECT DATE('2022-05-30 14:30:22') AS 'Date Part';
```
结果将会是 `'2022-05-30'`

#### DAYOFWEEK()

```sql
SELECT DAYOFWEEK('2022-05-30') AS 'Weekday';
```
结果将会是 `'2'`，即星期一

#### DATEDIFF()

```sql
SELECT DATEDIFF('2022-05-30', '2022-05-01') AS 'Date Difference';
```
结果将会是 `'29'`

#### DATE_ADD()

```sql
SELECT DATE_ADD('2022-01-01', INTERVAL 2 MONTH) AS 'New Date';
```
结果将会是 `'2022-03-01'`

#### DATE_FORMAT()

```sql
SELECT DATE_FORMAT('2022-05-30 14:30:22', '%M %d, %Y') AS 'Formatted Date';
```
结果将会是 `'May 30, 2022'`

#### CURDATE()

```sql
SELECT CURDATE() AS 'Current Date';
```
结果将会是当前日期的格式，如 `'2022-06-08'`

#### CURTIME()

```sql
SELECT CURTIME() AS 'Current Time';
```
结果将会是当前时间的格式，如 `'14:47:20'`

#### NOW()

```sql
SELECT NOW() AS 'Current Date and Time';
```
结果将会是当前日期和时间的格式，如 `'2022-06-08 14:47:20'`



### 汇总数据
以下是SQL的聚合函数列表，其中第一列为函数名，第二列为功能，第三列为参数（如果有）：

| 函数名   | 功能                                                         | 参数            |
| -------- | ------------------------------------------------------------ | --------------- |
| COUNT    | 统计指定列中非空值的数量                                     | 列名或表达式    |
| SUM      | 计算指定列中所有数字的总和                                   | 数字列的列名    |
| AVG      | 计算指定列中所有数字的平均值                                 | 数字列的列名    |
| MAX      | 计算指定列中所有数字的最大值                                 | 数字列的列名    |
| MIN      | 计算指定列中所有数字的最小值                                 | 数字列的列名    |
| GROUP BY | 将表中的数据按照指定列进行分组，并对每个分组应用聚集函数，以根据特定条件对数据进行分类和汇总 | 分组的列名或表达式 |



以下是 SQL 聚合函数的一些示例：

**COUNT 函数示例**

 
```sql
SELECT COUNT(City) AS NumberOfCustomers
FROM customers;
```

这将返回以下结果：

```
NumberOfCustomers
-----------------
              91
```

**SUM 函数示例**

以下 SQL 查询语句将返回一个名为 "orders" 的表中 "Price" 列的总和：

```sql
SELECT SUM(Price) AS TotalPrice
FROM orders;
```

这将返回以下结果：

```
TotalPrice
----------
    785.50
```

**AVG 函数示例**

以下 SQL 查询语句将返回一个名为 "products" 的表中 "Price" 列的平均值：

```sql
SELECT AVG(Price) AS AveragePrice
FROM products;
```

这将返回以下结果：

```
AveragePrice
------------
   14.520000
```

**MAX 函数示例**

以下 SQL 查询语句将返回一个名为 "orders" 的表中 "Price" 列中的最大值：

```sql
SELECT MAX(Price) AS HighestPrice
FROM orders;
```

这将返回以下结果：

```
HighestPrice
------------
       35.99
```

**MIN 函数示例**

以下 SQL 查询语句将返回一个名为 "orders" 的表中 "Price" 列中的最小值：

```sql
SELECT MIN(Price) AS LowestPrice
FROM orders;
```

这将返回以下结果：

```
LowestPrice
-----------
      10.99
```

**GROUP BY 函数示例**

以下 SQL 查询语句将返回一个名为 "orders" 的表中按照 "CustomerID" 列进行分组的订单数：

```sql
SELECT CustomerID, COUNT(*) AS OrderCount
FROM orders
GROUP BY CustomerID;
```

这将返回以下结果：

```
CustomerID | OrderCount
-----------|-----------
          1|         2
          2|         1
          3|         3
```


## sql中引号的使用 
在SQL中引号的使用与数据类型有关。对于字符串类型的数据，必须在引用它们时使用引号。通常情况下，SQL允许使用两种类型的引号：单引号（'）和双引号（"）。在大多数情况下，单引号是更常见的。

以下是一些关于在SQL中引号使用的准则：

1. 字符串值必须被单引号或双引号包含。在大多数情况下，使用单引号。例如：

```sql
SELECT * FROM users WHERE name = 'John';
```

2. 在写SQL语句时，如果字符串中包含引号，则需要使用另一种引号将其括起来，或者使用转义字符来表示引号。例如：

```sql
SELECT * FROM products WHERE description = 'This product\'s color is blue.';
```

3. 如果您使用的是双引号，则必须将列名、表名和其他对象名称用双引号括起来。例如：

```sql
SELECT "id", "name" FROM "users";
```

4. 对于数值类型的数据，不需要使用引号。例如：

```sql
SELECT * FROM products WHERE price > 10.00;
```




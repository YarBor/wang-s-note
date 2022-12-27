https://wiki.ubuntu.org.cn/%E8%B7%9F%E6%88%91%E4%B8%80%E8%B5%B7%E5%86%99Makefile:%E6%A6%82%E8%BF%B0

- example
  
```makefile
edit : main.o kdb.o command.o display.o  insert.o search.o file.o 
    cc -o edit main.o kdb.o command.o display.o  insert.o search.o file.o

main.o : main.c head.h
    cc -c main.c
kdb.o : kdb.c head.h
    cc -c kdb.c
command.o : command.c head.h command.h
    cc -c command.c
display.o : display.c buffer.h head.h
    cc -c display.c
file.o : file.c head.h command.h
    cc -c file.c
search.o : search.c head.h command.h
    cc -c search.c
clean : 
    rm edit main.o kbd.o command.o display.o insert.o search.o file.o
# 执行 「make clean」命令 将上述文件删除  
```

# 简述:
```makefile
target ... : prerequisites ...
    command
    ...
    ...
```

target 可以是一个目标文件 
`:` 后的是生成这个文件的依赖 
command 是生成target文件需要的命令

> 在定义好依赖关系后，**后续的那一行定义了如何生成目标文件的操作系统命令**，**一定要以一个tab键作为开头**。
> 记住，make并不管命令是怎么工作的，他只管执行所定义的命令。
> make会比较targets文件和prerequisites文件的修改日期，如果prerequisites文件的日期要比targets文件的日期要新，或者target不存在的话，那么，make就会执行后续定义的命令。

---

默认的方式下，也就是我们只输入make命令。那么，
make会在当前目录下找名字叫“Makefile”或“makefile”的文件。
1. 如果找到，它会找文件中的第一个目标文件（target），在上面的例子中，他会找到“edit”这个文件，并把这个文件作为最终的目标文件。
2. 如果edit文件不存在，或是edit所依赖的后面的 .o 文件的文件修改时间要比edit这个文件新，那么，他就会执行后面所定义的命令来生成edit这个文件。
3. 如果edit所依赖的.o文件也不存在，那么make会在当前文件中找目标为.o文件的依赖性，如果找到则再根据那一个规则生成.o文件。（这有点像一个堆栈的过程）
4. 当然，你的C文件和H文件是存在的啦，于是make会生成 .o 文件，然后再用 .o 文件生成make的终极任务，也就是执行文件edit了。
5. 这就是整个make的依赖性，make会一层又一层地去找文件的依赖关系，直到最终编译出第一个目标文件。在找寻的过程中，如果出现错误，比如最后被依赖的文件找不到，那么make就会直接退出，并报错，而对于所定义的命令的错误，或是编译不成功，make根本不理。make只管文件的依赖性，即，如果在我找了依赖关系之后，冒号后面的文件还是不在，那么对不起，我就不工作啦。

通过上述分析，我们知道，像clean这种，没有被第一个目标文件直接或间接关联，那么它后面所定义的命令将不会被自动执行，不过，我们可以显式要make执行。即命令——“make clean”，以此来清除所有的目标文件，以便重编译。

--- 

makefile 中的变量
> 类似bash
在开头的例子中 有很长的文件名

为规避冗长的文件名 我们可以这么写

```makefile
object = main.o kdb.o command.o display.o  insert.o search.o file.o 
cc = gcc

edit : $(object)
    cc -o edit $(object)
# ***.o : ****.c ***.h
#   cc -c ***.c

# ***.o : ****.c ***.h
#   cc -c ***.c

# ***.o : ****.c ***.h
#   cc -c ***.c

clean : 
    rm edit $(object)
```

---

makefile 中的include

makefile中可以包含其他makefile文件
```makefile
bar = a.mk b.mk
include hhh.make *.mk $(bar)
```
makefile 中的include支持shell的文件模式 · `*.mk`的模糊搜索 · `$()`的变量引用

- makefile寻找include
make命令开始时，会找寻include所指出的其它Makefile，并把其内容安置在当前的位置。就好像C/C++的#include指令一样。如果文件都没有指定绝对路径或是相对路径的话，make会在当前目录下首先寻找，如果当前目录下没有找到，那么，make还会在下面的几个目录下找：

1. 如果make执行时，有“-I”或“--include-dir”参数，那么make就会在这个参数所指定的目录下去寻找。
2. 如果目录<prefix>;/include（一般是：/usr/local/bin或/usr/include）存在的话，make也会去找。
   
如果有文件没有找到的话，make会生成一条警告信息，但不会马上出现致命错误。它会继续载入其它的文件，一旦完成makefile的读取， make会再重试这些没有找到，或是不能读取的文件，如果还是不行，make才会出现一条致命信息。如果你想让make不理那些无法读取的文件，而继续执行，你可以在include前加一个减号“-”。
```makefile
-include hhh.mk
```

---

make的工作方式

第一阶段
1. 读入所有的makefile
2. 读入include中的其他makefile
3. 初始化变量名
4. 推导隐晦规则，分析所有规则
5. 为所有的目标文件创建关系链

第二阶段
6. 根据依赖关系，决定哪些目标要重新生成
7. 执行生成命令

---
# make规则
- examples:
  ```makefile
  target : prerequisites 
      commands
      ...
      ...
  ```
  ```makefile
  target : prerequisites ; command
      commands
      ...
      ...
  ```
- make的第一个目标 将作为整个make的最终目标
- target 是文件名，以空格分开，可以使用通配符。一般来说，目标是单一文件。
- command 是命令行，如果和`依赖`不再一行 **必须要以「Tab」键开头**，如果在一行时 二者用`；`进行分割

命令太长时 可以使用`\`进行换行操作「为更好的阅读代码」

一般来说，make会以UNIX的标准Shell，也就是/bin/sh来执行命令。

---

- 通配符：make支持3个通配符：“～” “ * ” “ ？” ，这里的意义和bash是相同的
  ```makefile
  object = *.o 
    # object == "*.o"
        # 这里的 * 将在需要展开时间展开
  clean : 
    rm -f *.o   # delete all .o files in this dir 
  ```
  也支持bash中的变量
  ```makefile
  print: *.c
	lpr -p $?
	touch print
  #「 $? 」最后运行的命令的结束代码（返回值）
  ```
---
文件搜寻

在大的工程中，会有大量的源文件，我们通常的做法是将其放在不同的目录中。所以，当make进行寻找文件依赖时，可以在文件前加上路径，但最好把路径告诉make让make自己进行寻找

1. 引入变量 `VPATH`
  VPATH 变量 就是完成这个功能的， 没有这个变量，make只会在当前目录进行寻找。定义了这个变量后，make就会在当前目录找不到文件的情况下，到vpath中进行寻找
  ```makefile
  VPATH = ../headers:../../gh
  ```
  上文指定了两个目录`../headers`和`../../gh`
  make 会按照指定的顺序进行搜索。目录以`:`进行分割（当前目录的优先级永远最高）
2. vpath 「关键字」
   注意 不是变量
   ```makefile
   vpath %.h ../headers
   ```
   该语句表示 要求make在`../header`目录下进行搜索所有`.h`文件
   ```makefile
   vpath %.c foo
   vpath %.c hello
   vpath %.c hhh：www
   ```
   该语句表           示.c文件先在foo目录 后在hello目录 然后在hhh目录 最后在www目录
   vpath同样支持`：`分割路径

---

伪目标
```makefile
clean : 
    rm -rf *.o 
```
这就是一个伪目标 仅仅当`make clean`时 make才会执行clean 
但同时 为避免伪目标和文件重名 我们使用一个特殊的标记进行显式的声明伪目标 ` .PHONY ` 
```makefile
.PHONY : clean
clean : 
    rm -rf *.o temp
```
同时 伪目标同样可以成为依赖
```makefile
.PHONY : cleanall cleano cleandiff

cleanall : cleano cleandiff
    rm program
cleano : 
    rm *.o 
cleandiff : 
    rm *.diff
```
> 有点子程序的意思

---

关于静态模式进行定义多目标的规则:

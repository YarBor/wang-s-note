>文件IO（第4章、第5章、第13章）
文件系统（第14章、第15章、第18章）
进程控制（第6章、第24章到第27章、第34章到第37章）
信号（第20章、第21章、第22章）
定时器与休眠（第23章）
线程（第29章到第33章）
IO多路复用（重点学epoll）
进程间通信
网络编程
********************************************************
    2,3                  --略读

    4,5.2-5.5,15,18  --重点
********************************************************
## 系统文件IO
所有执行I/O操作的系统调用都以文件描述符,来指代打开的文件.针对每个进程文件描述符都自成一套  
大多数程序都期望使用3种文件描述符  
文件描述符|用途|POSIX名称|stdio流
   -----|--- |  ---   |----
   1|标准输入|STDIN_FILENO|stdin
   2|标准输出|STDOUT_FILENO|stdout
   3|标准错误|STDERR_FILENO|stderr

在程序中指代这些文件描述符时,可以使用数字(012)表示,或采用<unistd.h>定义的标准名称  
### **执行文件 I/O 操作的 4 个主要系统调用**
> 编程语言和软件包通常会利用 I/O 函数库对它们进行间接调用  
#### ***fd = open(pathname,flags,mode);***  
函数打开pathname所标识的文件,并返回文件描述符  
用以在后续函数中指代打开的文件.如果文件不存在，open()函数可以创建之，这取决于对位掩码参数 flags 的设置。flags 参数还可指定文件的打开方式：只读、只写亦或是读写方式。mode 参数则指定了由 open()调用创建文件的访问权限，如果 open()函数并未创建文件，那么可以忽略或省略 mode 参数  
flags参数表示打开文件所采用的操作，我们需要注意的是：必须指定以下三个常量的一种，且只允许指定一个

O_RDONLY：只读模式
O_WRONLY：只写模式
O_RDWR：可读可写
以下的常量是选用的，这些选项是用来和上面的必选项进行按位或起来作为flags参数。

* **O_APPEND 表示追加**，如果原来文件里面有内容，则这次写入会写在文件的最末尾。
* **O_CREAT** **表示如果指定文件不存在，则创建这个文件**
> S_IRWXU   00700 用户（文件所有者）有读、写和执行权限
S_IRUSR   00400 用户有读权限
S_IWUSR 00200  用户有写权限
S_IXUSR   00100 用户有执行权限
S_IRWXG   00070 组有读、写和执行允许
S_IRGRP 00040  组有读权限
S_IWGRP   00020 组有写权限
S_IXGRP   00010 组有执行权限
S_IRWXO   00007 其他人已读、写、执行允许
S_IROTH   00004 其他人有读权限
S_IWOTH   00002 其他人有写权限
S_IXOTH   00001 其他人有执行权限
* **O_EXCL 表示如果要创建的文件已存在，则出错，同时返回 -1，并且修改 errno 的值**。
* **O_TRUNC 表示截断，如果文件存在，并且以只写、读写方式打开，则将其长度截断为0**。
* **O_NOCTTY 如果路径名指向终端设备，不要把这个设备用作控制终端**。
* **O_NONBLOCK 如果路径名指向 FIFO/块文件/字符文件，则把文件的打开和后继 I/O设置为非阻塞模式**（nonblocking mode）  
**以下三个常量同样是选用的，它们用于同步输入输出**  

* **O_DSYNC 等待物理 I/O 结束后再 write。在不影响读取新写入的数据的前提下，不等待文件属性更新。**
* **O_RSYNC read 等待所有写入同一区域的写操作完成后再进行**
* **O_SYNC 等待物理 I/O 结束后再 write，包括更新文件属性的 I/O**
3、mode:
mode参数表示设置文件访问权限的初始值，和用户掩码umask有关，比如0644表示-rw-r–r–，也可以用S_IRUSR、S_IWUSR等宏定义按位或起来表示，详见open(2)的Man Page。要注意的是，有以下几点

文件权限由open的mode参数和当前进程的umask掩码共同决定。
第三个参数是在第二个参数中有O_CREAT时才作用，如果没有，则第三个参数可以忽略

#### ***numread = read(fd,buffer,count)***  
调用从 fd 所指代的打开文件中读取至多 count 字节的数据，并存储到 buffer 中。read()调用的返回值为实际读取到的字节数。如果再无字节可读（例如：读到文件结尾符 EOF 时），则返回值为 0  
#### ***numwritten = write(fd,buffer,count)***  
调用buffer中count字节的数据写入由fd所指向的以打开文件中.write()调用的返回值为实际写入文件的字节数,可能小于count.  
####  ***status = close(fd)***  
在文件所有操作完成后,调用close()释放文件描述符fd以及与之相关的核心资源  
```c
使用I/O系统调用
#include<sys/stat.h>
#include<fcntl.h>
#include"tlpi_hdr.h" 

#ifndef BUF_SIZE
#define BUF_SIZE 1024 
#endif 
int main(int argc,char ** argv)
{
    int inputFd,outputFd,openFlags;
    mode_t filePerms;
    ssize_t numRead ;
    char buf[BUF_SIZE];

    if(argc != 3||strcmp(argv[1],"--help")==0)
        usageErr("%s old-file new-file \n ",argv[0]);

    inputFd=open(argv[1],O_RDONLY);

    if(inputFd == -1)
        errExit ("opening file %s ",argv[1]);

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;//创建|只写|截断

    filePerms = S_IRUSR | S_IWUSR |S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    outputFd = open(argv[2],openFlags,filePerms);

    if(optputFd == -1)  errExit("opening file %s" ,argv[2]);

    while ((numRead=read(inputFd,buf,BUF_SIZE))>0 )
        if(write(outputFd,buf,numRead)!=numRead) 
            fatal("couldn't write whole buffer");

    if(numRead == -1) errExit("read");

    if(close(inputFd)==-1)  errExit("close input");

    if(close(outputFd)==-1)  errExit("close output");

    
    exit(EXIT_SUCCESS);
}
``` 
#### open()
* **int open(const char* pathname ,int flags, ... )**   
    * Return file descriptor on success , or -1 on error 

要打开的文件由参数pathname来标识 如果pathname是一符号连接,会对其进行解引用 调用成功open()返回一文件标识符,用于在后续函数调用中指代该文件.  若发生错误,则返回-1, 并将errno设置为相应的错误标志.  
flag为掩码,用于指定文件的访问模式
> 访问模式 (flag) 
> O_RDONLY 只读
> O_WRONLY 只写
> O_RDWR   读写  
#### read()
* ***ssize_t read(int fd, char *buffur,size_t count );***
    * Returns the number of bytes read,0 on EOF,-1 on error.
 
count参数指定最多能读取的字节数.(size_t数据类型属于无符号整数类型)buffer参数提供用来存放输入数据的内存缓冲区地址,缓冲区至少拥有count字节.  
如果 read()调用成功，将返回实际读取的字节数，如果遇到文件结束（EOF）则返回 0，如果出现错误则返回-1。ssize_t 数据类型属于有符号的整数类型，用来存放（读取的）字节数或-1（表示错误）。
当 read()应用于其他文件类型时，比如*管道、FIFO、socket 或者终端*，**在不同环境下也会出现 read()调用读取的字节数小于请求字节数的情况**。例如，默认情况下从终端读取字符，一遇到换行符（\n），read()调用就会结束。在后续章节论及其他类型文件时，会再次针对这些情况进行探讨。
> read()能够从文件中读取任意序列的字节。有些情况下，输入信息可能是文本数据，但在其他情况下，又可能是二进制整数或者二进制形式的 C语言数据结构。read()无从区分这些数据，故而也无法遵从 C 语言对字符串处理的约定，在字符串尾部追加标识字符串结束的空字符。如果输入缓冲区的结尾处需要一个表示终止的空字符，必须显式追加  
> 因此 由于表示字符串终止的空字符需要一个字节的内存，所以缓冲区的大小至少要比预计读取的最大字符串长度多出 1 个字节  

#### write()
* ***ssize_t write(int fd,void\* buffer ,size_t count);***
    * Returns number of bytes written , or -1 on error.  

write()调用的参数含义与 read()调用相类似。*buffer 参数为要写入文件中数据的内存地址*，*count参数为欲从 buffer 写入文件的数据字节数*，**fd 参数为一文件描述符，指代数据要写入的文件**。
如果 write()调用成功，将返回实际写入文件的字节数，该返回值可能小于 count 参数值。这被称为“部分写”。对磁盘文件来说，造成“部分写”的原因可能是由于磁盘已满，或是因为进程资源对文件大小的限制。  

#### close()
close()系统调用关闭一个打开的文件描述符，并将其释放回调用进程，供该进程继续使用。当一进程终止时，将自动关闭其已打开的所有文件描述符
* **显式关闭不再需要的文件描述符往往是良好的编程习惯**，会使代码在后续修改时更具可读性，也更可靠。进而言之，文件描述符属于有限资源，因此文件描述符关闭失败可能会导致一个进程将文件描述符资源消耗殆尽。在编写需要长期运行并处理大量文件的程序时，比如 shell 或者网络服务器软件，需要特别加以关注 
#### lseek()
对于每个打开的文件，系统内核会记录其文件偏移量，有时也将文件偏移量称为读写偏移量或指针。文件偏移量是指执行下一个 read()或 write()操作的文件起始位置，会以相对于文件头部起始点的文件当前位置来表示。文件第一个字节的偏移量为 0。
* ***off_t lseek(inf fd,off_t offser,int wherece)***  
offset 参数指定了一个以字节为单位的数值。（SUSv3 规定 off_t 数据类型为有符号整型数。）whence 参数则表明应参照哪个基点来解释 offset 参数，应为下列其中之一：
> SEEK_SET  将文件偏移量设置为从文件头部起始点开始的 offset 个字节。
SEEK_CUR    相对于当前文件偏移量，将文件偏移量调整 offset 个字节 。 
SEEK_END    将文件偏移量设置为起始于文件尾部的 offset 个字节。也就是说，offset 参数应该从文件最后一个字节之后的下一个字节算起。  


* * lseek()调用成功会返回新的文件偏移量。下面的调用只是获取文件偏移量的当前位置，并没有修改它  
    * `curr = lseek(fd ,0 ,SEEK_CUR)`  

5. 文件空洞  
如果程序的文件偏移量已然跨越了文件结尾，然后再执行 I/O 操作，将会发生什么情况？read()调用将返回 0，表示文件结尾。有点令人惊讶的是，write()函数可以在文件结尾后的任意位置写入数据。  
从文件结尾后到新写入数据间的这段空间被称为文件空洞。从编程角度看，文件空洞中是存在字节的，读取空洞将返回以 0（空字节）填充的缓冲区。  
然而，文件空洞不占用任何磁盘空间。直到后续某个时点，在文件空洞中写入了数据，文件系统才会为之分配磁盘块。文件空洞的主要优势在于，与为实际需要的空字节分配磁盘块相比，稀疏填充的文件会占用较少的磁盘空间。
```c
程序样例 
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc ,char** argv)
{
    size_t len ;
    off_t offset;
    int fd , ap ,j;
    char *buf;
    ssize_t numRead,numWritten;

    if(argc < 3|| strcmp(argv[1],"--help")==0)
        usageErr("%s file{r<length>|R<lenfth>|w<string>|s<offset>}...\n",argv[0]);

    fd = open(argv[1],O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if(fd==-1)  errExit("open");

    for(ap=2;ap<argc;ap++)
    {
        switch(argv[ap][0]){
            case 'r':
            case 'R': 
                len = getLong(&argv[ap][1],GN_ANY_BASE,argv[ap]);
                buf= malloc(len);
                if(buf==NULL) errExit("malloc");
            
            numRead = read(fd,buf,len);
            if(numRead==-1) errExit("read");

            if(numRead==0) {
                printf("%s:end-of-file \n",argv[ap]);
            }
            else{
                printf("%s: ",argv[ap]);
                for(j=0;j<numRead;j++)
                {
                    if(argv[ap][0]=='r')    printf("%c",isprint((unsigned char)buf[j])?buf[j]:'?');
                    else    printf("%02x ",(unsigned int)buf[j]); 
                }
                printf("\n");
            }
            free(buf);
            break;

        case 'w':
            numWritten=write(fd,&argv[ap][1],strlen(&argv[ap][1]));
            if(numWritten == -1)    errExit("write");
            printf("%s:wrote %ld bytes\n",argv[ap],(long)numWritten);
            break;
        case 's':
            offset = getLong(&argv[ap][1],GN_ANY_BASE,argv[ap]);
            if(lseek(fd,offset,SEEK_SET)==-1)  errExit("lseek");
            printf("%s : seek succeeded\n",argv[ap]);
            break;
        default:
            cmdLineErr("Argument must start with [rRws]:%s\n",argv[ap])
        }
    }
    exit(EXIT_SUCCESS) ;
}
```
### $深入探究文件I/O  
#### 原子操作 
将某一系统调用所要完成的各个动作做为不可中断的操作,一次性加以执行.是许多系统调用的一正常执行的必要条件 .  
* 原子操作和竞争条件 
所有的系统调用都是以原子操作执行的,是指内核保证了某系统调用中的所有步骤会作为独立操作而一次性加以执行，其间不会为其他进程或线程所中断  
**特别是他规避了竞争状态** 
竞争状态是指操作共享资源的两个进程或线程 随机获得cpu的使用顺序  

#### 文件控制操作 : ***fcntl()***  
函数原型: 
1. **int fcntl ( int fd, int cmd ) ;**
2. **int fcntl ( int fd , int cmd , long arg ) ;**
3. **int fcntl( int fd , int cmd , struct * lock) ;** 

fcntl()有5种功能 :
*   1. 复制一个现有的描述符(cmd=F_DUPFD)  
*   2. 获得/设置文件描述符标记(cmd=F_GETFD或F_SETFD)
*   3. 获得/设置文件状态标记(cmd=F_GETFL或F_SETFL)
*   4. 获得/异步I/O所有权(cmd=F_GETOWN或F_SETOWN)
*   5. 获得/设置记录锁(cmd=F_GETLK,F_SETLK或F_SETLKW)

描述: **fcntl()针对文件描述符提供控制**,参数fd是被参数cmd操作的描述符;  
针对cmd的值,fcntl能接受第三个参数(arg)  

* int fcntl(int fd,int cmd, ... )  
((#include<fcntl.h>)RETURN on success depends on cmd , or -1 on error)  
cmd参数所支持的操作范围很广 . fcntl()的第三个参数以省略号来表示，这意味着可以将其设置为不同的类型，或者加以省略。内核会依据 cmd 参数（如果有的话）的值来确定该参数的数据类型  
fcntl的用途之一是针对一个打开的文件,获取或修改其访问模式和状态表示,这些值是通过open()函数的flag参数来设置的. 要获取这些设置,应将fcnlt()的cmd参数设置为F_GETFL  
> 针对一个打开的文件，只有通过 open()或后续 fcntl()的 F_SETFL 操作，才能对该文件的状态标志进行设置  
判定文件的访问模式有一点复杂，这是因为 O_RDONLY(0)、O_WRONLY(1)和 O_RDWR(2)这 3 个常量并不与打开文件状态标志中的单个比特位对应。因此，要判定访问模式，需使用掩码 O_ACCMODE 与 flag 相与，将结果与 3 个常量进行比对，
```c
int flag,accessMode ;
flags=fcntl(fd,F_GETFL);
if(flags==-1) errExit("fcntl");
accessMode=flags&O_ACCMODE
if(accessMode==O_WRONLY || accessMode == O_RDWR)
    printf("file is witable\n" );
``` 

内核对所有打开的文件维护都有一个系统级的描述表格 (有时也称为-打开文件表),并将表中的各个条目称为打开文件句柄.**一个打开文件句柄存储了于一个打开文件的所有信息**  
* 当前文件偏移量(调用read()write()时更新或者直接lseek()修改).  
* 打开文件时所用的状态标志(open()的flags参数).
* 文件访问模式(如 open()时设置的只读模式.只写.读写).  
* 于信号驱动I/O相关的设置  
* 对文件i-node对象的引用.  
每个文件系统都会为驻留其上的文件建立一个i-node表 这里只是列出每个文件的i-node信息  
* 文件类型(常规文件,套接字,FIFO)和访问权限 
* 一个指针指向该文件所持有的锁的列表
* 文件的各种属性,包括文件的大小以及不同类型操作的时间戳 ;

两个不同的文件描述符,若指向同一打开文件句柄,将共享同一文件偏移量.因此,若通过其中的

#### 复制一个现存的文件描述符。
dup/dup2函数:  
```c
#include <unistd.h> 
int dup(int oldfd);
int dup2(int oldfd, int newfd);
```
当调用dup函数时 内核在进程中创建一个新的文件描述符,此描述符是当前文件描述符的最小值这个文件描述符指向oldfd拥有的所有的文件表项.  
> dup()和dup2()的区别就是可以用newfd指定新描述符的数值,若newfd已经打开 则先将其关闭(忽略关闭该过程的所有错误)   

> 如果newfd等于oldfd，则dup2返回newfd, 而不关闭它。dup2函数返回的新文件描述符同样与参数oldfd共享同一文件表项

实际上，
dup(oldfd) == fcntl(oldfd, F_DUPFD,0);
dup2(oldfd, newfd) == close(oldfd);fcntl(oldfd, F_DUPFD,newfd);

#### 在文件特定偏移量处IO:pread()/pwrite()  
*并非文件指针所在,而在调用时的输入的偏移量处*
**定义:>** 
```c
#include <unistd.h>
ssize_t pread(int fd,void *buf,size_t count,off_t offset);
ssize_t pwrite(int fd ,void *buf,size_t count,off_t offset);
```
系统调用pread()和pwrite()完成read()和write()类似的操作,  
只是前两者会在offset参数指定的位置进行IO操作,而非始于文章偏移量处,并且它们不会改变文章的偏移量  
pread()函数相当于将如下调用列入同一原子操作:
```c
off_t orig;
orig = lseek(fd,0,SEEK_CUR);//SEEK_CUR -> 指文件当前偏移量
lseek(fd,offset,SEEK_SET);  //SEEK_SET -> 文件头部启始的offset偏移量
s = read(fd ,buf,len);
lseek(fd,orig,SEEK_SET);    //返回最初的偏移量 
```
***多线程应用为这些系统调用提供了用武之地***。正如第 29 章所述，**进程下辖的所有线程将共享同一文件描述符表**。这也意味着每个已打开文件的文件偏移量为所有线程所共享。**当调用pread()或 pwrite()时，多个线程可同时对同一文件描述符执行 I/O 操作，且不会因其他线程修改文件偏移量而受到影响**。如果还试图使用 lseek()和 read()(或 write())来代替 pread()（或pwrite()），那么将引发竞争状态   .

#### 分散输入和集中输入  readv()与writev()函数
readv(),writev()函数系统调用分别实现了分散输入和集中输出的功能.
```c
#include <sys/uio.h>
struct iovec {
       void  *iov_base;    /* Starting address */
       size_t iov_len;      /* Number of bytes to transfer */
};
ssize_t readv(int fd,const struct iovec * iov,int iovcnt);
        // returns the number of bytes read, 0 on EOF, -1 on error
ssize_t writev(int fd,const struct iovec * iov,int iovcnt);
        // Returns the  number of bytes writen ,-1 on error
```
***readv和writev函数用于在一次函数调用中读、写多个非连续缓冲区***。有时也将这两个函数称为散布读（scatter read）和聚集写（gather write）。  
* 应用场景:   以writev为例，在实现http服务器时，返回响应报文需要准备的数据有：响应行、响应头、响应空行、响应体。通常的处理方式为：将前三者放入同一写缓存空间，而响应体可能是客户端请求的一个文件，自然属于单一的缓存空间。对于这种情况，就可以使用writev同时处理两块buffer，且writev是按序处理，可以自然的把响应体放到最后
##### readv()
`ssize_t readv(int fd,const struct iovec * iov,int iovcnt);`
        `// returns the number of bytes read, 0 on EOF, -1 on error`
* 功能: 从文件描述符fd关联的文件中读取数据到iov指向的buffer中  
* 描述: iov标识的buffer是iovec结构体类型的数组,iovcnt表示数组的最大长度,读取时,按数组顺序存储数据,即iov[0],先于iov[1],如果数据量不够存满整个数组,则剩余部分不予存储,  
* readv()函数属于**原子操作**
* 返回读取字节数,读取失败返回-1;
##### writev()
`ssize_t writev(int fd,const struct iovec * iov,int iovcnt);`
        `// Returns the  number of bytes writen ,-1 on error`
* 功能：writev相对于readv就相当于write对于read，把iov指向的数组空间中的内容写入fd关联的文件，同样也是原子操作。  
* 返回写入字节数 ,写入失败返回-1;  
##### 在指定的文章偏移量处致敬分散输入和集中输出
***preadv()*** / ***pwritev()*** ，  
将分散输入/集中输出和于指定文件偏移量处的 I/O 二者集于一身
``` c
#define _BSD_SOURCE
#include <sys/uio.h>

ssize_t preadv(int fd,const struct iovec *iov,int iovcnt ,off_t offset);
            // returns the number of bytes read, 0 on EOF, -1 on error
ssize_t pwritev(int fd,const struct iovec * iov,int iovcnt,off_t offset);
            // returns the number of bytes write ,-1 on error
```
preadv()和 pwritev()系统调用所执行的任务与 readv()和 writev()相同，但执行 I/O 的位置将由 offset 参数指定(类似于 pread()和 pwrite()系统调用)。
***对于那些既想从分散-集中 I/O 中受益，又不愿受制于当前文件偏移量的应用程序（比如，多线程的应用程序）而言，这些系统调用(preadv(),pwritev())恰好可以派上用场***。
##### 总结
 readv和writev，相比于read和write函数只是多了对多个buffer操作的能力。当要将一个连续区域数据写入文件时，使用write()即可，但当要将多个不同区域数据写入文件时，则使用writev()更为方便。
#### 截断文件: truncate()和ftruncate()
truncate()和ftruncate()系统调用将文件大小设置为length参数指定的值  
```c
#include<nuistd.h>
int truncate(const char *pathname,off_t length);
int ftruncate(int fd,off_t length); 
                // both return 0 on success ,or -1 on error
```
若文件当前长度大于length,调用将丢弃超出部分,若小于length,调用将在文件尾部添加一系列空字节或一个文件空洞.  
两个系统调用之间的差别在于如何指定操作文件. 
* turncate()以路径名字符串来指定文件,并要求可访问该文件,且对该文件拥有写权限. 
* fturncate()以问文件描述符以指代该文件,并且在调用之前,须以可写方式打开操作文件,获取文件描述符以指代该文件,该系统调用不会修改文件偏移量.  
#### 非阻塞IO
在打开文件时指定O_NINBLOCK 目的: 
1. 若open()调用未能立即打开文件 ,则返回错误 ,而非陷入阻塞. but 有一种情况例外 即open()操作FIFO可能会陷入阻塞 
2. 调用open()成功后,后续IO操作也是非阻塞的. 若IO系统调用未能立即完成,则可能只传入部分数据,或系统调用失败,并返回EAGAIN或EWOULDBLOCK错误.具体返回何种错误将依赖于系统调用.Linux将两个错误常量视为同义  

管道,FIFO,套接字,设备(终端,伪终端)都支持非阻塞状态。（因为无法通过 open()来获取管道和套接字的文件描述符，所以要启用非阻塞标志，就必须使用 5.3 节所述 fcntl()的F_SETFL 命令。）
> FIFO:http://t.csdn.cn/wBFuZ 

#### 大文件IO
通常来说存放文件偏移量的数据类型off_t实际上是一个有符号的长整形; 在X86-32 ~ 即32位体系架构中,将文件大小限制在2^31-1个字节(2GB)的限制下  
然而,32位的UNIX实现有处理超过2GB大小的需求 **则提出了*LFS*的增强特性**
* 应用程序可使用如下两种方式之一以获得 LFS 功能。
*   1. 使用支持大文件操作的备选 API。该 API 由 LFS 设计，意在作为 SUS 规范的“过渡型扩展”。因此，尽管大部分系统都支持这一 API，但这对于符合 SUSv2 或 SUSv3 规范的系统其实并非必须。这一方法现已过时。
*   2.  在编译应用程序时，将宏_FILE_OFFSET_BITS 的值定义为 64。这一方法更为可取，因为符合 SUS 规范的应用程序无需修改任何源码即可获得 LFS 功能。
##### 过度型LFS API
要使用过渡型LFS API **必须在编译程序时定义_LARGEFILE64_SOURCE 功能测试宏,该定义可以通过命令行指定,也可以定义于源文件中包含所有头文件之前的位置**  
 该 API 所属函数具有处理 64 位文件大小和文件偏移量的能力。这些函数与其 32 位版本命名相同，只是尾部缀以 64 以示区别。其中包括：fopen64()、open64()、lseek64()、truncate64()、stat64()、mmap64()和 setrlimit64()
##### 向printf()传输off_t值 
对于于定义的值,展示其值的方式是将其转化为long类型 使用%ld
但对于 LFS功能的off_t,则需要将其转换为longlong类型 使用%lld 
#### /dev/fd目录
对于每个进程,内核都提供一个特殊的虚拟目录/dev/fd.该目录包含"/dev/fd/n"形式的文件名,其中n是与进程中的打开文件描述符相对应的编号.因此,例如,/dev/fd/0则对应的是该进程的标准输入  
```c
下面两行代码是等价的 
fd=open("/dev/fd/1", O_WRONLY);
fd=dup(1);
```
在为open()设置参数时,注意将其设置为与原描述相同的访问模式 . 这一场景下,在flag标志下设置中引入其他标志诸如O_CREAT,是无意义的.

!!! /dev/fd 实际上是一个符号链接，链接到 Linux 所专有的/proc/self/fd 目录。后者又是 Linux特有的/proc/PID/fd 目录族的特例之一，此目录族中的每一目录都包含有符号链接，与一进程所打开的所有文件相对应。
***
没看太懂

程序中很少会使用/dev/fd 目录中的文件。其主要用途在 shell 中。许多用户级 shell 命令将文件名作为参数，有时需要将命令输出至管道，并将某个参数替换为标准输入或标准输出。
出于这一目的，有些命令（例如，diff、ed、tar 和 comm）提供了一个解决方法，使用“-”符号作为命令的参数之一，用以表示标准输入或输出（视情况而定）。所以，要比较 ls 命令输出的文件名列表与之前生成的文件名列表，命令就可以写成：
`$ ls | diff -oldfilelist`
这种方法有不少问题。首先，该方法要求每个程序都对“-”符号做专门处理，但是许多程序并未实现这样的功能，这些命令只能处理文件，不支持将标准输入或输出作为参数。其次，有些程序还将单个“-”符解释为表征命令行选项结束的分隔符。
使用/dev/fd 目录，上述问题将迎刃而解，可以把标准输入、标准输出和标准错误作为文件名参数传递给任何需要它们的程序。所以，可以将前一个 shell 命令改写成如下形式：
`$ ls | diff /dev/fd/0/ oldfilelist`
方便起见，系统还提供了 3 个符号链接：/dev/stdin、/dev/stdout 和/dev/stderr，分别链接到/dev/fd/0、/dev/fd/1 和/dev/fd/2
#### 创建临时文件 
很多程序需要创建一些临时文件,仅供其在运行期间使用,程序终止后即删除. 
例如: mkstemp()和tmpfile()。
```c
#include <stdlib.h>
int mkstemp(char *template);
```
!!! 模板参数采用路径名形式，其中最后 6 个字符必须为 XXXXXX。这 6 个字符将被替换，以保证文件名的唯一性，且修改后的字符串将通过 template 参数传回。因为会对传入的 template参数进行修改，所以必须将其指定为字符数组，而非字符串常量. 
文件拥有者对mkstemp()函数建立的文件拥有读写权限(其他用户没有任何权限),且打开文件时使用了O_EXCL标志,以保证调用者以独占的方式访问文件.   

通常打开临时文件不久,程序就会使用unlink系统调用将其删除
* mkstemp()的示例代码如下所示
```c
int fd;
char templateHahahahaha[]="/tmp/somesthingXXXXXX";

fd = mkstemp(templateHahahahaha);
if(fd==-1) errExit("mkstemp");
printf("Generated filename was: %s\n",templateHahahahaha);
unlink(templateHahahahaha);

if(close(fd)==-1)  errExit("close");
``` 
* tmpfile()函数会创建一个名称唯一的临时文件,并以读写进行打开(flag包含O_EXCL标志,以防另一个进程创建了一个同名文件)
```
#include<stdio.h>
FILE *tmpfile(void)
```
tmpfile()函数执行成功，将返回一个文件流供 stdio 库函数使用。文件流关闭后将自动删除临时文件。为达到这一目的，tmpfile()函数会在打开文件后，**从内部立即调用 unlink()来删除该文件名**
#### 总结
本章介绍了原子操作的概念，这对于一些系统调用的正确操作至关重要。 
特别是，**指定O_EXCL 标志调用 open()，这确保了调用者就是文件的创建者**。  
而**指定 O_APPEND 标志来调用 open()，还确保了多个进程在对同一文件追加数据时不会覆盖彼此的输出**。

**系统调用 fcntl()可以执行许多文件控制操作**，其中包括：*修改打开文件的状态标志、复制文件描述符*。**使用 dup()和 dup2()系统调用也能实现文件描述符的复制功能。**

本章接着研究了**文件描述符、打开文件句柄和文件 i-node** 之间的关系，并特别指出这 3 个对象各自包含的不同信息。**文件描述符及其副本指向同一个打开文件句柄，所以也将共享打开文件的状态标志和文件偏移量**。

之后描述的诸多系统调用，是对常规 read()和 write()系统调用的功能扩展。**pread()和pwrite()系统调用可在文件的指定位置处执行 I/O 功能，且不会修改文件偏移量**。**readv()和writev()系统调用实现了分散输入和集中输出的功能。preadv()和 pwritev()系统调用则集上述两对系统调用的功能于一身**。

使用 **truncate() 和 ftruncate()系统调用，既可以丢弃多余的字节以缩小文件大小，又能使用填充为 0 的文件空洞来增加文件大小**。

本章还简单介绍了非阻塞 I/O 的概念，后续章节中还将继续讨论。LFS 规范定义了一套扩展功能，允许在 32 位系统中运行的进程来操作无法以 32 位表示的大文件。

运用虚拟目录/dev/fd 中的编号文件，进程就可以通过文件描述符编号来访问自己打开的文件，这在 shell 命令中尤其有用。

**mkstemp()和 tmpfile()函数允许应用程序去创建临时文件**
### 文件I/O---->缓冲
#### 文件IO的内核缓冲:缓冲区高速缓存 
read()和write()**系统调用在操作磁盘文件时不会直接发起磁盘访问,而是在用户缓冲区与内核缓冲区高速缓存之间复制数据**  
例如: 如下调用将3个字节的数据从用户空间传递到内核空间的缓冲区中: 
`write(fd,"asd",3);`
write()随即返回. 在后续的某个时刻,内核会将缓冲区的数据写入(刷新)至磁盘.可以说 **系统调用与磁盘操作并不同步**,如果在此期间,另一进程试图读取该文件的这几个字节,那么内核将自动从缓冲区高速缓存中提供这些数据,而不是从文件中(读取过期的内容)  
与此同理，对输入而言，内核从磁盘中读取数据并存储到内核缓冲区中。read()调用将从该缓冲区中读取数据，直至把缓冲区中的数据取完，这时，内核会将文件的下一段内容读入缓冲区高速缓存(对于序列化的文件访问，内核通常会尝试执行预读，以确保在需要之前就将文件的下一数据块读入缓冲区高速缓存中) 

**采用这一设计，意在使 read()和 write()调用的操作更为快速**，因为它们不需要等待（缓慢的）磁盘操作  

Linux内核对缓冲区的大小没有固定的上限,其仅受限于两个因素:可用于的物理内存 以及出于其他目的对物理内存的需求. 若可用内存不足,内核会将一些修改过后的缓存区高速缓存页刷新到磁盘,并释放其供系统重用;  
###### 缓冲区大小对IO系统调用性能的影响
当传输总量相同时 缓冲区大小为 1 字节时，需要调用 read()和 write()1亿次，缓冲区大小为 4096 个字节时，需要调用 read()和 write() 24000 次左右，几乎达到最优性能。设置再超过这个值，对性能的提升就不显著了，这是因为与在用户空间和内核空间之间复制数据以及执行实际磁盘 I/O 所花费的时间相比，read()和 write() 系统调用的成本就显得微不足道了
  *总之，如果与文件发生大量的数据传输，通过采用大块空间缓冲数据，以及执行更少的系统调用，可以极大地提高 I / O 性能*。 
#### stdio库的缓冲
###### 简介
**当操作磁盘文件时，缓冲大块数据以减少系统调用，C 语言函数库的 I/O 函数（比如，fprintf()、fscanf()、fgets()、fputs()、fputc()、fgetc()）正是这么做的**。因此，使用 stdio 库可以使编程者免于自行处理对数据的缓冲，无论是调用 write()来输出，还是调用 read()来输入。
##### 设置一个stdio流的缓冲模式  
###### setvbuf()函数:
调用setvbuf()函数,可以控制stdio库使用的缓冲的形式. 
```c
#include <stdio.h>
int setvbuf(FILE * stream,char *buf,int mode,size_t size);
!!setvbuf()     出错时返回非 0 值 **（而不一定是−1）**。
```
参数 stream 标识将要修改哪个文件流的缓冲。打开流后，必须在调用任何其他 stdio 函数之前先调用 setvbuf()。setvbuf()调用将影响后续在指定流上进行的所有 stdio 操作。

参数buf和size则针对参数stream要使用的缓冲区,指定的参数有如下两种方式.
* 如果参数buf不为NULL,那么指向size大小的内存块将作为stream的缓冲区.(其应为malloc()或类似函数在堆区或静态变量区分配的一块空间,而不应是分配在栈上的函数本地变量。否则，函数返回时将销毁其栈帧，从而导致混乱)  
* 若buf为NULL,那么stdio库会为stream自动分配一个缓冲区,该场景下会忽略size参数  
* 参数mode指定了缓冲类型,并具有下列值之一
    * **_IONBF**
    不对 I/O 进行缓冲。每个 stdio 库函数将立即调用 write()或者 read()，并且忽略 buf 和 size参数，可以分别指定两个参数为 NULL 和 0。stderr 默认属于这一类型，从而保证错误能立即输出。
    * **_IOLBF**
    采用行缓冲 I/O。指代终端设备的流默认属于这一类型。对于输出流，在输出一个换行符（除非缓冲区已经填满）前将缓冲数据。对于输入流，每次读取一行数据。
    * **_IOFBF**
    采用全缓冲 I/O。单次读、写数据（通过 read()或 write()系统调用）的大小与缓冲区相同。指代磁盘的流默认采用此模式  
###### setbuf()函数:
setbuf()函数构建在setvbuf()之上,执行了类似任务
```c
#include<stdio.h>
void setbuf(FILE *stream, char *buf);
```
`setbuf(fp,buf)`调用除了不返回函数结果外，就相当于
`setvbuf (  fp,(buf==NULL) ? _IONBF : _IOFBF,BUFSIZ ) ;`
##### fflush()刷新stdio缓冲区
无论当前采用何种缓冲形式,都可以用fflush()函数强制将stdio输出流中的数据强制刷新到内核缓冲区中.
```c
#include <stdio.h>
int fflush(FILE* stream);
```
若stream==NULL 则fflush()将刷新所有的stdio缓冲区.
也能将fflush()函数应用于输入流,这将丢弃以缓冲的输入数据 
当关闭相应流时 自动刷新stdio缓冲区.
> 在包括 glibc 库在内的许多 C 函数库实现中，若 stdin 和 stdout 指向一终端，那么无论何时从 stdin 中读取输入时，都将隐含调用一次 fflush(stdout)函数。这将刷新写入 stdout 的任何提示，但不包括终止换行符（比如，printf("Date：")）。然而，SUSv3 和 C99 并未规定这一行为，也并非所有的 C 语言函数库都实现了这一行为。要保证程序的可移植性，应用应使用显式的 fflush(stdout)调用来确保显示这些提示。
#### 控制文件IO的内核缓冲
强制刷新内核缓冲区到输出文件是可能的,并且有时很重要
##### 同步IO数据完整性和同步IO文件完整性
同步I/O完成-定义为：**某一 I/O 操作，要么已成功完成到磁盘的数据传递，要么被诊断为不成功**
SUSv3 定义的第一种同步 I/O 完成类型是 **synchronized I/O *data* integrity completion** ，旨在确保针对文件的一次更新传递了足够的信息（到磁盘），以便于之后对数据的获取  
* 就读操作而言，这意味着被请求的文件数据已经（从磁盘）传递给进程
* 就写操作而言，这意味着写请求所指定的数据已传递（至磁盘）完毕，且用于获取数据的所有文件元数据也已传递（至磁盘）完毕
**Synchronized I/O *file* integrity completion** **是** SUSv3 定义的另一种同步 I/O 完成 是*synchronized I/O **data** integrity completion* **的超集**该 I/O 完成模式的区别在于在对文件的一次更新过程中，要将所有发生更新的文件元数据都传递到磁盘上，即使有些在后续对文件数据的读操作中并不需要

> * data 类型是指将**足够的信息**(data)传到磁盘  
> * file 类型是将**所有文件信息**(file)传到磁盘 
##### 用于控制文件IO内核缓冲的系统调用函数  
###### fsync()
* fsync()系统调用将使缓冲数据和与打开文件描述符 fd 相关的所有元数据都刷新到磁盘上。调用 fsync()会强制使文件处于 Synchronized I/O **file** integrity completion 状态。(data的超集)
```c
#include <unistd.h>
int fsync(int fd);  // return 0 to success,-1 to error;
```
仅在对磁盘设备（或者至少是其高速缓存）的传递完成后，fsync()调用才会返回。
###### fdatasync()
* fdatasync()系统调用的运作类似于fsync() ，只是强制文件处于ynchronized I/O **data** integrity completion 的状态。
```c
 #include <unistd.h>
 int fdatasync(int fd); //  returns 0 to success , -1 on error;
 ```

##### 通过 open的flag 使所有同步写入:
###### --> O_SYNC
调用open()函数时指定O_SYNC 则会使所有后续输出同步
`fd=open(payhname,O_SYNC|O_WRONLY) `
在调用open函数时添加O_SYNC 之后的每个write函数将会自动将文件数据和元数据刷新到磁盘上(即 按照*************data**************模式进行操作)
采用 O_SYNC 标志（或者频繁调用 fsync()、fdatasync()或 sync()）对性能的影响极大
###### --> O_DSYNC 和 O_RSYNC 标志
同样的 SUSv3 规定了两个与同步IO相关的打开文件标志状态 O_DSYNC 和 O_RSYNC 
* O_DSYNC 标志要求按照(****************data****************模式执行)
* O_SYNC 标志要求按照(****************file****************模式执行)
* O_RSYNC 标志是和上两个结合进行使用的
    * 同时指定 O_RSYNC 和 O_DSYNC 标志，那么就意味着会遵照 synchronized I/O data integrity completion （即，在执行读操作之前，像执行 O_DSYNC 标志一样完成所有待处理的写操作）
    * 同时指定 O_RSYNC 和 O_SYNC 标志，则意味着会遵照 synchronized I/O file integritycompletio （即，在执行读操作之前，像执行 O_RSYNC 标志一样完成所有待处理的写操作）  

##### IO缓冲小节
![截图 2022-10-19 09-31-42.png](https://s2.loli.net/2022/10/19/B4vRx9aYgnPimQj.png)

##### 绕过缓冲区直接IO
直接 I/O 只适用于有特定 I/O 需求的应用。例如数据库系统，其高速缓存和 I/O 优化机制均自成一体，无需内核消耗 CPU 时间和内存去完成相同任务。
* 直接IO的使用方式是---> **用open()打开文件或设备时指定O_DIRECT标志**
###### 直接IO的对齐限制
因为直接IO涉及对磁盘的直接访问,所以在执行IO时 需要遵守以下限制  
* 用于数据传递的缓冲区,其边界必须对齐为块的整数倍大小 
* 数据传输的开始点,即文件设备的偏移量 ,必须为块的大小的整数倍 
* 待传输数据的长度必须是块大小的整数倍  

不遵守上述任一限制将EINVAL错误,(上述 块大小为物理块的大小)

#### 混合使用库函数与系统调用进行IO操作
```c
#include <stdio.h>
int fileno(FILE* stream);
                return file descriptor on success , or -1 on error
FILE* fdopen(int fd,const char * mode);
                return (new) file opinter on success , or NULL on error
```
##### fileno()
给定一个文件流 返回他相应的文件描述符(即stdio库在流上已经打开的文件描述符) 
随即可以在read()write()dup()fcntl()系统调用中正常使用该文件描述符;
##### fdopen()
与fileno()函数功能相反.给定一个文件描述符,该函数创建了一个使用该描述符进行文件IO想相应流.返回一个(文件)流,mode参数和fopen参数中mode参数相同(w a r,写 追加 读...)
如果 该文件描述符 与函数中mode的访问模式不一致,则对fdopen()函数的调用失败   
**fdopen函数对非常规文件描述符很有用 在创建套接字和管道的系统调用总是返回文件描述符**.为了在这些文件类型上使用stdio的库 则必须使用fdopen()函数创建相应的文件流  
##### 混合使用的缓冲问题
***在进行库调用和系统调用时,应将缓冲问题牢记***IO的系统调用将直接吧数据冲到内核缓冲区高速缓存,而stdio库函数需要等待用户空间的缓冲区填满时,再调用write()函数将缓冲区数据从入内核缓冲区高速缓存  
将IO系统调用和stdio函数混合使用时,**使用fflush()来规避这些问题**,同时**也可以使用setvbuf()或setbuf()函数使缓冲区失效** 但这么做会影响IO性能 因为每次输出操作将引起一次write()系统调用
#### 总结
输入输出数据的缓冲由内核和 stdio 库完成。有时可能希望阻止缓冲，但这需要了解其对应用程序性能的影响。可以使用各种系统调用和库函数来控制内核和 stdio 缓冲，并执行一次性的缓冲区刷新。

在 Linux 环境下，open()所特有的 O_DIRECT 标识允许特定应用跳过缓冲区高速缓存。

在对同一个文件执行 I/O 操作时，**fileno()和 fdopen()有助于系统调用和标准 C 语言库函数的混合使用**。给定一个流，fileno()将返回相应的文件描述符，fdopen()则反其道而行之，针对指定的打开文件描述符创建一个新的流。

***
## 系统编程
###  系统编程概念
#### 文件系统结构 
图示磁盘分区和文件系统之间的关系,以及一般文件系统的组成
![截图 2022-10-19 11-47-22.png](https://s2.loli.net/2022/10/19/zFBw48TyZjn5DVe.png)
文件系统由以下部分组成 
##### 引导块 
作为文件系统的首块.引导块不为文件系统所用,只是包含引导操作系统的信息.操作系统只需要一个引导块,但所有文件系统都设有引导块
##### 超级块
紧随引导块之后的一个独立块,包含文件系统有关的参数信息,其中包括:>
* -i 节点容量
* \- 文件系统中的逻辑块大小;
* \- 以逻辑块计,文件系统的大小;  
驻留于同一物理设备上的不同文件系统,其类型.大小以及参数设置(块大小...)都可以有所不同.这也是将一块磁盘划分为多个分区的原因
- \- i节点表: 文件系统的每个文件和目录在i节点表中都对应着唯一一条记录,这条记录登记了关乎文件的各种信息有时节点表称之为i-list 
- \- 数据块:文件系统的大部分空间都用于存放数据,以构成驻留于文件系统之上的文件与目录.
#### i 节点
针对驻留于文件系统上的每个文件，文件系统的 i 节点表会包含一个 i 节点（索引节点的简称）。对 i 节点的标识，采用的是 i 节点表中的顺续位置，以数字表示。文件的 i 节点号（或简称为 i 号）是 ls –li 命令所显示的第一列。
`ls -li`-----* **3283416** drwxr-xr-x 5 **** **** 4096 10月 18 22:23 Desktop*

##### **i(-node) 节点所维护的信息如下所示** 

- **文件类型**(常规文件.目录.符号链接.以及字符设备...)
- **文件属主**(UID 也称 用户ID)
- **文件属组**(GID 也称  组ID)
- **3类文件访问权限**  [ 属主（有时也称为用户）、属组 、其他用户（属主和属组用户之外的用户）]
- **3个时间戳**: 对文件最后的访问时间(ls -lu) 、对文件最后的修改时间(ls -l(默认显示)) 、以及文件状态最后改变的时间(ls -lc)  
- **指向文件的硬链接的数量**
- **文件的大小,以字节为单位** 
- **实际分配给文件块的数量**,以512字节块为单位.不会简单等于文件的大小,有文件空洞的存在(分配给文件的块数可能要低于文件的正常大小所计算出的块数)  
- **指向文件数据块的指针** 

#### ext2 中的i-node(节点)和数据块指针
类似于大多数 UNIX 文件系统，ext2 文件系统在存储文件时，数据块不一定连续，甚至不一定按顺序存放（尽管 ext2 会尝试将数据块彼此靠近存储）。为了定位文件数据块，内核在 i 节点内维护有一组指针。下图所示为在 ext2 文件系统上完成上述任务的情况
![截图 2022-10-19 11-47-22.png](https://s2.loli.net/2022/10/19/zFBw48TyZjn5DVe.png)
> 碎片化的存储方式 使得对磁盘的文件利用的效率更高

这一貌似复杂的系统，其设计意图是为了满足多重需求。首先，该系统在维持 i 节点结构大小固定的同时，支持任意大小的文件。其次，文件系统既可以以不连续方式来存储文件块，又可通过 lseek()随机访问文件，而内核只需计算所要遵循的指针。最后，对于在大多数系统中占绝对多数的小文件而言，这种设计满足了对文件数据块的快速访问：通过 i 节点的直接指针访问，一击必中。

#### 虚拟文件系统(vfs)
虚拟文件系统VFS 定义了一套借口.即使所有与文件交互的程序都会按照这一接口来进行操作  
每种文件系统都会提供VFS接口的实现  
这样以来 程序只需要理解VFS结口,而无需具体关心实现细节  
VFS接口的操作涉及文件系统和目录的所有常规系统调用相对应.
- 这些调用都有open() read() write() lseek() close() truncate() stat() mount() unmount() mmap() mkdir() link() unlink() symlink() rename()
```cpp
 mount() 挂载系统
 umount() 卸载文件系统
 //这俩没整明白咋搞的,应该不咋用吧...
```
VFS的抽象层建模精确仿照传统UNIX文件系统模型.当然,还有一些文件系统,尤其是非UNIX文件系统并不支持所有的VFS操作(微软不支持symlink()创建的链接概念)
### 文件属性
本章将探讨文件的各种属性（文件元数据）。首先介绍的是系统调用 stat()，可利用其返回一个包含多种文件属性的结构。然后，将描述用来改变文件属性的各种系统调用本章将在结尾处讨论 i 节点标志（也称为 ext2 扩展文件属性），可利用其控制内核对文件处理的方方面面
#### stat() 获取文件信息
利用系统调用stat() \ lstat() \ fstat() 可以获取文件有关的信息,其中大部分提取自文件i节点  

```c
#include<sys/stat.h>

int stat(const char * pathname,struct stat * stabuf);
int lstat(const char * pathname,struct stat * stabuf);
int fstat(int fd ,struct stat * stabuf);

```
- stat() 会返回所命名文件名的相关信息.
- lstat()与stat()类似, 区别在如果文件属于符号链接,那么返回的信息针对的是符号链接本身
- fstat() 则会返回某个文件打开符所指代文件的相关信息  

系统调用**stat()和lstat()**无需对其操作文件本身拥有任何权限, **但针对指定的pathname的父目录要有操作权限**
但对于**fstat() 只需要提供有效的文件描述符**   

上述所有结构都会在缓冲区返回一个statbuf 指向的stat结构
```c
其结构如下
//! 需要包含de头文件
#include <sys/types.h>
#include <sys/stat.h> 
int stat(const char *filename, struct stat *buf); //! prototype,原型 
 
struct stat
{
    dev_t       st_dev;     /* ID of device containing file     -文件所在设备的ID               */
    ino_t       st_ino;     /* inode number                     -inode节点号                   */
    mode_t      st_mode;    /* file type and permissions        -文件的类型                     */
    nlink_t     st_nlink;   /* number of hard links             -链向此文件的连接数(硬连接)       */
    uid_t       st_uid;     /* user ID of owner                 -user id                      */
    gid_t       st_gid;     /* group ID of owner                - group id                    */
    dev_t       st_rdev;    /* device ID (if special file)      -设备号,针对设备文件             */
    off_t       st_size;    /* total size, in bytes             -文件大小,字节为单位             */
    blksize_t   st_blksize; /* blocksize for filesystem I/O     -系统块的大小                   */
    blkcnt_t    st_blocks;  /* number of blocks allocated       -文件所占块数                   */
    time_t      st_atime;   /* time of last access              -上次访问时间                   */
    time_t      st_mtime;   /* time of last modification        -上次修改时间                   */
    time_t      st_ctime;   /* time of last status change       -文件状态上次修改时间             */
};
```
##### stat(详细介绍)
- st_dev 和 st_ino  
    - st_dev 字段标识文件所驻留的设备。st_ino 字段则包含了文件的 i 节点号。利用以上两者，可在所有文件系统中唯一标识某个文件
    - dev_t 类型记录了设备的主、辅 ID。如果是针对设备的 i 节点，那么 st_rdev 字段则包含设备的主、辅 ID
- st_uid 和 st_gid
    - 其分别识别文件的属主和属组(用户ID 组ID)
- st_nlink
    - 包含了文件的硬链接数
- st_mode
    - 字段内包含掩码,起标识文件类型 和 指定文件权限的双重作用
    与常量S_IFMT相与(&),可从字段中析取文件类型(Linux 使用st_mode字段中的4位来标识文件类型)
    ![截图 2022-10-19 17-04-41.png](https://s2.loli.net/2022/10/19/BIzaATwgdhKx5LF.png)
    在判断类型的时候 可以使用上图的宏(<sys/stat.h>)
- st_*time 
    - 时间变化记录
#### 文件时间戳
##### 使用utime()和utimes()来改变文件时间戳

```c
#include<utime.h>
//定义结构体
struct utimbuf {
    time_t actime;
    time_t modtime;
}
struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
}

int utime(const char *pathname, const struct utimbuf* buf);
        Returns 0 on success,-1 on error
int utimes(const char *pathname, const struct timeval tv[2]);//268
```
参数 pathname 用来标识欲修改时间的文件。若该参数为符号链接，则会进一步解除引用。
### 目录与链接
#### 目录和(硬)链接
在文件系统中,目录的存储方式类似于普通文件,但目录与普通文件的区别有二
- i-node中,目录会是一种不同的文件类型
- 目录是经特殊组织形成的文件,本质是一个表格,包含文件名和i-node编号  

> 进程能够大开一个目录,但不能read()读取目录的内容,检索目录的内容,进程必须使用后面的系统的调用和库函数. 
> 同理进程也不能使用write()来改变目录的内容,仅能借助于open()函数 link()函数 

>i-node表的编号始于1 而非0，因为若i-node字段值为0，则表明该条目尚未使用。 i-node 1 用来记录文件系统的坏快 。文件根目录总是储存在i-node条目2中，如下图 所以内核在解析路径名时就知道从哪里下手
![截图 2022-10-20 09-30-31.png](https://s2.loli.net/2022/10/20/AcgF8V49OblaQqn.png)

回顾文件i-node中存储的信息,会发现未包含文件名,仅通过目录列表内的一个映射来定义文件名. 其妙用在于，能够在相同或者不同目录中创建多个名称，每个均指向相同的 i-node 节点。也将这些名称称为链接，有时也称之为硬链接
:> **ln** 命令
![截图 2022-10-20 09-40-03.png](https://s2.loli.net/2022/10/20/TZFwp5JnzE1RQXC.png)
> 如图 现在指向该文件的有两个名字。（由于指向相同的 i-node，针对文件 xyz 输出的链接计数也是 2

若移除其中一个文件名,另一文件名以及文件本身将继续存在 
当i-node的链接计数降为0时,也就是移除了所用的名字,才会释放i-node的内存
> 在线论坛上经常会有这样的问题出现：在程序中如何找到与文件描述符 X 相关联的文件名？简单的回答是不能，至少缺乏明确而又便于移植的手段，因为一个文件描述符指向一个i-node，而指向这个 i-node 的文件名则可能有多个

对于硬链接的限制有二 均可使用符号链接加以规避
- 因为目录条目(硬链接)对文件的指代采用了i-node编号,而i-node编号的唯一性仅在一个文件系统之内才能得到保障,所以硬链接要与文件驻留在同一定义文件系统
- 不能为目录创建链接,从而避免出现令诸多系统程序陷于混乱的链接环路  
#### 符号链接(软链接)
符号链接,有时也称为软链接，是一种特殊的文件类型，其数据是另一文件的名称。图 18-2 展示的情况是：两个硬链接—/home/erena/this 和/home/allyn/that—指向同一个文件，而符号链接/home/kiran/other，则指代文件名/home/erena/this。
在shell中，符号链接是由ln-s创建的。ls-F的命令输出会在符号链接的尾部标记 @ 。
符号连接的内容可以是绝对路径，也可以是相对路径。解释相对符号链接时以链接本身的位置作为参照点。
符号链接的地位不如硬链接。尤其是，文件的链接计数中并未将符号链接计算在内因此，如果移除了符号链接所指向的文件名，符号链接本身还将继续存在，尽管无法再对其进行解引用（下溯）操作，也将此类链接称之为悬空链接。更有甚者，还可以为并不存在的文件名创建一个符号链接。
#### 系统调用对符号链接的解释
诸多系统调用都会对符号链接进行解引用处理，从而对链接指向的文件展开操作，还有一些系统调用对符号链接则不作处理，直接操作于链接文件本身。书中会在论及每个系统调用的同时，描述其针对符号链接的行为。（Linux_UNIX系统编程手册--P286）
#### 创建和移除(硬)链接--link()和unlink()
link()和unlink()系统调用分别创建和移除硬链接.
##### link()
```c
#include<unistd.h>
int link(const char* oldpath,const char* newpath);
                return 0 on success;-1 on error
```
oldpath中提供的是一个已经存在的路径名,则系统调用link()将newpath参数所指向的路径名创建一个新链接.若newpath指定的路径名已然存在,则不会将其覆盖;相反会产生一个error;
> 在 Linux 中，link()系统调用不会对符号链接进行解引用操作。若 oldpath 属于符号链接，则会将 newpath 创建为指向相同符号链接文件的全新硬链接。（换言之，newpath 也是符号链接，指向 oldpath 所指代的同一文件。）这一行为有悖于 SUSv3 规范。SUSv3 要求，除非另行规定（link()系统调用不在此列），否则所有执行路径名解析操作的函数都应对符号链接进行解引用。大多数其他 UNIX 实现的行事方式都与 SUSv3 相符。值得注意的是，Solaris 是个例外，默认情况下的行为与 Linux 相同。但若采用适当的编译器选项，又可提供符合 SUSv3 规范的行为。鉴于系统实现间的这种差异，应避免将 oldpath 参数指定为符号链接，以保障程序的可移植性。
##### unlink()
```c
#include <unistd.h>
int unlink(const char *pathname);
                    RETURN 0 ON SUCCESS;-1 ON error
```
unlink()系统调用移除一个链接（删除一个文件名），且如果此链接是指向文件的最后一个链接，那么还将移除文件本身。若 pathname 中指定的链接不存在，则 unlink()调用失败，并将errno 置为 ENOENT
##### 仅当关闭所有文件描述符时，可删除一个已打开的文件
当移除指向文件的最后一个链接时，如果仍有进程持有指代该文件的打开文件描述符，那么在关闭所有此类描述符之前，系统实际上将不会删除该文件。这一特性的妙用在于允许取消对文件的链接，而无需担心是否有其他进程已将其打开。（然而，对于链接数已降为 0 的打开文件，就无法将文件名与其重新关联起来。）此外，基于上述事实，还可以玩点小技巧：先创建并打开一个临时文件，随即取消对文件的链接（unlink），然后在程序中继续使用该文件。
（这正是 5.12 节所述 tmpfile()函数的所作所为。）
#### 更改文件名 : rename()
借助于rename()既可以重命名文件,又可以将文件移动至同一文件系统的另一目录.
```c
#include<stdio.h>
int rename(const char* oldpath,const char*newpath);
                return 0 on success; -1 on error
```
该系统调用会将现有的一个路径名oldpath重命名为newpath参数指定的路径名。
rename()调用仅操作目录条目，而不移动文件数据 
- :> !!! 注意 !!!
    - **若 newpath 已经存在，则将其覆盖**。
    - **若 newpath 与 oldpath 指向同一文件，则不发生变化（且调用成功）**
    - **rename()系统调用对其两个参数中的符号链接均不进行解引用。**如果 oldpath 是一符号链接，那么将重命名该符号链接。如果 newpath 是一符号链接，那么会将其视为由oldpath 重命名而成的普通路径名
    - **如果 oldpath 指代文件，而非目录，那么就不能将 newpath 指定为一个目录的路径名**（否则将 errno 置为 EISDIR）
    如下调用既将一个文件移动到另一目录中，同时又将其改名：
    `rename("a/wang1.txt","b/wang2.txt");`
    - 若将 **oldpath 指定为目录名，则意在重命名该目录**。**这种情况下，必须保证 newpth 要么不存在，要么是一个空目录的名称**
    - **若 oldpath 是一目录，则 newpath 不能包含 oldpath 作为其目录前缀**
    - **oldpath 和 newpath 所指代的文件必须位于同一文件系统。**
#### 使用符号连接: symlink()和readlink()
##### symlink()
symlink()系统调用会针对filepath所指定的路径名创建一个新的符号链接--linkpath. (移除符号链接,需使用unlink() )
```c
#include<unistd.h>
int symlink(const char *filepath,const char *linkpath);
                return 0 on success, or -1 on error
```
! 若linkpath 中给定的路径名已然存在,则调用失败(且将error设置为EEXIST).filepath指定的路径名可以是绝对路径,也可以是相对路径.
由 filepath 所命名的文件或目录在调用时无需存在。即便当时存在，也无法阻止后来将其删除。这时，linkpath 成为“悬空链接”，其他系统调用试图对其进行解引用操作都将出错（通常错误号为 ENOENT）
##### readlink()
readlink()系统调用的本职工作，将符号链接字符串的一份副本置于 buffer 指向的字符数组中
```c
#include<unistd.h>
ssize_t readlink(const char *pathname,char *buffer,size_t bufsize);
            return number of bytes placed in buffer on success , or -1 on error
```
bufsiz 是一个整型参数，用以告知 readlink()调用 buffer 中的可用字节数
如果一切顺利，readlink()将返回实际放入 buffer 中的字节数。若链接长度超过 bufsiz，则置于 buffer 中的是经截断处理的字符串（并返回字符串大小，亦即 bufsiz）;
> 由于 buffer 尾部并未放置终止空字符，故而也无法分辨 readlink()所返回的字符串到底是经过截断处理，还是恰巧将 buffer 填满。验证后者的方法之一是重新分配一块更大的 buffer，并再次调用 readlink()。另外，还可以将 pathname 的长度定义为常量 PATH_MAX (该常量定义了程序可拥有的最长路径名长度)
#### 创建mkdir()和rmdir()
##### mkdir()系统调用用于创建一个新目录;
```c
#include <sys/stat.h>
int mkdir(const char *pathname,mode_t mode);
            return 0 on success , or -1 on error
```
pathname参数指定了新目录的路径名. 该名可以是相对路径, 也可以是绝对路径,若具有该路径名的文件已经存在,则调用失败并设置error
**对该位掩码值的指定方式既可以与 open()调用相同-----对表所列各常量进行或(|)操作，也可直接赋予八进制数值**。既定的 mode 值还将于进程掩码相与（&）（参见 15.4.6 节）。另外，set-user-ID 位始终处于关闭状态，因为该位对于目录而言毫无意义。
##### rmdir()删除一个目录
```c
#include <unistd.h>
int rmdir(const char *pathname);
            return 0 on success , or -1 on error
```
************要使 rmdir()调用成功，则要删除的目录必须为空************。
如果 pathname 的最后一部分为符号链接，那么 rmdir()调用将不对其进行解引用操作，并返回错误，同时将 errno 置为 ENOTDIR。
#### 移除一个文件或目录: remove()
```c
#include<stdio.h>
int remove(const char *pathname);
            return 0 on success;-1 on error
```
remove()属于C语言库 而非系统调用
- 如果pathname是文件 那么remove()将调用unlink()
- 如果pathname是目录 那么remove()将调用rmdir()  
与unlink()和rmdir()一样 remove()函数不会对符号链接解引用 其会直接删除符号连接本身,而非指向的文件  
#### 读目录: opendir()和readdir()
本节所述库函数可用于打开一个目录并逐一获取其包含文件的名称. 
##### opendir()/fdopendir()
用于打开一个目录,并返回指向该目录的句柄
```c
#include<dirent.h>

DIR * opendir(const char * dirpath);
            return directory stream handle , or NULL on error
```
opendir()函数打开dirpath指定的目录,并返回DIR类型结构的指针,该结构即所谓目录流
```c
#include<dirent.h>

DIR * fdopendir(int fd);
            return directory stream handle , or NULL on error
```
> 提供 fdopendir()函数，意在帮助应用程序免受各种竞态条件的困扰
##### readdir()
```c
#include<dirent.h>

struct dirent {
    ino_t d_ino;  // file i-node number
    char d_name[];// Null-terminated name of file// 256 byte
}

struct dirent * readdir(DIR * dirp);

```
每调用readdir()一次,就会从dirp中读取目录流中的下一目录条目 返回一枚指针,指向静态分配的dirent类型结构 
每次调用readdir() dirent结构就会被覆盖掉
遇到目录结尾或出错 readdir()将返回NULL (出错设置errno) 


## 信号
信号分两大类 
- 一组用于向内核通知事件 构成所谓的标准信号  Linux中标准信号编号范围是0-31 
- 另一组由实时信号构成

产生信号后 其会于稍后进行传递 
若目标进程正在运行 则马上传递并响应 
若目标进程阻塞 则在目标进程运行时进行传递

得到信号后
- 忽略信号：也就是说，内核将信号丢弃，信号对进程没有产生任何影响（进程永远都不知道曾经出现过该信号）。

- 终止（杀死）进程：这有时是指进程异常终止，而不是进程因调用 exit()而发生的正常终止。

- 产生核心转储文件，同时进程终止：核心转储文件包含对进程虚拟内存的镜像，可将其加载到调试器中以检查进程终止时的状态。

- 停止进程：暂停进程的执行。

- 于之前暂停后再度恢复进程的执行。

除了根据特定信号而采取默认行为之外，程序也能改变信号到达时的响应行为。也将此称之为对信号的处置（disposition）设置。程序可以将对信号的处置设置为如下之一。
-  采取默认行为。这适用于撤销之前对信号处置的修改、恢复其默认处置的场景。

- 忽略信号。这适用于默认行为为终止进程的信号。

- 执行信号处理器程序。
  - 信号处理器程序是由程序员编写的函数，用于为响应传递来的信号而执行适当任务

> 信号处理器程序是由程序员编写的函数，用于为响应传递来的信号而执行适当任务。例如，shell 为 SIGINT 信号（由中断字符串 Control-C 产生）提供了一个处理器程序，令其停止当前正在执行的工作，并将控制返回到（shell 的）主输入循环，并再次向用户呈现 shell 提示符

无法将信号处置设置为终止进程或者核心转储//最为相似的行为是该信号安装一个处理器程序 于其中调用exit()或者abort()
abort()函数将为进程产生一个SIGABRT信号 该信号将引发进程转储核心文件并终止

### 信号类型和默认行为
![](https://img-blog.csdnimg.cn/2021010821250941.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3poaXpoZW5nZ3Vhbg==,size_16,color_FFFFFF,t_70)
![](https://img-blog.csdnimg.cn/20210108212619741.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3poaXpoZW5nZ3Vhbg==,size_16,color_FFFFFF,t_70)

- 各种信号产生条件和默认处理方式描述如下：
    - **SIGABRT** 默认处理方式：终止+core；当程序调用abort函数时，会产生该信号。程序异常终止。
    - **SIGALRM** 默认处理方式：终止；当由alarm或setitimer函数设置的定时器超时时，会产生该信号。
    - **SIGBUS** 默认处理方式：终止+core；经常因为内存错误产生该信号。
    - **SIGCHLD** 默认处理方式：忽略；当进程terminate或stopped的时候，该信号会发送给父进程。如果父进程需要知道子进程什么时候终止，父进程必须捕获该信号。通常在该信号的捕获函数中调用wait或waitpid获取子进程的pid和终止状态。
    - **SIGCONT** 默认处理方式：忽略/继续；作业控制命令进程继续执行时，该信号发送给进程。如果进程之前已被停止，则该信号的默认处理方式是继续进程的执行；否则，忽略该信号。
    - **SIGFPE** 默认处理方式：终止+core；当发生算术错误（如：除零，溢出等）时，产生该信号。
    - **SIGHUP** 默认处理方式：终止；当终端界面检测到连接断开时，内核向与控制终端的session leader进程发送该信号（当且仅当终端的CLOCAL标识位没有被设置时，才会发送该信号）。接收信号的session leader可能是后台进程，这与普通终端产生的信号不同，普通终端信号接收者是前台进程组。另外，当控制终端的session leader终止时，SIGHUP信号会发送到前台进程组。因为守护进程没有控制终端，通常不应该接收该信号的，所以这个信号常常被用作守护进程重新读取配置文件的信号。
    - **SIGILL** 默认处理方式：终止；当处理器执行了非法指令时，产生该信号。
    - **SIGINT** 默认处理方式：终止；当向终端输入终端键（Control+C或DELETE）时，终端产生SIGINT信号。该信号被发送到前台进程组。通常用来终止已运行的进程。
    - **SIGIO** 默认处理方式：终止/忽略；该信号用来提供异步IO模式。当有IO可用时，产生该信号通知进程。
    - **SIGKILL** 默认处理方式：终止；该信号给超级用户提供了终止任何进程的能力，通常通过kill函数或命令。该信号不能够被忽略或捕获。
    - **SIGPIPE** 默认处理方式：终止；当向已经关闭读者的管道写数据时，会产生该信号。同样向未连接的SOCK_STREAM类型的socket写数据时，也会产生该信号。
    - **SIGPOLL** 默认处理方式：终止；当指定的事件发生在可选择的设备上时，产生该信号。
    - **SIGPROF** 默认处理方式：终止；由setitimer设置的间隔定时器超时会产生该信号。
    - **SIGPWR** 默认处理方式：终止；当系统有UPS（Uninterruptible Power Supply，即电池）时，断电后使用电池，当电池电量低时，会产生该信号通知进程在1530秒内关闭。
    - **SIGQUIT** 默认处理方式：终止+core；当输入退出键（Control+\）时，终端将会产生SIGQUIT信号，该信号被传送到前台进程组。
    - **SIGSEGV** 默认处理方式：终止+core；非法内存引用时，产生该信号。
    - **SIGSTOP** 默认处理方式：停止进程；作业控制信号，用来停止进程。该信号不能被忽略或捕获。
    - **SIGSYS** 默认处理方式：终止+core；非法系统调用时，产生该信号。
    - **SIGTERM** 默认处理方式：终止；kill函数默认发送的信号，用来终止进程。
    - **SIGTRAP** 默认处理方式：终止+core；系统定义的硬件错误。通常，在遇到调试断点时，将控制权传递给debugger。
    - **SIGTSTP** 默认处理方式：停止进程；当输入挂起键（Control+Z）时，终端产生该（交互式停止）信号停止进程。该信号被发送给前台进程组。
    - **SIGTTIN** 默认处理方式：停止进程；当后台进程组中的进程要求从控制终端读取数据时，会产生该信号。有两个例外情况：1、要求读数据的后台进程忽略或阻塞了该信号，2、进程所属进程组是“孤儿”。在这两种情况下，不会产生该信号，否则，read会错误返回，并将errno设置为EIO。
    - **SIGTTOU** 默认处理方式：停止进程；当后台进程组中的进程要求写数据到控制终端时，会产生该信号。后台进程可以被允许写数据到控制终端。当不允许后台进程写数据到控制终端时，write会错误返回，并将errno设置为EIO。到有两个例外情况：1、要求写数据的后台进程忽略或阻塞了该信号，2、进程所属进程组是“孤儿”。在这两种情况下，不会产生该信号。
    - **SIGURG** 默认处理方式：忽略；当网络连接（Socket）接收到带外数据（out-of-band data）时，会产生该信号。
    - **SIGUSR1** 默认处理方式：终止；用户自定义的信号。
    - **SIGUSR2** 默认处理方式：终止；用户自定义的信号。
    - **SIGVTALRM** 默认处理方式：终止；由setitimer设置的虚拟定时器超时时，产生该信号。
    - **SIGXCPU** 默认处理方式：终止+core/忽略；进程超过了CPU的软限制时，产生该信号。
    - **SIGXFSZ** 默认处理方式：终止+core/忽略；进程超过了文件大小的软限制时，产生该信号。  


### 改变信号处置 signal()/sigaction()
#### signal()
```c 
#include <signal.h>
void (*signal(int sig,void(*handler)(int)))(int );
```
`signal(int sig,void(*handler)(int))`signal函数有2个参数，第一个是int，第二个是无返回值，带一个int参数的函数指针
`void   (*signal(xxx))   (int)`  **signal函数返回的是一个函数指针**，无返回值，有一个int参数

```c 
test.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sighandler(int);

int main()
{
   signal(SIGINT , sighandler);//第一个位置不是掩码
   signal(SIGTSTP , sighandler);

   while(1) 
   {
      printf("开始休眠一秒钟...\n");
      sleep(1);
   }

   return(0);
}

void sighandler(int signum)
{
   printf("捕获信号 %d,跳出...\n", signum);
   exit(1);
}
```
#### sigaction()
sigaction()调用比signal调用更复杂 也更灵活
```c
#include<signal.h>
int sigaction(int sig,const struct sigaction *act , struct struct *oldact);

```
- sig 要获得或改变的信号编号.该参数可以是除去SIGKILL和SIGSTOP之外的任何信号
- act 要设置的对信号的新处理方式
- oldcat 对原来信号的处理方式
返回值 0——成功 -1——有错误生成

如下是`sigaction strcut`的定义 
在调用sigaction 时要定义其相应的结构体
```c
struct sigaction
{
 void     (*sa_handler)(int);//信号处理函数
 void     (*sa_sigaction)(int, siginfo_t *, void *);
 sigset_t  sa_mask;// 将sa_mask信号组添加至进程掩码 直至信号处理函数返回
 int       sa_flags;
 void     (*sa_restorer)(void);
};
```
`sa_handler` 对应 `signal()的 handle参数`
- 其所指定的值为信号处理器函数的地址，亦或是常量 SIG_IGN、SIG_DFL 之一。仅当 sa_handler 是信号处理程序的地址时，亦即 sa_handler的取值在 SIG_IGN 和 SIG_DFL 之外，才会对 sa_mask 和 sa_flags 字段（稍后讨论）加以处理。
- sa_mask 定义了一组信号,当调用信号处理器时，将该组信号中未处于进程掩码之列的信号 添加至进程掩码 直至信号处理函数返回
  - 即 使用sa_mask定义一组信号 不允许这组信号在执行信号处理函数时中断函数


 sa_flags 字段是一个位掩码，指定用于控制信号处理过程的各种选项。该字段包含的位如下（可以相或（|））。

- **SA_NOCLDSTOP**
  - 若 sig 为 SIGCHLD 信号，则当因接受一信号而停止或恢复某一子进程时，将不会产生此信号。参见 26.3.2 节。

- **SA_NOCLDWAIT**
  - （始于 Linux 2.6）若 sig 为 SIGCHLD 信号，则当子进程终止时不会将其转化为僵尸。更多细节参见 26.3.3 节。

- **SA_NODEFER**
  - 捕获该信号时，不会在执行处理器程序时将该信号自动添加到进程掩码中。SA_NOMASK历史上曾是 SA_NODEFER 的代名词。之所以建议使用后者，是因为 SUSv3 将其纳入规范。

- **SA_ONSTACK**
  - 针对此信号调用处理器函数时，使用了由 sigaltstack()安装的备选栈。参见 21.3 节。

- **SA_RESETHAND**
  - 当捕获该信号时，会在调用处理器函数之前将信号处置重置为默认值（即 SIG_DFL）（默认情况下，信号处理器函数保持建立状态，直至进一步调用 sigaction()将其显式解除。）

- **SA_ONESHOT** 历史上曾是 SA_RESETHAND 的代名词，之所以建议使用后者，是因为 SUSv3将其纳入规范。

- **SA_RESTART**
  - 自动重启由信号处理器程序中断的系统调用。参见 21.5 节。

- **SA_SIGINFO**
  - 调用信号处理器程序时携带了额外参数

```c
sigaction(SIGINT, &saIgnore, &saOrigInt);
sigaction(SIGQUIT, &saIgnore, &saOrigInt);
//将原来对信号的操作保存到 saOrinInt 结构体中
//将新的信号处理程序改为 saIgnore 
```
- 对于 结构体sigaction 的
    - sa_handler参数 
        - == SIG_IGN : 忽略信号
        - == SIG_DEF : 恢复信号的默认行为
    - sigset_t sa_mask： 
        - 指定一个。信号集，在调用sa_handler所指向的信号处理函数之前，该信号集将被加入到进程的信号屏蔽字中。信号屏蔽字是指当前被阻塞的一组信号，它们不能被当前进程接收到
    


### 信号处理器
信号处理程序 当指定信号传递给进程时会调用的一个函数. 
调用信号处理程序,可能会随时打断主程序流程:内核代表进程来调用处理器程序
简单信号处理器
```cpp
#include<signal.h>
static void sigHandler(int sig)
{
    printf("wow");
}
int main(int argc,char ** argv)
{
    int j;
    if(signal(SIGINT, sigHandler)==SIG_ERR)
    exit ("SIGNAL");

    while(1)
    {
        printf("%d\n",j);
        sleep(3);
    }
}

```
##### pause()等待进程
pause会阻塞进程 直到键入某个字符
### 发送信号: kill()
一个进程能够使用kill()系统调用向另一进程发送信号.
```c
#include<signal.h>
int kill(pid_t pid,int sign);
        return 0 on success, or -1 on error
```
- pid > 0 则会发送给信号由pid指定的进程
- pid = 0 则会发送信号给与调用进程组相同的每个进程.包括将信号发送给自己
- pid < -1 会向组ID绝对值与pid相等的进程(向一个进程组的所有进程发送信号在shell作业中有特殊用途)
### raise()
```c
int raise(int sign);
```
向自己发送信号 
### 信号集函数

- `int sigemptyset(sigset_t *set);`

  - 该函数的作用是将信号集初始化为空。

- `int sigfillset(sigset_t *set);`

  - 该函数的作用是把信号集初始化包含所有已定义的信号。

- `int sigaddset(sigset_t *set,int signo);`

  - 该函数的作用是把信号signo添加到信号集set中，成功时返回0，失败时返回-1。

- `int sigdelset(sigset_t *set,int signo);`

  - 该函数的作用是把信号signo从信号集set中删除，成功时返回0，失败时返回-1.

- `int sigismember(sigset_t *set,int signo);`

  - 该函数的作用是判断给定的信号signo是否是信号集中的一个成员，如果是返回1，如果不是，返回0，如果给定的信号无效，返回-1；

- `int sigprocmask(int how,const sigset_t *set,sigset_t *oset);`

  - 该函数可以根据参数指定的方法修改进程的信号屏蔽字。新的信号屏蔽字由参数set（非空）指定，而原先的信号屏蔽字将保存在oset（非空）中。如果set为空，则how没有意义，但此时调用该函数，如果oset不为空，则把当前信号屏蔽字保存到oset中。

- `int sigpending(sigset_t *set);`

  - 该函数的作用是将被阻塞的信号中停留在待处理状态的一组信号写到参数set指向的信号集中，成功调用返回0，否则返回-1，并设置errno表明错误原因。

- `int sigsuspend(const sigset_t *sigmask);`

  - 该函数通过将进程的屏蔽字替换为由参数sigmask给出的信号集，然后挂起进程的执行。注意操作的先后顺序，是先替换再挂起程序的执行。程序将在信号处理函数执行完毕后继续执行。如果接收到信号终止了程序，sigsuspend()就不会返回，如果接收到的信号没有终止程序，sigsuspend()就返回-1，并将errno设置为EINTR。




## 进程
### 进程和程序
进程是一个可执行程序的实例.
程序是包含了一系列信息的文件，这些信息描述了如何在运行时创建一个进程， 所包括的内容如下
- 二进制格式标识：每个程序文件都包含用于描述可执行文件格式的元信息。（a.out）
- 机器语言指令：对程序算法进行编码。
- 程序入口地址：标识程序开始执行时的启始指令位置。
- 数据：程序文件包含的变量初始值和程序使用的字面量值。
- 符号表及重定位表：描述程序中函数和变量的位置及名称。
- 其他信息

#### 进程号和父进程号
每个进程都有唯一一个进程号，进程号是一个正数，用以唯一标识系统中的某个进程。对各种系统调用而言，进程号有时可以作为传入参数有时可以作为返回值。
- getpid()
```c
#include<unistd.h>
pid_t getpid();
```
getpid()返回调用进程的进程号，linux限制进程号需要小于等于32767,创建心进程时内核会按顺序将下一个可用的进程号分配给其使用。每当进程号达到32767的限制时，内核将重置进程号技术器 
> 一旦进程号达到 32767，会将进程号计数器重置为 300，而不是 1。之所以如此，是因为低数值的进程号为系统进程和守护进程所长期占用，在此范围内搜索尚未使用的进程号只会是浪费时间

每个进程都有一个创建自己的父进程。getppid()可以检索到自己的父进程号
```c
#include<unistd.h>
pid_t getppid(void);

```
每个进程都有自己的父进程，进程号之间的属性反应了进程之间的树状关系。
进程 1 init进程，即所有进程的始祖。使用pstree(1)命令可以查看到这一家族树
若父进程终止，则子进程将变为孤儿，init将收留子进程对子进程的getppid()函数 将返回 1 
#### 进程内存布局
每个进程所分配的内存由很多部分组成，称之为“段”
- 文本段 包含了进程运行的程序机器语言指令。文本段具有只读属性，以防进程通过错误指针意外修改自身指令。<!-- 因为多个进程可以同时运行同一程序 --> 所以又将文本段设为可以共享，这样一份代码的拷贝可以映射到这些进程的虚拟地址空间中
- 初始化数据段 包含显式初始化的全局变量和静态变量. 当程序加载到内存时,从可执行程序中读取这些变量的值  
- 未初始化数据段 包含了未进行显式初始化的全局变量和静态变量.程序启动之前,系统将本段内所有内存初始化为 0 ;
- 栈 (stack) 是一个动态增长和收缩的段 ,由栈帧组成,系统会为当前调用的函数分配一个栈帧.栈帧中存储了函数的局部变量.实参.返回值
- 堆 (heap) 是在可运行时为变量动态进行内存分配的一块区域
#### 虚拟内存管理
linux 同现代很多内核一样,采用了虚拟内存管理技术,该技术利用了大多数程序访问的访问局部性,以求cpu和RAM资源.大多程序都展现了两种类型的局部性
- 空间局部性: 是指程序倾向与访问在最近访问过的内存地址附近的内存,
- 时间局部性: 是指程序倾向于在不久的将来再次访问最近刚访问过的内存地址(由于循环)

虚拟内存的规划之一是将每个应用程序使用的内存切割成小型的、固定大小的“页”单元，相依的，将RAM划分的一系列与虚拟页尺寸相同的页帧。任一时刻，每个程序仅有部分页需要存留在物理页帧中
这些页构成了驻留集。程序未使用的页拷贝保存在交换区，这是磁盘空间中的保留区域，作为计算机RAM的补充----仅在需要时才会载入物理内存。若进程想要访问的页面目前并未驻留在物理内存中，将会发生页面错误。内核将挂起进程，同时从磁盘中将页面载入内存

为支持这种方式，__内核需要为每个进程维护一张页表__ 该页表描述了每页在进程虚拟地址空间中的位置 页表中的每个条目,要么指出一个虚拟页面在RAM中的位置,要么表明其当前驻留在磁盘上.

#### 栈和栈帧
函数的调用和返回使栈的增长和收缩呈现性.栈驻留在内存的高端并向下增长,专用寄存器--栈指针.用于跟踪当前的栈顶.每次调用时,会在栈上新分配一帧,每当函数返回,则会从栈上将此帧移去
每当函数调用 则会为其分配栈帧,(递归调用自己同理)
#### 环境变量
大多数shell通过`export`命令向环境中添加变量
在哪进程创造时,会继承父进程的环境变量,二者通过环境变量来通信,在子进程创建后,二这可以更改各自的环境变量,而不会对对方在成影响

##### 在程序中访问环境变量
C 语言中,可以使用`char **environ`访问环境列表,(environ)与(argv)参数类似指向一个以NULL结尾的指针列表,每个指针又指向一个以空字符结尾的字符串
此外 还可以同过声明main函数的参数列表来访问环境列表
`int main(int argc, char ** argv,char **envp)`
- getenv()函数能够从进程环境中检索单个值.
```c
#include<stdlib.h>
char *getenv(const char *name);
```
返回相应的字符串指针
如果指定 SHELL 为参数 name，那么将返回/bin/bash。如果不存在指定名称的环境变量，那么 getenv()函数将返回 NULL
##### 修改环境
```c
#include<stdlib.h>
int putenv(char* string);

```
参数 string 是一指针，指向 name=value 形式的字符串。调用 putenv()函数后，该字符串就成为环境的一部分，换言之，putenv 函数将设定 environ 变量中某一元素的指向与 string 参数的指向位置相同，而非 string 参数所指向字符串的复制副本。因此，如果随后修改 string 参数所指的内容，这将影响该进程的环境。出于这一原因，string 参数不应为自动变量（即在栈中分配的字符数组 ），因为定义此变量的函数一旦返回，就有可能会重写这块内存区域。
- putenv()函数的 glibc 库实现还提供了一个非标准扩展。如果 string 参数内容不包含一个等号（=），那么将从环境列表中移除以 string 参数命名的环境变量

```c
#include<stdlib.h>
int setenv(const char *name,const char *value,int overwrite);
```
setenv()函数为形如 name=value 的字符串分配一块内存缓冲区，并将 name 和 value 所指向的字符串复制到此缓冲区，以此来创建一个新的环境变量。注意，不需要（实际上，是绝对不要）在 name 的结尾处或者 value 的开始处提供一个等号字符，因为 setenv()函数会在向环境添加新变量时添加等号字符。

#### 执行非局部跳转
setjmp()_和_longjmp()
在一个深度嵌套的函数调用中发生了错误，需要放弃当前任务，从多层函数调用中返回
```c
#include <setjmp.h>
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
```
setjmp()调用为后续longjmp()调用的执行确立了跳转目标. 该目标正是程序发起setjmp()调用的位置, 
- 可以区分 setjmp 调用是初始返回还是第二次“返回”。初始调用返回值为 0，后续“伪”返回的返回值为 longjmp()调用中 val 参数所指定的任意值。
    - 若指定的val为0,则函数会将二次返回的值调换为1;
setjmp()直接调用返回0 ;若从longjmp()处返回则返回longjmp()中指定的val.
使用longjmp()的第二个参数的原因是可以多个longjmp()对应一个setjmp()
```c
#include <stdio.h>
#include <setjmp.h>
 
static jmp_buf buf;
 
void second(void) {
    printf("second\n");         // 打印
    longjmp(buf,1);             // 跳回setjmp的调用处 - 使得setjmp返回值为1
}
 
void first(void) {
    second();
    printf("first\n");          // 不可能执行到此行
}
 
int main() {   
    if ( ! setjmp(buf) ) {
        first();                // 进入此行前，setjmp返回0
    } else {                    // 当longjmp跳转回，setjmp返回1，因此进入此行
        printf("main\n");       // 打印
    }
 
    return 0;
}
```
```shell
程序输出
second
main
```
-  longjmp必须在setjmp调用之后，而且longjmp必须在setjmp的作用域之内。具体来说，在一个函数中使用setjmp来初始化一个全局标号，然后只要该函数未曾返回，那么在其它任何地方都可以通过longjmp调用来跳转到 setjmp的下一条语句执行。实际上setjmp函数将发生调用处的局部环境保存在了一个jmp_buf的结构当中，只要主调函数中对应的内存未曾释放 （函数返回时局部内存就失效了），那么在调用longjmp的时候就可以根据已保存的jmp_buf参数恢复到setjmp的地方执行。

- setjmp()函数的使用限制
    - SUSv3和C99规定,对setjmp()的调用只能在如下语境中使用
1. if、switch、while等整个控制表达式.
2. 作为一元操作符! 的操作对象,其最终表达式构成了选择或迭代语句的整个控制表达式
3. 作为比较操作的一部分,另一操作对象必须是一个常量表达式
4. 作为独立的函数调用,没有嵌入更大的表达式
    - `int a = setjmp(env);/*WRONG!*/`
之所以规定这些限制，是因为作为常规函数的 setjmp()实现无法保证拥有足够信息来保存所有寄存器值和封闭表达式中用到的临时栈位置，以便于在 longjmp()调用后此类信息能得以正确恢复。因此，仅允许在足够简单且无需临时存储的表达式中调用 setjmp()。

- **滥用longjmp()**
如果将 env 缓冲区定义为全局变量对所用函数可见 , 那么就可以执行如下操作.
1. 调用函数x(),使用setjmp()调用在全局变量env中建立一个跳转目标
2. 从函数x()中返回.
3. 调用函数y(),使用env变量调用longjmp()函数
***这是一个严重错误，因为 longjmp()调用不能跳转到一个已经返回的函数中***。思考一下，在这种情况下，longjmp()函数会对栈打什么主意—尝试将栈解开，恢复到一个不存在的栈帧位置，这无疑将引起混乱。如果幸运的话，程序会一死（crash）了之。然而，取决于栈的状态，也可能会引起调用与返回间的死循环，而程序好像真地从一个当前并未执行的函数中返回了。（在多线程程序中有与之相类似的滥用，在线程某甲中调用 setjmp()函数，却在线程某乙中调用 longjmp()。）
### 进程的创建
#### fork() exit() wait() execve()的简介
- fork()函数允许一进程创建一新进程.具体的子进程为父进程的拷贝,子进程拥有父进程的栈,数据段,堆,和执行文本段.
- **exit(status)函数**,终止一个进程将进程的所有资源归还内核,交给其进行再次分配.参数status为一整型变量,表示进程的退出状态.父进程可以用wait()函数来获取该状态
- **wait(&status)函数** 目的有二 如果子进程未调用exit()函数 那么wait函数会挂起父进程直到子进程终止;其二子进程的终止状态通过exit()函数返回
- 系统调用execve(pathname,argv,envp)加载一个新程序路径名为pathname,参数列表为argv,envp)到当前进程的内存。这将丢弃现存的文本段，并为新程序重新创建栈、数据段、堆。这一操作通常称为执行一个新程序。

#### fork()
创建一个新的子进程 
```c
#include<unistd.h>
pid_t fork(void);
```
理解 fork()的诀窍是，要意识到，完成对其调用后将存在两个进程，且每个进程都会从 fork()的返回处继续执行。这两个进程将执行相同的程序文本段，但却各自拥有不同的栈段、数据段以及堆段拷贝。子进程的栈、数据以及栈段开始时是对父进程内存相应各部分的完全复制。执行 fork()之后，每个进程均可修改各自的栈数据、以及堆段中的变量，而并不影响另一进程
- 不同的是 fork()在不同的进程中返回不同  
fork()在父进程中返回0  
在子进程中返回子进程的进程ID(pid_t)  
    - //fork()创建失败后返回-1
**调用 fork()之后，系统将率先“垂青”于哪个进程（即调度其使用 CPU），是无法确定的，意识到这一点极为重要。在设计拙劣的程序中，这种不确定性可能会导致所谓“竞争条件（racecondition）”的错误**，24.2 节会对此做进一步说明 
##### 父子进程之间的文件共享
父子进程之间共享文件属性的妙用屡见不鲜.父子进程同时写入一文件,共享文件偏移量会确保二者不会覆盖彼此的输出内容. 但这并不能阻止进程之间的输入混乱, 若要规避这一现象 需要进行进程同步.比如父进程可以调用wait()来暂停运行 
##### vfork()//不建议?
```c
pid_t vfork(void );
```
fork()和vfork()区别
- vfork会挂起父进程,直到vfork出的子进程进行_exit()或exec()操作
- 与fork()不同 vfork()的子进程与父进程共享内存,
- vfork()产生的子进程不应调用 exit()退出，因为这会导致对父进程 stdio 缓冲区的刷新和关闭
除非速度绝对重要的场合，新程序应当舍 vfork()而取 fork()。原因在于，当使用写时复制语义实现 fork()（大部分现代 UNIX 实现皆是如此）时，在速度几近于 vfork()的同时，又避免了 vfork()的上述怪异行止

### 进程的终止
#### exit()和_exit()
exit()函数会进行系统调用_exit() 并且exit()会关闭当前进程的所有文件描述符  

### fork().stdio缓冲区以及_exit()之间的交互
```cpp
#include<stdio.h>
int main(int argc,char ** argv)
{
    printf("hello world");
    write(STDOUT_FILENO,"Ciao\n",5);
    if(fork()==-1)
        errExit("fork");
    exit(EXIT_SUCCESS) ;
}
```
上述代码在输出定向到终端时,会看到预计结果
```shell
$ ./test
hello world
Ciao
```
但重定向输出到文件时,结果如下
```shell
$ ./test > a
$ cat a
Ciao
hello world
hello world
```
> 要理解为什么 printf()的输出消息出现了两次，首先要记住，是在进程的用户空间内存中（参考 13.2 节）维护 stdio 缓冲区的。因此，通过 fork()创建子进程时会复制这些缓冲区。**当标准输出定向到终端时，因为缺省为行缓冲，所以会立即显示函数 printf()输出的包含换行符的字符串**。不过，**当标准输出重定向到文件时，由于缺省为块缓冲，所以在本例中，当调用 fork()时，printf()输出的字符串仍在父进程的 stdio 缓冲区中，并随子进程的创建而产生一份副本。父、子进程调用 exit()时会刷新各自的 stdio 缓冲区，从而导致重复的输出结果**  
- 采用以下方式来避免问题
    - 针对stdio缓冲区解决 调用fork()前使用函数fflush()来刷新缓冲区 或者使用setvbuf()与setbuf()来关闭stdio流的缓冲功能
    - 子进程调用_exit()系统调用 从而不再刷新stdio缓冲区  
        - 在创建子进程的应用中,典型情况下应仅有一个进程通过exit()函数终止,其他的子进程应通过_exit()终止,从而确保只有一个进程调用处理程序并刷新stdio缓冲区.

write()的输出并未出现两次 是因为write()调用会将数据直接传给内核缓冲区 在进行fork()时 不会复制这部分缓冲
write()的输出结果先于printf()而出现，是因为 write()会将数据立即传给内核高速缓存，而 printf()的输出则需要等到调用 exit ()刷新 stdio 缓冲区时

### 监控子进程
#### 等待子进程
##### 系统调用wait()
系统调用wait()等待调用进程的任一子进程终止,同时在参数status所指的缓冲区中返回该子进程的终止状态.
```c
#include<sys/wait.h>
pid_t wait(int *status);
```
- 执行如下操作
    - 调用进程无子进程终止,调用将一直阻塞,知道某个子进程exit,若调用时已有子进程终止,wait()即立即返回,
    - 如果status非空,那么关于子进程的信息会通过status返回
    - 内核会为父进程下所有子进程的运行总量追加进程的cpu时间以及资源使用数据
    - 将终止进程的ID作为wait()的结果返回

```c
while((childpid=wait(NULL))!=-1)
    continue;
if(errno!=ECHILD)
    errexit(errno);
```
如上代码将等待所有子进程退出后执行后续代码
##### 系统调用waitpid()
wait()调用有着诸多限制,设计waitpid()则避免了这些限制
1. 父进程已经创建了诸多子进程 ,wait将无法等待某个特定子进程的完成 ,只能顺序等待
2. 没有子进程退出,wait()总是保持阻塞. 有时会希望执行非阻塞的等待.
3. 使用wait()只能发现已经终止的子进程.对于子进程因某个信号 

 ```c
#include <sys/wait.h>
pid_t waitpid(pid_t pid,int *status,int options);
 ```
- pid 大于 0, 表示等待进程ID为pid的子进程
- pid 小于-1, 则会等待进程组标识符与pid绝对值相等的所有子进程
- pid 等于-1, 则等待任意子进程.
  - wait(&status)调用与waitpid(-1,&status,0)等价

其中options是一个位掩码,可以是0 也可以包含多个如下标识(*或*操作)

1. WNOHANG：如果没有任何已经结束的子进程则马上返回, 不予以等待.    
2. WUNTRACED：**如果子进程进入暂停执行情况则马上返回**, **但结束状态不予以理会**. 子进程的结束状态返回后存于status, 底下有几个宏可判别结束情况.    
3.  WNOHANG: 忽略子进程的返回状态 不阻塞;

- 一些宏
1.  WIFEXITED (status)：**如果子进程正常结束则为非0** 值.    
2. WEXITSTATUS(status)：取得子进程exit()返回的结束代码, 一般会先用WIFEXITED 来判断是否正常结束才能使用此宏.    
3. WIFSIGNALED(status)：**如果子进程是因为信号而结束则此宏值为真**.    
4.  WTERMSIG  (status)：取得子进程因信号而中止的信号代码, 一般会先用WIFSIGNALED 来判断后才使用此宏.   
5. WIFSTOPPED (status)：**如果子进程处于暂停执行情况则此宏值为真**. **一般只有使用WUNTRACED时才会有此情况**.    
6.  WSTOPSIG  (status)：取得引发子进程暂停的信号代码, 一般会先用WIFSTOPPED 来判断后才使用此宏.  

#### wait() 和waitpid() 的区别

- wait等待第一个终止的子进程，而waitpid可以通过pid参数指定等待哪一个子进程。当pid=-1、option=0时，waitpid函数等同于wait，可以把wait看作waitpid实现的特例。  
- waitpid函数提供了wait函数没有提供的三个功能：
    1. waitpid等待一个特定的进程，而wait则返回任一终止子进程的状态 。
    2. waitpid提供了一个 wait的非阻塞版本，有时希望取得一个子进程的状态， 但不想进程阻塞。
    3. waitpid支持作业控制。

##### 等待状态值
wait()和waitpid()返回的status的值,可以用来区分以下子进程.
- 子进程exit退出终止
- 子进程受到未处理信号而终止
- 子进程因受到信号而停止,并以WUNTRACED标识调用waitpid().
- 子进程因收到信号SIGCONT而恢复,并以WCONTINUED标志调用waitpid().  
上述所用情况都可以用 **"等待状态"**来涵盖, 而前两种则可以用 **"终止状态"**指代,


头文件<sys/wait.h>定义了下述宏 对各自wait()或waitpid()返回的status值进行处理时,以下列表中各宏会返回真值(只有一个)
- WIFEXITED(status)

  - 子进程正常结束则返回真(1).此时,宏 **WEXITSTATUS** 返回子进程的退出状态;

- WIFSIGNALED(status)

  - 若通过信号杀死子进程则返回真.此时,宏 WTERMSIG(status)返回导致子进程终止的信号编号,若子进程产生内核转储文件，则宏 WCOREDUMP(status)返回真值（true）

- WIFSTOPPED(status)

  - 若子进程因信号而停止,则此宏返回true.此时宏 WSTOPSIG(status)返回导致子的信号编号

- WIFCONTINUED(status)

  - 子进程收到SIGCONT而回复执行,则此宏返回真值true.


 **尽管上述宏的参数也以 status 命名，不过此处所指只是简单的整型变量，而非像wait()和 waitpid()所要求的那样是指向整型的指针**


```c
 #define _GUN_SOURCE
 #include <string.h>
 #include <sys/wait.h>
 void printWaitStatus(const char *msg, int status)
 {
     if (msg != NULL)
         printf("%s", msg);
     if (WIFEXITED(status))
     {
         printf("child exited ,status=%d\n", WEXITSTATUS(status));
     }
     else if (WIFSIGNALED(status))
     {
         printf("child killed by signal %d (%s)",WTERMSIG(status),strsignal(WTERMSIG(status));
 
 #ifdef WCOREDUMP
         if(WCOREDUMP(status))
             printf("(core dumped)");
 #endif
         printf("\n");
     }
     else if (WIFSIGNALED(status))
     {
         printf("child stopped by signal %d (%s)\n," WSTOPSIG(status), strsignal(WSTOPSIG(status)));
 #ifdef WIFCONTINUED
     }
     else if (WIFCONTINUED(status))
     {
         printf("child continue\n");
  #endif
     }
     else
     {
         printf("what happened to this child?(status=%x)\n", (unsigned int)status);
     }
}

 ```

#### 从信号处理程序中终止程序
默认情况下某些信号会终止进程,有时希望在进程终止前执行一些清理步骤. 为此可以处理程序来捕捉这些信号,随即执行清理步骤 但若这么做 父进程 依然可以通过wait()和waitpid()获得到子进程的终止状态 **如子程序获取信号后** **调用_exit** **则在父进程中监控的程序状态则为正常终止的**

如果需要通知父进程自己因某个信号而终止，那么子进程的信号处理程序应首先将自己废除，然后再次发出相同信号，该信号这次将终止这一子进程。信号处理程序需包含如下代码
```c
void handler(int sig)
{
    /* perform cleanup */
    signal(sig, SIG_DFL);
    raise(sig);
}
```

#### 僵尸进程
子进程若不被父进程wait()退出 
则会释放大部分进程资源,只保留进程表中的信息. 转化为僵尸进程 

若进行wait退出 则在进程表中的数据将会删除
若父进程被杀死 则同样在进程表中的数据会被删除

在一个父进程长期存在的程序中 
若存在大量僵尸进程 则会占满进程表 届时将无法创建进程

#### SIGCHLD建立信号处理程序
无论一个子进程于何时终止 系统都会向父进程发送SIGCHLD信号
我们可在父进程中针对SIGCHLD信号编写处理僵尸进程的信号处理程序

但当调用信号处理程序时，会暂时将引发调用的信号阻塞起来（除非为 sigaction()指定了 SA_NODEFER 标志），且不会对 SIGCHLD 之流的标准信号进行排队处理。SIGCHILD这样一来，当信号处理程序正在为一个终止的子进程运行时，如果相继有两个子进程终止，即使产生了两次 SIGCHLD 信号，父进程也只能捕获到一个

解决方案是在信号处理程序中内部循环以WNOHANG标志来调用waitpid(),直至再无其他终止的子进程需要处理为止

通常SIGCHLD处理程序都简单的由以下代码组成 **/仅捕获已终止子进程而不关心其退出状态**
`while(waitpid(-1,NULL,WNOHANG)>0);`

### 程序的执行
exec()族关系
|前4位|第五位  |第六位|
|-|--|--|
| **都为exec** | l:参数以列举的方式传递 |e:可以传递环境变量  |
| **都为exec**|v:参数以结构体指针的方式传递|p:可执行文件查找方式为文件名|
```cpp

int execl(const char *pathname, const char *arg, ...);

int execv(const char *pathname, char *const argv[]);

int execle(const char *pathname, const char *arg, ..., const char *envp[]);

int execlp(const char *filename, const char *arg, ...);

int execve(const char *pathname, char *const arg, ..., const char *envp[]);

int execvp(const char *filename, char *const argv[]);
// 各个函数的定义
// ... 是指以 "..." 列出的传入的arg参数
```
```cpp
// 举例
int main(int argc, char **argv)
{
    char *const ps_argv[] = {"ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL};

    char *const ps_envp[] = {"PATH=/bin:/usr/bin", "TERM=console", NULL};

    execl("/bin/ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);

    execv("/bin/ps", ps_argv);

    execle("/bin/ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL, ps_envp);

    execve("/bin/ps", ps_argv, ps_envp);

    execlp("ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);

    execvp("ps", ps_argv);
    
    execl("/home/wang/Desktop/object/test/", "-a", "-b", "--help",NULL);
}
```

- exec函数族使用注意点

在使用exec函数族时，一定要加上错误判断语句。因为exec很容易执行失败，其中最常见的原因有：

1. 找不到文件或路径，此时errno被设置为ENOENT。

2. 数组argv和envp忘记用NULL结束，此时errno被设置为EFAULT。

3. 没有对应可执行文件的运行权限，此时errno被设置为EACCES。

- exec后新进程保持原进程以下特征

        - 环境变量（使用了execle、execve函数则不继承环境变量）；
        - 进程ID和父进程ID；
        - 实际用户ID和实际组ID；
        - 附加组ID；
        - 进程组ID；
        - 会话ID；
        - 控制终端；
        - 当前工作目录；
        - 根目录；
        - 文件权限屏蔽字；
        - 文件锁；
        - 进程信号屏蔽；
        - 未决信号；
        - 资源限制；
        - tms_utime、tms_stime、tms_cutime以及tms_ustime值。

对打开文件的处理与每个描述符的exec关闭标志值有关，进程中每个文件描述符有一个exec关闭标志（FD_CLOEXEC），若此标志设置，则在执行exec时关闭该描述符，否则该描述符仍打开。除非特地用fcntl设置了该标志，否则系统的默认操作是在exec后仍保持这种描述符打开，利用这一点可以实现I/O重定向。

### 一个system()的自主实现
不进行信号屏蔽版**shell.0.0.2.c**
```c
int my_system(char *command)
{
    int status;
    pid_t childPid;
    switch (childPid = fork())
    {
    case /* constant-expression */ -1:
        /* code */ return -1;
        break;
    case 0:
        execl("/usr/bin/zsh", "zsh", "-c", command, NULL);
        _exit(127);
    default:
        if (waitpid(childPid, &status, 0) == -1)
            return -1;
        else
            return status;
    }
}
```

不进行信号屏蔽可能会引发竞争状态 下文有解释


进行信号屏蔽版**shell.0.0.2.c**
```c
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>

int my_system(const char *command)
{
    sigset_t blockMaxk, origMask;//声明信号集
    struct sigaction saIgnore, saOrigQuit, saOrigInt, saDefault;//声明sigaction结构体存放 先前的 目标的 信号处理程序
    pid_t childPid;
    int status, savedErrno;
    // 声明savedErrno 是存放先前的errno值 避免在执行system时 将errno的值修改

    if (command == NULL)   return system(":") == 0;

    sigemptyset(&blockMaxk);//将信号集初始化为空
    sigaddset(&blockMaxk, SIGCHLD);//将signo添加到信号集中
    sigprocmask(SIG_BLOCK, &blockMaxk, &origMask);
    //将原先的信号屏蔽字存储在origMask中 将blockMask中的信号设置为当前的信号屏蔽字


    saIgnore.sa_handler = SIG_IGN;//将saIgnore的信号处理程序设置为忽略信号
    saIgnore.sa_flags = 0;//信号掩码为0
    sigemptyset(&saIgnore.sa_mask);//将saIgnore的信号集合设置为空
    sigaction(SIGINT, &saIgnore, &saOrigInt);
    sigaction(SIGQUIT, &saIgnore, &saOrigInt);
    //将原来对信号的操作保存到 saOrinInt 结构体中
    //将新的信号处理程序改为 saIgnore 

    switch (childPid = fork())
    {
    case -1:
        status = -1;
        break;
    case 0:
        saDefault.sa_handler = SIG_DFL;
        saDefault.sa_flags = 0;
        sigemptyset(&saDefault.sa_mask);

        if (saOrigInt.sa_handler != SIG_IGN)
            sigaction(SIGINT, &saDefault, NULL);
        if (saOrigInt.sa_handler != SIG_IGN)
            sigaction(SIGQUIT, &saDefault, NULL);

        sigprocmask(SIG_SETMASK, &origMask, NULL);

        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(127);

    default:
        while (waitpid(childPid, &status, 0) == -1)
        {
            /* code */
            if (errno != EINTR)
            {
                status = -1;
                break;
            }
        }
        break;
    }
    savedErrno = errno;

    sigprocmask(SIG_SETMASK, &origMask, NULL);
    sigaction(SIGINT, &saOrigInt, NULL);
    sigaction(SIGQUIT, &saOrigInt, NULL);

    errno = savedErrno;

    return status;
}
```
#### 为什么要在执行system()函数时进行信号屏蔽
- **在system()内部正确处理信号**
给 system()的实现带来复杂性的是对信号的正确处理。

首先需要考虑的信号是 **SIGCHLD**。假设调用 system()的程序还直接创建了其他子进程，
对 SIGCHLD 的信号处理器自身也执行了 wait()。在这种情况下，当由 system()所创建的子进程退出并产生 SIGCHLD 信号时，在 system()有机会调用 waitpid()之前，主程序的信号处理器程序可能会率先得以执行（收集子进程的状态）。这是**竞争条件**的又一例证。这会产生两种不良后果。
- 调用程序会误以为其所创建的某个子进程终止了。
- system()函数却无法获取其所创建子进程的终止状态。
**所以，system()在运行期间必须阻塞 SIGCHLD 信号。**

简而言之 即system()也有可能创建子进程 其子进程返回时产生的**SIGCHLD**信号有可能会被调用system()的进程的信号信号处理程序所捕获 但system()同时也想捕获子进程的退出状态 
引发了 **竞争状态** 
所以 要进行信号**SIGCHLD**的屏蔽
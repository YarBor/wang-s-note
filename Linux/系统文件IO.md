## 系统文件I/O
所有执行I/O操作的系统调用都以文件描述符,来指代打开的文件.针对每个进程文件描述符都自成一套  
大多数程序都期望使用3种文件描述符  
文件描述符|用途|POSIX名称|stdio流
   -----|--- |  ---   |----
   1|标准输入|STDIN_FILENO|stdin
   2|标准输出|STDOUT_FILENO|stdout
   3|标准错误|STDERR_FILENO|stderr

在程序中指代这些文件描述符时,可以使用数字(012)表示,或采用<unistd.h>定义的标准名称  
### **执行文件 I/O 操作的主要系统调用**
> 编程语言和软件包通常会利用 I/O 函数库对它们进行间接调用  
**open()write()read()close()**<---主要的系统调用函数
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
*   1. **复制一个现有的描述符**(cmd=F_DUPFD)  
*    2. **获得/设置文件描述符标记**(cmd=F_GETFD或F_SETFD)
 *   3. **获得/设置文件状态标记**(cmd=F_GETFL或F_SETFL)
  *  4. **获得/异步I/O所有权**(cmd=F_GETOWN或F_SETOWN)
   * 5. **获得/设置记录锁**(cmd=F_GETLK,F_SETLK或F_SETLKW)
   

描述: **fcntl()针对文件描述符提供控制**,参数fd是被参数cmd操作的描述符;  
针对cmd的值,fcntl能接受第三个参数(arg)  

* int fcntl(int fd,int cmd, ... )  
``((#include<fcntl.h>)RETURN on success depends on cmd , or -1 on error) `` 
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
##### **dup/dup2函数**:  
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
dup2(oldfd, newfd) == close(newfd);fcntl(oldfd, F_DUPFD,newfd);

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
***
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
##### mkstemp()
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
##### tmpfile()
* tmpfile()函数会创建一个名称唯一的临时文件,并以读写进行打开(flag包含O_EXCL标志,以防另一个进程创建了一个同名文件)
```c
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

**mkstemp()和 tmpfile()函数允许应用程序去创建临时文件 用unlink来销毁**


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
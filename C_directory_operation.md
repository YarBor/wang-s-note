## 目录/文件相关

### 存储信息的结构体：

- `DIR* dp`: **某个目录的信息**

  头文件：`dirent.h`

  ```c
  struct __dirstream   
     {   
      void *__fd;                   // 文件描述符
      char *__data;                 // 目录块
      int __entry_data;             // data对应的条目号   
      char *__ptr;                  // 块中的当前指针
      int __entry_ptr;              // 条目号对应的指针  
      size_t __allocation;          // 为块分配的空间
      size_t __size;                // 块中的总有效数据
      __libc_lock_define (, __lock);//此结构的互斥锁
     };   
      
  typedef struct __dirstream DIR;  
  ```

- `struct dirent *entry`: **某个文件在目录里的简略信息**

  头文件：`dirent.h`

  ```c
  struct dirent
    {
      __ino_t d_ino;              // 文件的inode节点号
      __off_t d_off;              // 在目录文件中的偏移
      unsigned short int d_reclen;// 文件名字长度
      unsigned char d_type;       // 文件类型 
      char d_name[256];		    // 文件名字 
    };
  ```

- `stat`结构体 : **某个文件的详细信息**

  ```c
  struct stat {   
      mode_t st_mode;      // 文件类型、访问权限   
      ino_t st_ino;        // 索引节点号   
      dev_t st_dev;        // 文件使用的设备号   
      dev_t st_rdev;       // 设备文件的设备号   
      nlink_t st_nlink;    // 文件的硬连接数    
      uid_t st_uid;        // 所有者用户识别号   
      gid_t st_gid;        // 组识别号   
      off_t st_size;       // 以字节为单位的文件容量   
      time_t st_atime;     // 最后一次访问该文件的时间   
      time_t st_mtime;     // 最后一次修改该文件的时间   
      time_t st_ctime;     // 最后一次改变该文件状态的时间   
      blksize_t st_blksize;// 文件内容所占的磁盘块的大小   
      blkcnt_t st_blocks;  // 文件内容所占的磁盘块的数量  
  };  
  ```
  ### 相关函数：

- stat 与 lstat函数`chdir()`

    用途：`int chdir(const char * path)`        

    切换到参数指定的目录，成功返回0 , 失败返回-1 , 置errno

- `getcwd()`
	
	```c     	
	#include <unistd.h>
	char *getcwd(char *buf, size_t size);
	```
	用途：将当前目录的绝对路径拷贝到参数`buffer`所指的空间，参数`size`为`buf`所指的空间大小。`bu`f为`NULL`，`size`为0时，`getcwd`调用`malloc`来动态分配内存一个空间存储绝对路径，返回指向这个空间的指针，此时要注意后面释放`buf`内存，防止内存泄漏。成功返回指向绝对路径的指针，失败返回`NULL`，置`errno`。
	
- mkdir()

    ```c
    #include <sys/types.h> 
    #include <sys/stat.h>
    int mkdir(const char *pathname, mode_t mode) 
    ```

    用途：在当前目录下按照mode指定的权限创建名为`pathname`的目录，成功返回0，失败返回-1，置`errno`

    mode简述（不全）：使用掩码，后三位数字从左往右依次表示文件所有者权限、用户组权限、其他用户权限。1：只执行，2：只写入，4：只可读，7：读、写、执行，3：执行+写，5：读+执行，依次类推。可以只写后三位数字。

- rmdir()
  ```c
  #include <unistd.h>
  int rmdir(const char *pathname)
  ```
    用途：在当前目录下删除名为`pathname`的空目录，成功返回0，失败返回-1，置`errno`

- `opendir()`                    
	
    ```c	
    #include <sys/types.h>
    #include <dirent.h>
    DIR *opendir(const char *name);
	DIR *fdopendir(int fd);
    ```
	用途：打开参数name或fd指定的目录，创建一个目录流。成功返回一个`DIR`指针，失败返回`NULL`，置`errno`
	
- `readdir()`
  ```c
  include <dirent.h>
  struct dirent*readdir(DIR *dirp)
  ```
  用途：读取参数dirp指定的目录流，成功返回指向目录内当前偏移量指向的文件的简略信息的`dirent`结构指针，失败或者到达目录尾则返回`NULL`

- `closedir()`
  
  ```c
  #include <sys/types.h>
  #include <dirent.h>
  int closedir(DIR *dirp);
  ```
  用途：关闭参数dirp指定的目录流，成功返回0，失败返回-1，置ereno
  
- `stat() fstat() lstat()` 
  
  ```c
  #include <sys/stat.h>  
  #include <unistd.h>
  int stat(const char *path, struct stat *struct_stat);
  int lstat(const char *path,struct stat *struct_stat);
  int fstat(int fdp, struct stat *struct_stat);
  ```
  用途：通过文件名path或者文件描述符fdp获取文件详细信息，将信息保存在struct_stat结构体中，成功返回0,失败返回-1并，置errno。
  
  区别：stat没有处理字符链接(软链接）的能力，如果一个文件是符号链接(软链接)，stat会直接返回它所指向的文件的属性；而lstat返回的就是这个符号链接的内容。目录在linux中也是一个文件，文件的内容就是这这个目录下面所有文件与inode的对应关系。硬链接就是在某一个目录下面将一个文件名与一个inode关联起来，就是添加一条记录。软链接也叫符号链接，这个文件的内容就是一个字符串，这个字符串就是它所链接的文件的绝对或者相对地址。
































---

### $ C_directory_operation
`#include<dirent.h>`
代表了目录流的类型 
```c
struct dirent {
ino_t d_ino;             /* inode number 索引节点号 */ 
off_t d_off;             /* offset to this dirent 在目录文件中的偏移 */  
unsigned short d_reclen; /* length of this d_name 文件名长 */  
unsigned char d_type;    /* the type of d_name 文件类型 */  
char d_name[256];        /* file name (null-terminated) 文件名，最长255字符 */  
};
```
#### 函数目录:>
```c
int closedir(DIR *dir);
DIR *opendir(const char *path);
struct dirent *readdir(DIR *dir);
int readdir(DIR *dir);
int readdir_r(DIR *dir,struct dirent *,struct dirent **);
void rewinddir(DIR *dir);
void seekdir(DIR *dir,long int);
long int telldir(DIR *dir);
```
##### closedir():
closedir();-关闭一个目录流 
* ***int closedir ( DIR \* dirp*** **)**;
closedir()函数关闭由参数dirp引用的目录流. 返回时, dirp的值 可能不再指向DIR类型的可访问对象.如果文件描述符用于实现类型DIR,则该文件描述符将被关闭. 成功完成后closedir()将返回 0 否则返回 -1 并设置 error错误;  
* **for error**
    * 在dirp参数不引用打开的目录流 ? 
        * The dirp argument does not refer to an open directory stream.
    * closedir() 函数被信号中断 。 
        * The closedir() function was interrupted by a signal.

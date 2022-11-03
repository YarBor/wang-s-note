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
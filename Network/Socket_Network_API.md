### 在TCP传输中，socket的状态可以分为以下几种：


- `CLOSED`：初始状态，表示套接字未被打开或已关闭。

- `LISTEN`：表示套接字正在监听传入的连接请求。

- `SYN_SENT`：表示套接字已经向远程主机发送连接请求，并等待远程主机的响应。

- `SYN_RECEIVED`：表示套接字已经接收到远程主机的连接请求，并向远程主机发送确认响应。

- `ESTABLISHED`：表示套接字已经建立连接，可以进行数据传输。

- `CLOSE_WAIT`：表示套接字已经接收到远程主机的结束连接请求，但本地应用程序还没有结束连接。

- `FIN_WAIT_1`：表示套接字已经向远程主机发送结束连接请求，等待远程主机的确认响应。

- `CLOSING`：表示套接字同时接收到远程主机的结束连接请求和向远程主机发送结束连接请求，等待远程主机的确认响应。

- `FIN_WAIT_2`：表示套接字已经接收到远程主机的确认响应，但还没有收到远程主机的结束连接请求。

- `TIME_WAIT`：表示套接字已经发送结束连接请求，并接收到远程主机的确认响应，但仍然保持连接状态，等待一段时间后才关闭套接字。

- `LAST_ACK`：表示套接字已经接收到远程主机的结束连接请求的确认响应，即将关闭套接字。

在以上状态中，`CLOSED、LISTEN`和`ESTABLISHED`是TCP连接的三个基本状态，其他状态则是连接的过渡状态。


### IPv4数据报
IPv4首部（IPv4 header）是IPv4协议数据报中包含的固定长度的首部，用于在网络中传输IPv4数据报。IPv4首部包含了一系列字段，用于描述数据报的源地址、目的地址、长度、协议等信息，具体包括以下几个部分：

---

- 版本号（Version）：占4位，表示IP协议版本号，IPv4的版本号为4。

- 首部长度（Header Length）：占4位，表示IPv4首部长度，以32位字长为单位，最小值为5，最大值为15，因此IPv4首部长度的取值范围为20-60个字节。

- 区分服务（Differentiated Services）：占8位，用于区分不同的服务类别，实现不同的服务质量要求，如QoS。

- 总长度（Total Length）：占16位，表示整个IPv4数据报的长度，包括IPv4首部和数据部分，以字节为单位。

- 标识（Identification）：占16位，标识该数据报的唯一性，以便在分片和重组时使用。

- 标志（Flags）：占3位，用于控制数据报的分片和重组。

- 片偏移（Fragment Offset）：占13位，表示该分片相对于原始数据报开始处的偏移量，以8字节为单位。

- 生存时间（Time to Live，TTL）：占8位，用于避免数据报在因路由循环或转发故障而无限期地在网络中传输，达到一定的跳数（TTL减1）后数据报将被丢弃。

- 协议（Protocol）：占8位，指示数据报中的数据部分所使用的协议类型，如TCP、UDP、ICMP等。

- 首部校验和（Header Checksum）：占16位，用于检验IPv4首部的正确性，不包括数据部分。

- 源地址（Source Address）：占32位，表示IPv4数据报的源地址。

- 目的地址（Destination Address）：占32位，表示IPv4数据报的目的地址。

- 选项（Options）：可变长，可选项，用于提供额外的协议信息，如时间戳、安全、路由等。


IPv4首部中的各字段提供了关于IP数据报的重要信息，如源和目的地址、数据报长度、数据报类型等，这些信息对于路由、分组和重组等网络操作至关重要。同时，IPv4首部还提供了一定的可选项，如选项字段，用于提供更多的协议信息和增强协议功能，提高网络性能和安全性。

> - socket状态::
> 在TCP/IP协议中，socket有以下几种状态：
> 
> - `CLOSED`：初始状态，表示socket未被打开或已关闭。
> - `LISTEN`：监听状态，表示socket正在监听来自客户端的连接请求。
> - `SYN-SENT`：表示socket已经向对方发送了SYN包，等待对方的确认。
> - `SYN-RECEIVED`：表示socket已经收到了对方的SYN包，需要发送确认。
> - `ESTABLISHED`：连接已经建立，双方可以进行通信。
> - `FIN-WAIT-1`：表示socket已经向对方发送了FIN包，等待对方的ACK确认。
> - `FIN-WAIT-2`：表示socket已经收到了对方的ACK确认，等待对方发送FIN包。
> - `CLOSE-WAIT`：表示socket已经收到对方的FIN包，并发送了ACK确认，等待上层应用关闭socket。
> - `CLOSING`：表示socket同时收到了对方的FIN包和ACK确认，等待发送自己的FIN包。
> - `LAST-ACK`：表示socket已经发送了FIN包，并收到了对方的ACK确认，等待对方的FIN包。
> - `TIME-WAIT`：表示socket已经发送了FIN包和ACK确认，并等待2MSL时间，等待对方的FIN包确认。
> - `DELETE-TCB`：表示socket已经完成关闭，TCB（传输控制块）已经被删除。
> 
> 这些状态是TCP协议中的状态，而UDP协议是无连接的，不存在类似的状态。

<!-- 192.168.30.175 本地IP -->
 
## 套接字编程简介

- IPV4地址结构
``` cpp
#include <netinet/in.h>
struct in_addr {
    in_addr_t s_addr; 
                                        //这二者在进行set时需要进行htonl()调用
    // 32位的 IPV4 的地址编码 
    // typedef in_addr_t uint32_t;
    // 网络序列
}
struct sockaddr_in {
    uint8_t         sin_len;    // 结构体 长度 （16）
    sa_family_t     sin_family; // 套接字地址结构的地址族

    in_port_t       sin_port; // 16bity tcp/udp 端口
                                        //这二者在进行set时需要进行htons()调用
    
    struct in_addr  sin_addr; // 32位 ipv4 地址      

    char            sin_zero[8];// unused
};
```

- 通用地址结构
```c
#include <sys/socket.h>
struct sockaddr{
    uint8_t     sa_len; 
    sa_family_t sa_family;      // 标识码 AF_**** value    
    char        sa_data[16];    // 用于字节对齐  unused    
}

定义这种结构目的是 无论IP4/6 

//通用函数
int bind(int ,struct sockaddr *,socklen_t);

调用时 可以通过 sa_family_t 字段识别相应地址结构

//调用
bind(  ,(struct sockaddr *) &(sockaddr_in__Value),  );
```

### 字节排序函数

网络协议用大端进行存储  计算机用小端进行存储
在进行网络通信时 
要把小端字节序 转换为大端字节序

函数：》
```c
#include<netinet/in.h>

uint16_t htons (uint16_t host16bitValue);
uint32_t htonl (uint32_t host32bitValue);
// host to net short ./. long
小转大 

uint16_t ntohs (uint16_t net16bitValue);
uint32_t ntohl (uint32_t net32bitValue);
// net to host short ./. long
大转小

```

### 字节操纵函数
```c
#include <strings.h>

void bzero(void * dest,size_t size);
// == memset(  void * dest , 0 , size_t size  )
void bcopy(const void * src,void * dest,size_t size);
// == memcpy(void * dest ,const void * src,size_t size);
int bcmp(const void * ptr1,const void * ptr2,size_t size);
// == strncmp(char* a,char* b ,size_t size);

```
bzero() 通常将某个套接字的内存设为0;
bcopy() 将指定数目的字节从原串复制到该串
bcmp() 比较


### inet_pton() inet_ntop()
地址转换函数
```c
#include <arpa/inet.h>
int inet_pton(int family,const char* strptr,void *addrptr);
//family 可以是 AF_INET / AF_INET6    若不是设定的family参数 则返回err 将errno设置为"EAFNOSUPPORT"
//如果 strptr 对family 不是有效的字串 返回0 

const char * inet_ntop(int family,const void* addrptr,char * strptr,size_t size);
//同理 
//此处多出来的参数size 表述 strptr (必须malloc)的buffer大小
```

### 绑定函数
```c
#include <sys/socket.h>
int bind(int soktfd,const struct sockaddr * address,size_t addrlen);

```
通过bind()调用进行绑定socket 
在sockaddr「通用」结构体中 通过 family字段进行识别IP4还是IP6
然后进行强制类型转换

转换后 会进行绑定 「IP和端口」若未设置IP地址/端口 则内核进行选择
> 一般的 ： 服务端需要自行绑定 因为客户端需要一个众所周知的IP地址和端口进行和服务端通信
         客户端则可以使用内核随机指定的IP和端口


在IP端口进行赋值时 需要调用htonl()转换字节序
```c
servaddr.sin_addr.s_addr = htonl(value);
```


### 获得socket信息

在随机分配IP,端口后 通过`getsockname()`拿到目标套接字的信息
```c

#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  // 创建套接字
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    return -1;
  }

  // 准备地址信息并将套接字绑定
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(4646);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  int bind_result = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
  if (bind_result == -1) {
    std::cerr << "Failed to bind socket to address" << std::endl;
    close(server_socket);
    return -1;
  }

  // 获取本地地址信息并打印
  struct sockaddr_in local_address;
  socklen_t address_length = sizeof(local_address);

  int getsockname_result = getsockname(server_socket, (struct sockaddr*)&local_address, &address_length);
        // 通过getsockname 拿到刚刚随机分配的IP信息
    
  if (getsockname_result == -1) {
    std::cerr << "Failed to get socket name" << std::endl;
    close(server_socket);
    return -1;
  }
  char local_address_string[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(local_address.sin_addr), local_address_string, INET_ADDRSTRLEN);
  std::cout << "Local address: " << local_address_string << ", port: " << ntohs(local_address.sin_port) << std::endl;

  // 其他操作...

  return 0;
}

```
### 两端进行tcp链接
```cpp
int main(int argc,char** argv)
{

  int listenfd = socket(AF_INET, SOCK_STREAM,0);
  
  // set socket address
  struct sockaddr_in sever_addr;
  sever_addr.sin_fimaly = AF_INET;
  sever_addr.sin_port = htons(8080);
  sever_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

  bind(listenfd,(struct sockaddr*)&sever_addr,sizeof(sever_addr));

  listen(listenfd, 10 /*number of backlog*/);


/// 获取客户端的ip地址

  struct sockaddr_in client;
  int skfd = accept(listenfd,(struct sockaddr*)client,sizeof(client));

/// 忽略客户端的ip地址

  int skfd = accept(listenfd,null,null);

////针对skfd 的 code IO

  close (skfd);
}
```

在服务端进行tcp链接需要
> // 返回一个新的skfd 文件描述符

socket(int domain,int type,int protocol)

> 声明 server地址

bind(int skfd,struct sockaddr* addr,socklen len);

> //进行绑定

listen(int skfd,int backlog)

> //将套接字转变为被动套接字 第二个参数是传入的请求队列的最大长度 
默认值是128

accept(int listenfd,null,null) 或 accept(int listenfd,struct sockaddr * addr,socklen_t)

> 返回新的套接字文件描述符 
至此 
就可以用accept返回的文件描述符进行和对等套接字进行IO了

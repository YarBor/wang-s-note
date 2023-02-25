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

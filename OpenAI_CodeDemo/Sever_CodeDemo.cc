#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./client <server_ip> <server_port>" << std::endl;
        return -1;
    }

    std::string server_ip = argv[1];
    int server_port = std::stoi(argv[2]);

    // 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket!" << std::endl;
        return -1;
    }

    // 设置服务器地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr.sin_port = htons(server_port);

    // 连接服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect server!" << std::endl;
        close(sockfd);
        return -1;
    }

    // 发送数据
    std::string message = "Hello, server!";
    if (send(sockfd, message.c_str(), message.length(), 0) < 0) {
        std::cerr << "Failed to send message!" << std::endl;
        close(sockfd);
        return -1;
    }

    // 关闭连接
    close(sockfd);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int connfd = accept(sockfd, NULL, NULL);
        if (connfd < 0) {
            perror("accept failed");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            close(connfd);
            continue;
        } else if (pid == 0) {
            close(sockfd);  // close old listening socket
            // handle client request using new socket
            char buffer[1024];
            int n = read(connfd, buffer, 1023);
            buffer[n] = '\0';
            printf("Received message: %s\n", buffer);
            write(connfd, "Hello from server\n", 18);
            close(connfd);
            exit(EXIT_SUCCESS);
        } else {
            close(connfd);  // close the new connected socket in parent process
        }
    }

    close(sockfd);
    return 0;
}

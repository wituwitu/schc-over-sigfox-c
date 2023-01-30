#include <netinet/in.h>
#include <stdio.h>

int main(int argc, char const* argv[]) {
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if ((client_fd = connect(
            sock,
            (struct sockaddr*) &serv_addr,
                    sizeof(serv_addr)
            )) < 0) {
        printf("\nConnection failed\n");
        return -1;
    }

    send(sock, hello, strlen(hello))

}
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "sigfox_socket.h"

void sgfx_client_start(SigfoxClient* client) {
    struct sockaddr_in serv_addr;
    int sock_fd;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    client->sock_fd = sock_fd;
    client->expects_ack = 0;
    client->timeout = 60;
    client->seqnum = 420;
    strncpy(client->buffer, "", DOWNLINK_MTU);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if ((client->server_fd = connect(
            client->sock_fd,
            (struct sockaddr*) &serv_addr,
            sizeof(serv_addr)
            )) < 0) {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }
}

ssize_t sgfx_client_send(SigfoxClient* client, const void* buf) {
    ssize_t ret;

    client->seqnum++;
    ret = send(client->sock_fd, buf, UPLINK_MTU, 0);

    if (client->expects_ack == 1) {
        ret = read(client->sock_fd, client->buffer, DOWNLINK_MTU);
    }
    return ret;
}

ssize_t sgfx_client_recv(SigfoxClient* client, char buf[]) {
    strcpy(buf, client->buffer);
    return (ssize_t) strlen(buf);
}

void sgfx_client_set_reception(SigfoxClient* client, int flag) {
    client->expects_ack = flag;
}

void sgfx_client_set_timeout(SigfoxClient* client, float timeout) {

    int sec = (int) timeout;
    int usec = 1000 * (int) (timeout - (float) sec);

    struct timeval timestr = {
            .tv_sec = sec,
            .tv_usec = usec
    };

    if (setsockopt(
            client->sock_fd,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timestr,
            sizeof(timestr)
            ) < 0) {
        perror("Set timeout failed\n");
        exit(EXIT_FAILURE);
    }
}

void sgfx_client_close(SigfoxClient* client) {
    close(client->server_fd);
}

void sgfx_server_start(SigfoxServer* server) {
    struct sockaddr_in serv_addr;
    size_t addrlen = sizeof(serv_addr);
    int sock_fd, opt = 1;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(
            sock_fd,
            SOL_SOCKET,
            SO_REUSEADDR | SO_REUSEPORT,
            &opt,
            sizeof(opt)
            )) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server->sock_fd = sock_fd;

    struct timeval tv;
    tv.tv_sec = 60;
    tv.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof tv);

    server->timeout = 60;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(
            server->sock_fd,
            (struct sockaddr*) &serv_addr,
            sizeof(serv_addr)
            ) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server->sock_fd, 5) < 0) {
        perror("Listen faied");
        exit(EXIT_FAILURE);
    }

    printf("Ola\n");

    if ((server->client_fd = accept(
            server->sock_fd,
            (struct sockaddr*) &serv_addr,
            (socklen_t*) &addrlen
            )) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
}

ssize_t sgfx_server_send(SigfoxServer* server, const void* buf) {
    return send(server->client_fd, buf, DOWNLINK_MTU, 0);
}

ssize_t sgfx_server_recv(SigfoxServer* server, char buf[]) {
    return read(server->client_fd, buf, UPLINK_MTU);
}

void sgfx_server_set_timeout(SigfoxServer* server, float timeout) {

    int sec = (int) timeout;
    int usec = 1000 * (int) (timeout - (float) sec);

    struct timeval tv = {
            .tv_sec = sec,
            .tv_usec = usec
    };

    if (setsockopt(
            server->sock_fd,
            SOL_SOCKET,
            SO_RCVTIMEO,
            (struct timeval*) &tv,
            sizeof(struct timeval)
            ) < 0) {
        perror("Set timeout failed\n");
        exit(EXIT_FAILURE);
    }

    server->timeout = timeout;
}

void sgfx_server_close(SigfoxServer* server) {
    close(server->client_fd);
    shutdown(server->sock_fd, SHUT_RDWR);
}
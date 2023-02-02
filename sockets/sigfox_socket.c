//
// Created by witu on 02-02-23.
//

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "sigfox_socket.h"

void sgfx_client_setup(SigfoxClient client) {
    struct sockaddr_in serv_addr;
    int sock_fd;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    client.sock_fd = sock_fd;
    client.expects_ack = 0;
    client.seqnum = 0;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if ((client.server_fd = connect(
            client.sock_fd,
            (struct sockaddr*) &serv_addr,
            sizeof(serv_addr)
            )) < 0) {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }
}

void sgfx_client_send(SigfoxClient client, const void* buf) {

    client.seqnum++;
    send(client.sock_fd, buf, strlen(buf), 0);

    if (client.expects_ack == 1) {
        read(client.sock_fd, client.buffer, 8);
    }
}

void sgfx_client_recv(SigfoxClient client, char buf[]) {
    strcpy(buf, client.buffer);
}

void sgfx_client_set_reception(SigfoxClient client, int flag) {
    client.expects_ack = flag;
}

void sgfx_client_set_timeout(SigfoxClient client, float timeout) {

    int sec = (int) timeout;
    int usec = 1000 * (int) (timeout - (float) sec);

    struct timeval timestr = {
            .tv_sec = sec,
            .tv_usec = usec
    };

    if (setsockopt(
            client.sock_fd,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timestr,
            sizeof(timestr)
            ) < 0) {
        perror("Set timeout failed\n");
        exit(EXIT_FAILURE);
    }
}

void sgfx_server_setup(SigfoxServer server) {
    struct sockaddr_in serv_addr;
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

    server.sock_fd = sock_fd;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server.sock_fd,
             (struct sockaddr*) &serv_addr,
             sizeof(serv_addr)
             ) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server.sock_fd, 5) < 0) {
        perror("Listen faied");
        exit(EXIT_FAILURE);
    }

    if ((server.client_fd = accept(
            server.sock_fd,
            (struct sockaddr*) &serv_addr,
            (socklen_t*) &serv_addr
            )) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
}

void sgfx_server_send(SigfoxServer server, const void* buf) {
    send(server.client_fd, buf, strlen(buf), 0);
}

void sgfx_server_recv(SigfoxServer server, char buf[]) {
    read(server.client_fd, buf, 12);
}

void sgfx_server_set_timeout(SigfoxServer server, float timeout) {

    int sec = (int) timeout;
    int usec = 1000 * (int) (timeout - (float) sec);

    struct timeval timestr = {
            .tv_sec = sec,
            .tv_usec = usec
    };

    if (setsockopt(
            server.sock_fd,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timestr,
            sizeof(timestr)
            ) < 0) {
        perror("Set timeout failed\n");
        exit(EXIT_FAILURE);
    }
}
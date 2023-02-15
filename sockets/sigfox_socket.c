#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/param.h>
#include "sigfox_socket.h"

void sgfx_client_start(SigfoxClient *client) {
    int sock_fd;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    client->sock_fd = sock_fd;
    client->expects_ack = 0;
    client->timeout = 60;
    client->seqnum = 0;
    strncpy(client->buffer, "", DOWNLINK_MTU);

    client->serv_addr.sin_family = AF_INET;
    client->serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(client->serv_addr).sin_addr);

    sgfx_client_set_timeout(client, 60);
}

ssize_t sgfx_client_send(SigfoxClient *client, const char sendbuf[]) {
    ssize_t sendval, readval = 0;

    client->seqnum++;
    sendval = sendto(
            client->sock_fd,
            sendbuf,
            MIN((int) strlen(sendbuf), UPLINK_MTU),
            0,
            (const struct sockaddr *) &(client->serv_addr),
            sizeof(client->serv_addr)
            );

    if (client->expects_ack == 1) {
        socklen_t len;
        readval = recvfrom(
                client->sock_fd,
                client->buffer,
                DOWNLINK_MTU,
                0,
                (struct sockaddr *) &(client->serv_addr),
                &len
                );
        client->buffer[DOWNLINK_MTU] = '\0';
    }

    if (sendval == -1 || readval == -1) return -1;

    return sendval;
}

ssize_t sgfx_client_recv(SigfoxClient *client, char recvbuf[]) {
    strncpy(recvbuf, client->buffer, sizeof(client->buffer));
    strncpy(client->buffer, "", DOWNLINK_MTU);
    return (ssize_t) strlen(recvbuf);
}

void sgfx_client_set_reception(SigfoxClient *client, const int flag) {
    client->expects_ack = flag;
}

void sgfx_client_set_timeout(SigfoxClient *client, const float timeout) {
    int sec = (int) timeout;
    int usec = 1000 * (int) (timeout - (float) sec);

    struct timeval tv = {
            .tv_sec = sec,
            .tv_usec = usec
    };

    if (setsockopt(
            client->sock_fd,
            SOL_SOCKET,
            SO_RCVTIMEO,
            (struct timeval*) &tv,
            sizeof(struct timeval)
            ) < 0) {
        perror("Set timeout failed\n");
        exit(EXIT_FAILURE);
    }

    client->timeout = timeout;
}

void sgfx_client_close(SigfoxClient *client) {
    close(client->sock_fd);
}

void sgfx_server_start(SigfoxServer *server) {
    struct sockaddr_in serv_addr;
    size_t addrlen = sizeof(serv_addr);
    int sock_fd, opt = 1;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

    sgfx_server_set_timeout(server, 60);
    server->timeout = 60;
}

ssize_t sgfx_server_send(SigfoxServer *server, const char buf[]) {
    return sendto(
            server->sock_fd,
            buf,
            DOWNLINK_MTU,
            0,
            (const struct sockaddr *) &(server->cli_addr),
            sizeof(server->cli_addr)
            );
}

ssize_t sgfx_server_recv(SigfoxServer *server, char buf[]) {
    memset(buf, '\0', UPLINK_MTU);
    socklen_t len = sizeof(server->cli_addr);
    return recvfrom(
            server->sock_fd,
            buf,
            UPLINK_MTU,
            0,
            (struct sockaddr *) &(server->cli_addr),
            &len
            );
}

void sgfx_server_set_timeout(SigfoxServer *server, const float timeout) {
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

void sgfx_server_close(SigfoxServer *server) {
    shutdown(server->sock_fd, SHUT_RDWR);
}
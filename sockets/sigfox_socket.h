#ifndef SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H
#define SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H

#endif //SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H

#define PORT 5000
#define UPLINK_MTU 12
#define DOWNLINK_MTU 8

typedef struct {
    int sock_fd;
    int server_fd;
    int expects_ack;
    int seqnum;
    float timeout;
    char buffer[DOWNLINK_MTU + 1];
} SigfoxClient;

typedef struct {
    int sock_fd;
    int client_fd;
    float timeout;
} SigfoxServer;

void sgfx_client_start(SigfoxClient* client);
ssize_t sgfx_client_send(SigfoxClient* client, const char sendbuf[]);
ssize_t sgfx_client_recv(SigfoxClient* client, char recvbuf[]);
void sgfx_client_set_reception(SigfoxClient* client, int flag);
void sgfx_client_set_timeout(SigfoxClient* client, float timeout);
void sgfx_client_close(SigfoxClient* client);

void sgfx_server_start(SigfoxServer* server);
ssize_t sgfx_server_send(SigfoxServer* server, const char buf[]);
ssize_t sgfx_server_recv(SigfoxServer* server, char buf[]);
void sgfx_server_set_timeout(SigfoxServer* server, float timeout);
void sgfx_server_close(SigfoxServer* server);



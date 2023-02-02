#ifndef SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H
#define SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H

#endif //SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H

#define PORT 5000

typedef struct {
    int sock_fd;
    int server_fd;
    int expects_ack;
    int seqnum;
    int timeout;
    char buffer[8];
} SigfoxClient;

typedef struct {
    int sock_fd;
    int client_fd;
    int timeout;
    char buffer[12];
} SigfoxServer;

void sgfx_client_start(SigfoxClient* client);
void sgfx_client_send(SigfoxClient* client, const void* buf);
void sgfx_client_recv(SigfoxClient* client, char buf[]);
void sgfx_client_set_reception(SigfoxClient* client, int flag);
void sgfx_client_set_timeout(SigfoxClient* client, float timeout);
void sgfx_client_close(SigfoxClient* client);

void sgfx_server_start(SigfoxServer* server);
void sgfx_server_send(SigfoxServer* server, const void* buf);
void sgfx_server_recv(SigfoxServer* server, char buf[]);
void sgfx_server_set_timeout(SigfoxServer* server, float timeout);
void sgfx_server_close(SigfoxServer* server);



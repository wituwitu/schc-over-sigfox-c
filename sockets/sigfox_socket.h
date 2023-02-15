#include <netinet/in.h>

#ifndef SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H
#define SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H

#endif //SCHC_OVER_SIGFOX_C_SIGFOX_SOCKET_H

#define PORT 5000
#define UPLINK_MTU 12
#define DOWNLINK_MTU 8

typedef struct {
    int sock_fd;
    int expects_ack;
    int seqnum;
    float timeout;
    char buffer[DOWNLINK_MTU + 1];
    struct sockaddr_in serv_addr;
} SigfoxClient;

typedef struct {
    int sock_fd;
    float timeout;
    struct sockaddr_in cli_addr;
} SigfoxServer;

/*
 * Function:  sgfx_client_start
 * --------------------
 * Initializes an emulated Sigfox client socket, connecting it to a
 * previously initialized server (via sgfx_server_start()).
 *
 *  client: SigfoxClient struct address containing socket data.
 */
void sgfx_client_start(SigfoxClient *client);

/*
 * Function:  sgfx_client_send
 * --------------------
 * Sends an array of chars toward the server socket.
 * If the expects_ack flag of the client socket is set to 1, it also receives
 * a message from the server.
 *
 * Returns the number of bytes sent or -1 for errors, such as read timeout if
 * a timeout is set and the reception flag is set to 1.
 *
 *  client: SigfoxClient struct address containing socket data.
 *  buf: Array of chars to send.
 */
ssize_t sgfx_client_send(SigfoxClient *client, const char buf[]);

/*
 * Function:  sgfx_client_recv
 * --------------------
 * Gets information from the client reception buffer.
 *
 * Returns the number of bytes received sent.
 *
 *  client: SigfoxClient struct address containing socket data.
 *  buf: Array of chars where to store the data.
 */
ssize_t sgfx_client_recv(SigfoxClient *client, char buf[]);

/*
 * Function:  sgfx_client_set_reception
 * --------------------
 * Configures the reception flag of the client socket.
 *
 *  client: SigfoxClient struct address containing socket data.
 *  flag: 0 (disable reception) or 1 (enable reception).
 */
void sgfx_client_set_reception(SigfoxClient *client, int flag);

/*
 * Function:  sgfx_client_set_timeout
 * --------------------
 * Configures the read timeout of the client socket.
 *
 *  client: SigfoxClient struct address containing socket data.
 *  timeout: amount of time to wait during recv().
 */
void sgfx_client_set_timeout(SigfoxClient *client, float timeout);

/*
 * Function:  sgfx_client_close
 * --------------------
 * Closes the client socket.
 *
 *  client: SigfoxClient struct address containing socket data.
 */
void sgfx_client_close(SigfoxClient *client);

/*
 * Function:  sgfx_server_start
 * --------------------
 * Initializes an emulated Sigfox server socket, which accepts an incoming
 * emulated Sigfox client connection (initialized via sgfx_client_start()).
 *
 *  server: SigfoxServer struct address containing socket data.
 */
void sgfx_server_start(SigfoxServer *server);

/*
 * Function:  sgfx_server_send
 * --------------------
 * Sends an array of chars toward the client socket.
 *
 * Returns the number of bytes sent or -1 for errors.
 *
 *  server: SigfoxServer struct address containing socket data.
 *  buf: Array of chars to send.
 */
ssize_t sgfx_server_send(SigfoxServer *server, const char buf[]);

/*
 * Function:  sgfx_server_recv
 * --------------------
 * Receives information from the client socket.
 *
 * Returns the number of bytes received sent or -1 for errors, such as
 * read timeout.
 *
 *  server: SigfoxServer struct address containing socket data.
 *  buf: Array of chars where to store the data.
 */
ssize_t sgfx_server_recv(SigfoxServer *server, char buf[]);

/*
 * Function:  sgfx_client_set_timeout
 * --------------------
 * Configures the read timeout of the server socket.
 *
 *  client: SigfoxServer struct address containing socket data.
 *  timeout: amount of time to wait during recv().
 */
void sgfx_server_set_timeout(SigfoxServer *server, float timeout);

/*
 * Function:  sgfx_server_close
 * --------------------
 * Closes the server socket.
 *
 *  client: SigfoxServer struct address containing socket data.
 */
void sgfx_server_close(SigfoxServer *server);
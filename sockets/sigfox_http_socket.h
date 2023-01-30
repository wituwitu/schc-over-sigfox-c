//
// Created by witu on 1/30/23.
//

#ifndef SCHC_OVER_SIGFOX_C_SIGFOX_HTTP_SOCKET_H
#define SCHC_OVER_SIGFOX_C_SIGFOX_HTTP_SOCKET_H

#endif //SCHC_OVER_SIGFOX_C_SIGFOX_HTTP_SOCKET_H

int expects_ack;
int seqnum;
int timeout;

void send();
void recv();
void set_reception();
void set_timeout();
#ifndef IPC_H_
#define IPC_H_
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <tlv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <poll.h>
#include <bits/socket.h>
#include <arpa/inet.h>
#include <utils/paquete.h>
#include <utils/buffer.h>

typedef enum Host {
    SERVER,
    CLIENT
} Host;

typedef struct t_message {
    void *buffer;
    int size;
} t_message;

int conn_create_localhost(Host, char *);

int conn_create(Host, char *, char *);

int conn_accept(int);

int conn_close(int);

int conn_is_open(int);

void conn_close_sockets(t_queue *);

void conn_wait_until_close(int);

int peek_socket(int, void *, int);

int read_socket(int, void *, int);

char* read_socket_string(int);

int socket_recv(int socket_fd, t_paquete **paquete);

t_paquete *read_socket_paquete(int);

t_list *read_socket_tlv_list(int);

int write_socket(int, void *, int);

int write_socket_string(int, char *);

int write_socket_paquete(int, t_paquete *);

int write_socket_tlv_list(int, t_list *);

t_paquete *paquete_create(int);

void paquete_destroy(t_paquete *);

t_buffer *buffer_create(int);

void buffer_destroy(t_buffer *);

void *buf_create(int);

int *alloc_int(int);

#endif
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

int conn_create_localhost(Host host_type, char *port);

int conn_create(Host host_type, char *ip, char *port);

int conn_accept(int socket_fd);

int conn_close(int socket_fd);

int conn_is_open(int socket_fd);

void conn_close_sockets(t_queue *);

void conn_wait_until_close(int socket_fd);

int socket_peek(int socket_fd, void *buffer, int buffer_size);

int _socket_recv(int socket_fd, void *buffer, int buffer_size);

int socket_recv(int socket_fd, t_paquete **paquete);

int _socket_send(int socket_fd, void *buffer, int buffer_size);

int socket_send_int(int socket_fd, int *buffer);

int socket_send_string(int socket_fd, char *buffer);

int socket_send(int socket_fd, t_paquete *paquete);

t_paquete *paquete_create_mwrite(int dir, char *bytes, int tamanio, int offset_dir, int p_id);

t_paquete *paquete_create_mread(int dir, int tamanio, int offset_dir, int p_id);

t_paquete *paquete_create(int codigo_operacion);

void paquete_destroy(t_paquete *paquete);

t_buffer *buffer_create(int size);

void buffer_destroy(t_buffer *buffer);

void *buf_create(int size);

int *alloc_int(int value);

#endif
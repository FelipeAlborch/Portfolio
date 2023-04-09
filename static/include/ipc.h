#ifndef IPC_H_
#define IPC_H_
#include <commons/collections/list.h>
#include <tlv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>

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

int peek_socket(int, void *, int);

int read_socket(int, void *, int);

char* read_socket_string(int);

t_list *read_socket_tlv_list(int);

int write_socket(int, void *, int);

int write_socket_string(int, char *);

int write_socket_tlv_list(int, t_list *);

void *buffer_create(int);

#endif
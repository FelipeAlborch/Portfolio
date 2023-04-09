#ifndef IPC_H_
#define IPC_H_
#include <commons/collections/list.h>
#include <tlv.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>

typedef enum Host {
    SERVER,
    CLIENT
} Host;

typedef struct t_payload {
    void *buffer;
    int size;
} t_payload;

int conn_create_localhost(Host, char *);

int conn_create(Host, char *, char *);

int conn_accept(int);

int conn_close(int);

int peek_socket(int, void *, int);

int read_socket(int, t_payload *);

int read_socket_tlv_list(int, t_list *);

int write_socket(int, t_payload *);

int write_socket_tlv_list(int, t_list *);

void *buffer_create(int);

t_payload *payload_create();

t_payload *payload_create_string(char *);

char *payload_get_string(t_payload *);

void payload_destroy(t_payload *);

#endif
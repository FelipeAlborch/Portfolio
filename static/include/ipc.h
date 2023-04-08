#ifndef IPC_H_
#define IPC_H_

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

int write_socket(int, t_payload *);

void *buffer_create(int);

t_payload *payload_create();

t_payload *payload_string_create(char *);

void payload_destroy(t_payload *payload);

#endif
#ifndef IPC_H_
#define IPC_H_

typedef enum Host {
    SERVER,
    CLIENT
} Host;

int conn_create_localhost(Host, char *);

int conn_create(Host, char *, char *);

int conn_accept(int);

int conn_close(int);

#endif
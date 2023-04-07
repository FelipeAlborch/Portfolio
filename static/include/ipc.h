#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <netdb.h>

typedef enum Host {
    SERVER,
    CLIENT
} Host;

int conn_create(Host, char *, char *);

int conn_accept(int);

int conn_close(int);
#include <ipc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>

#define LOCALHOST "127.0.0.1"

int conn_create_localhost(Host host_type, char *puerto)
{
    return conn_create(host_type, LOCALHOST, puerto);
}

int conn_create(Host host_type, char *ip, char *puerto)
{
    int error;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    error = getaddrinfo(ip, puerto, &hints, &servinfo);
    assert(error == 0);

    int socket_fd = socket(
        servinfo->ai_family,
        servinfo->ai_socktype,
        servinfo->ai_protocol
    );
    assert(socket_fd >= 0);

    if (host_type == CLIENT)
    {
        error = connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
        assert(error == 0);
    }
    else if (host_type == SERVER)
    {
        error = bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
        assert(error == 0);

        error = listen(socket_fd, SOMAXCONN);
        assert(error == 0);
    }

    freeaddrinfo(servinfo);

    return socket_fd;
}

int conn_accept(int socket_fd)
{
    int new_socket_fd = accept(socket_fd, NULL, NULL);
    assert(new_socket_fd >= 0);
    return new_socket_fd;
}

int conn_close(int socket_fd)
{
    return close(socket_fd);
}

int peek_socket(int socket_fd, void *buffer, int buffer_size)
{
    int n = recv(socket_fd, buffer, buffer_size, MSG_PEEK);
    assert(n >= 0);

    return n;
}

int read_socket(int socket_fd, t_payload *payload)
{
    int n = recv(socket_fd, &payload->size, sizeof(int), MSG_WAITALL);
    assert(n >= 0);

    payload->buffer = malloc(payload->size);
    n = recv(socket_fd, payload->buffer, payload->size, MSG_WAITALL);
    assert(n >= 0);

    return n;
}

int write_socket(int socket_fd, t_payload *payload)
{
    int _buffer_size = sizeof(payload->size) + payload->size;
    void *_buffer = malloc(_buffer_size);
    memcpy(_buffer, &payload->size, sizeof(payload->size));
    memcpy(_buffer + sizeof(payload->size), payload->buffer, payload->size);
    int n = send(socket_fd, _buffer, _buffer_size, 0);
    assert(n >= 0);

    return n;
}

void *buffer_create(int size)
{
    void *buffer = malloc(size);
    memset(buffer, 0, size);
    return buffer;
}

t_payload *payload_create()
{
    t_payload *payload = malloc(sizeof(t_payload));
    payload->buffer = NULL;
    payload->size = 0;

    return payload;
}

t_payload *payload_string_create(char *string)
{
    t_payload *payload = payload_create();
    payload->size = strlen(string) + 1;
    payload->buffer = malloc(payload->size);
    memcpy(payload->buffer, string, payload->size);

    return payload;
}

void payload_destroy(t_payload *payload)
{
    free(payload->buffer);
    free(payload);
}
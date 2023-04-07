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
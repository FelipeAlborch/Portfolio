#include <ipc.h>

#define LOCALHOST "127.0.0.1"

int conn_create_localhost(Host host_type, char *port)
{
    return conn_create(host_type, LOCALHOST, port);
}

int conn_create(Host host_type, char *ip, char *port)
{
    int error = 0, socket_fd = -1;
    struct addrinfo hints = {0}, *servinfo = NULL, *p = NULL;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    error = getaddrinfo(ip, port, &hints, &servinfo);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(error));
        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_fd == -1) {
            perror("socket error");
            continue;
        }

        if (host_type == CLIENT)
        {
            error = connect(socket_fd, p->ai_addr, p->ai_addrlen);
            if (error == -1) {
                perror("connect error");
                close(socket_fd);
                socket_fd = -1;
                continue;
            }
            break;
        }
        else if (host_type == SERVER)
        {
            error = bind(socket_fd, p->ai_addr, p->ai_addrlen);
            if (error == -1) {
                perror("bind error");
                close(socket_fd);
                socket_fd = -1;
                continue;
            }

            error = listen(socket_fd, SOMAXCONN);
            if (error == -1) {
                perror("listen error");
                close(socket_fd);
                socket_fd = -1;
                continue;
            }
            break;
        }
    }

    if (socket_fd == -1) {
        perror("Failed to create and bind socket");
        freeaddrinfo(servinfo);
        return -1;
    }

    freeaddrinfo(servinfo);
    return socket_fd;
}

int conn_accept(int socket_fd)
{
    int new_socket_fd = accept(socket_fd, NULL, NULL);
    if (new_socket_fd == -1) {
        perror("Error accepting connection");
        return -1;
    }
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

int read_socket(int socket_fd, void *buffer, int buffer_size)
{
    int n = recv(socket_fd, buffer, buffer_size, MSG_WAITALL);
    assert(n >= 0);

    return n;
}

char* read_socket_string(int socket_fd)
{
    int buffer_size;
    int n = recv(socket_fd, &buffer_size, sizeof(int), MSG_WAITALL);
    assert(n >= 0);

    char *string = malloc(buffer_size + 1);
    n = recv(socket_fd, string, buffer_size, MSG_WAITALL);
    assert(n >= 0);
    string[buffer_size] = '\0';

    return string;
}

t_list *read_socket_tlv_list(int socket_fd)
{
    t_list *tlv_list = list_create();
    int n = recv(socket_fd, &tlv_list->elements_count, sizeof(int), MSG_WAITALL);
    assert(n >= 0);

    for (int i = 0; i < tlv_list->elements_count; i++)
    {
        t_tlv *tlv = malloc(sizeof(t_tlv));
        n = recv(socket_fd, &tlv->type, sizeof(tlv->type), MSG_WAITALL);
        assert(n >= 0);
        n = recv(socket_fd, &tlv->length, sizeof(tlv->length), MSG_WAITALL);
        assert(n >= 0);
        tlv->value = malloc(tlv->length);
        n = recv(socket_fd, tlv->value, tlv->length, MSG_WAITALL);
        assert(n >= 0);
        list_add(tlv_list, tlv);
    }

    return tlv_list;
}

int write_socket(int socket_fd, void *buffer, int buffer_size)
{
    int n = send(socket_fd, buffer, buffer_size, 0);
    assert(n >= 0);

    return n;
}

int write_socket_string(int socket_fd, char *string)
{
    int buffer_size = sizeof(int) + strlen(string);
    void *buffer = malloc(buffer_size);
    memcpy(buffer, &buffer_size, sizeof(int));
    memcpy(buffer + sizeof(int), string, strlen(string));
    int n = send(socket_fd, buffer, buffer_size, 0);
    assert(n >= 0);

    return n;
}

int write_socket_tlv_list(int socket_fd, t_list *tlv_list)
{
    int buffer_size = sizeof(int);
    void *buffer = malloc(buffer_size);
    memcpy(buffer, &tlv_list->elements_count, sizeof(int));

    for (int i = 0; i < tlv_list->elements_count; i++)
    {
        t_tlv *tlv = list_get(tlv_list, i);
        buffer_size += sizeof(tlv->type) + sizeof(tlv->length) + tlv->length;
        buffer = realloc(buffer, buffer_size);
        memcpy(buffer + buffer_size - tlv->length - sizeof(tlv->length) - sizeof(tlv->type), &tlv->type, sizeof(tlv->type));
        memcpy(buffer + buffer_size - tlv->length - sizeof(tlv->length), &tlv->length, sizeof(tlv->length));
        memcpy(buffer + buffer_size - tlv->length, tlv->value, tlv->length);
    }

    int n = send(socket_fd, buffer, buffer_size, 0);
    assert(n >= 0);

    return n;
}

void *buffer_create(int size)
{
    void *buffer = malloc(size);
    memset(buffer, 0, size);
    
    return buffer;
}
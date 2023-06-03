#include <ipc.h>

#define LOCALHOST "192.168.1.83"

int conn_create_localhost(Host host_type, char *port)
{
    return conn_create(host_type, LOCALHOST, port);
}

int conn_create(Host host_type, char *ip, char *port)
{
    int status = 0, socket_fd = -1;
    struct addrinfo hints = {0}, *servinfo = NULL, *p = NULL;

    hints.ai_family = AF_INET;// use IPv4
    hints.ai_socktype = SOCK_STREAM;// use TCP
    if (host_type == SERVER) hints.ai_flags = AI_PASSIVE;// for bind
    else if (host_type == CLIENT) hints.ai_flags = AI_ADDRCONFIG;// for connect

    status = getaddrinfo(ip, port, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
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
            status = connect(socket_fd, p->ai_addr, p->ai_addrlen);
            if (status == -1) {
                perror("socket connect error");
                close(socket_fd);
                socket_fd = -1;
                continue;
            }
            break;
        }
        else if (host_type == SERVER)
        {
            status = bind(socket_fd, p->ai_addr, p->ai_addrlen);
            if (status == -1) {
                perror("socket bind error");
                close(socket_fd);
                socket_fd = -1;
                continue;
            }

            status = listen(socket_fd, SOMAXCONN);
            if (status == -1) {
                perror("socket listen error");
                close(socket_fd);
                socket_fd = -1;
                continue;
            }
            break;
        }
    }

    if (socket_fd == -1) {
        perror("socket create error");
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
        perror("socket accept error");
        return -1;
    }
    return new_socket_fd;
}

int conn_close(int socket_fd)
{
    if (close(socket_fd) == -1) {
        perror("socket close error");
        return -1;
    }
    return 0;
}

int conn_is_open(int socket_fd) {
    int error = 0;
    socklen_t len = sizeof(error);
    int ret = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);
    if (ret != 0) return 0;
    return error == 0;
}

void conn_close_sockets(t_queue *sockets)
{
    int socket_fd;
    while (!queue_is_empty(sockets))
    {
        socket_fd = *(int *)queue_pop(sockets);
        if (conn_is_open(socket_fd))
        conn_close(socket_fd);
    }
}

void conn_wait_until_close(int socket_fd)
{
    short int events = POLLIN|POLLOUT|POLLHUP|POLLERR;
    struct pollfd pfd = { socket_fd, events, 0 };
    while (poll(&pfd, 1, -1) > 0) { sleep(1); }
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

t_paquete *read_socket_paquete(int socket_fd)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    int n = recv(socket_fd, &paquete->codigo_operacion, sizeof(paquete->codigo_operacion), MSG_WAITALL);
    assert(n >= 0);

    paquete->buffer = malloc(sizeof(t_buffer));
    n = recv(socket_fd, &paquete->buffer->size, sizeof(paquete->buffer->size), MSG_WAITALL);
    assert(n >= 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    n = recv(socket_fd, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);
    assert(n >= 0);

    return paquete;
}

t_list *read_socket_tlv_list(int socket_fd)
{
    t_list *tlv_list = list_create();
    int n = recv(socket_fd, &tlv_list->elements_count, sizeof(int), MSG_WAITALL);
    assert(n >= 0);

    for (int i = 0; i < tlv_list->elements_count; i++)
    {
        t_tlv *tlv = tlv_create();
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

int write_socket_paquete(int socket_fd, t_paquete *paquete)
{
    int buffer_size = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
    void *buffer = malloc(buffer_size);
    memcpy(buffer, &paquete->codigo_operacion, sizeof(paquete->codigo_operacion));
    memcpy(buffer + sizeof(paquete->codigo_operacion), &paquete->buffer->size, sizeof(paquete->buffer->size));
    memcpy(buffer + sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size), paquete->buffer->stream, paquete->buffer->size);
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

t_paquete *paquete_create(int codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = buffer_create(1);

    return paquete;
}

void paquete_destroy(t_paquete *paquete)
{
    buffer_destroy(paquete->buffer);
    free(paquete);
}

t_buffer *buffer_create(int size)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = size;
    buffer->stream = malloc(size);

    return buffer;
}

void buffer_destroy(t_buffer *buffer)
{
    free(buffer->stream);
    free(buffer);
}

void *buf_create(int size)
{
    void *buffer = malloc(size);
    memset(buffer, 0, size);
    
    return buffer;
}

int *alloc_int(int value) {
    int *ptr = (int*) malloc(sizeof(int));
    *ptr = value;
    return ptr;
}
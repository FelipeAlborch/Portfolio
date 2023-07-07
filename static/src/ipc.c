#include <ipc.h>

#define LOCALHOST "127.0.0.1"

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
        setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
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

int socket_peek(int socket_fd, void *buffer, int buffer_size)
{
    int n = recv(socket_fd, buffer, buffer_size, MSG_PEEK);
    return n >= 0 ? 0 : -1;
}

int _socket_recv(int socket_fd, void *buffer, int buffer_size)
{
    int n = recv(socket_fd, buffer, buffer_size, MSG_WAITALL);
    return n >= 0 ? 0 : -1;
}

int socket_recv_int(int socket_fd, int *value)
{
    return _socket_recv(socket_fd, value, sizeof(*value));
}

int socket_recv_string(int socket_fd, char **string)
{
    int string_size;
    int error = _socket_recv(socket_fd, &string_size, sizeof(string_size));
    if (error == -1) {
        return -1;
    }
    char *_string = malloc(string_size);
    error = _socket_recv(socket_fd, *string, string_size);
    if (error == -1) {
        free(_string);
        return -1;
    }
    *string = _string;
    return 0;
}

int socket_recv(int socket_fd, t_paquete **paquete)
{
    t_paquete *_paquete = paquete_create(PAQUETE);
    int n = recv(socket_fd, &_paquete->codigo_operacion, sizeof(_paquete->codigo_operacion), MSG_WAITALL);
    if(n == -1) {
        paquete_destroy(_paquete);
        return -1;
    }
    (_paquete)->buffer = malloc(sizeof(t_buffer));
    n = recv(socket_fd, &_paquete->buffer->size, sizeof(_paquete->buffer->size), MSG_WAITALL);
    if(n == -1) {
        paquete_destroy(_paquete);
        return -1;
    }
    _paquete->buffer->stream = malloc(_paquete->buffer->size);
    n = recv(socket_fd, _paquete->buffer->stream, _paquete->buffer->size, MSG_WAITALL);
    if(n == -1) {
        paquete_destroy(_paquete);
        return -1;
    }
    *paquete = _paquete;
    return 0;
}

int _socket_send(int socket_fd, void *buffer, int buffer_size)
{
    int n = send(socket_fd, buffer, buffer_size, 0);
    return n >= 0 ? 0 : -1;
}

int socket_send_int(int socket_fd, int *buffer)
{
    return _socket_send(socket_fd, (void *)buffer, sizeof(int));
}

int socket_send_string(int socket_fd, char *string)
{
    int buffer_size = sizeof(int) + strlen(string);
    void *buffer = malloc(buffer_size);
    memcpy(buffer, &buffer_size, sizeof(int));
    memcpy(buffer + sizeof(int), string, strlen(string));
    return _socket_send(socket_fd, buffer, buffer_size);
}

int socket_send(int socket_fd, t_paquete *paquete)
{
    int buffer_size = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
    void *buffer = malloc(buffer_size);
    memcpy(buffer, &paquete->codigo_operacion, sizeof(paquete->codigo_operacion));
    memcpy(buffer + sizeof(paquete->codigo_operacion), &paquete->buffer->size, sizeof(paquete->buffer->size));
    memcpy(buffer + sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size), paquete->buffer->stream, paquete->buffer->size);
    return _socket_send(socket_fd, buffer, buffer_size);
}

t_paquete *paquete_create_mread(int dir, int tamanio, int offset_dir) 
{
    t_paquete *paquete = paquete_create(M_READ);
    paquete->buffer = buffer_create(sizeof(dir) + sizeof(tamanio) + tamanio + sizeof(offset_dir));
    memcpy(paquete->buffer->stream, (void *)&dir, sizeof(dir));
    memcpy(paquete->buffer->stream + sizeof(dir), (void *)&tamanio, sizeof(tamanio));
    memcpy(paquete->buffer->stream + sizeof(dir) + sizeof(tamanio), (void *)&offset_dir, sizeof(offset_dir));
    return paquete;
}

t_paquete *paquete_create_mwrite(int dir, char *bytes, int tamanio, int offset_dir) 
{
    t_paquete *paquete = paquete_create(M_WRITE);
    paquete->buffer = buffer_create(sizeof(dir) + sizeof(tamanio) + tamanio + sizeof(offset_dir));
    memcpy(paquete->buffer->stream, (void *)&dir, sizeof(dir));
    memcpy(paquete->buffer->stream + sizeof(dir), (void *)&tamanio, sizeof(tamanio));
    memcpy(paquete->buffer->stream + sizeof(dir) + sizeof(tamanio), (void *)bytes, tamanio);
    memcpy(paquete->buffer->stream + sizeof(dir) + sizeof(tamanio) + tamanio, (void *)&offset_dir, sizeof(offset_dir));
    return paquete;
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
#include <commons/memory.h>
#include <log.h>
#include <ipc.h>
#include <utils.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

void *connection_handler(void *socket_fd);

int 
main(int argc, char *argv[])
{
    int fs_socket_fd, mem_socket_fd, client_socket_fd, error;

    pthread_t thread_id;

    t_log *log = fs_log_create();

    fs_config *config = fs_config_create();

    print_cwd();

    fs_config_print(config);

    mem_socket_fd = conn_create(CLIENT, config->IP_MEMORIA, config->PUERTO_MEMORIA);

    log_info(log, "mem socket: %d", mem_socket_fd);

    fs_socket_fd = conn_create(SERVER, config->IP_FSYSTEM, config->PUERTO_ESCUCHA);

    log_info(log, "fs socket: %d", fs_socket_fd);

    while ((client_socket_fd = conn_accept(fs_socket_fd)))
    {
        error = pthread_create(&thread_id, NULL, connection_handler, (void *) &client_socket_fd);
        assert(error == 0);

        // pthread_join(client_socket_fd, NULL);

        // close(client_socket_fd);
    }

    close(mem_socket_fd);

    close(fs_socket_fd);

    fs_config_destroy(config);

    log_destroy(log);
        
    return 0;
}

void *connection_handler(void *p_socket_fd)
{
    int socket_fd = *(int *)p_socket_fd;

    char *message = read_socket_string(socket_fd);

    printf("%s\n", message);

    free(message);

    write_socket_string(socket_fd, "mensaje recibido");
    
    conn_close(socket_fd);

    pthread_exit(NULL);
}
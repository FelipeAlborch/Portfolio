#include <utils.h>
#include <config.h>
#include <ipc.h>
#include <commons/log.h>
#include <pthread.h>
#include <assert.h>

#define CONFIG_FILE "fs.config"
#define LOG_FILE "fs.log"
#define PS_NAME "FS"
#define LOCALHOST "127.0.0.1"

void *connection_handler(void *socket_fd);

int 
main(int argc, char *argv[])
{
    print_cwd();

    t_log *log = log_create(LOG_FILE, PS_NAME, 1, LOG_LEVEL_INFO);

    fs_config *config = fs_config_create(CONFIG_FILE);

    fs_config_print(config);

    int mem_socket_fd = conn_create(CLIENT, config->IP_MEMORIA, config->PUERTO_MEMORIA);

    log_info(log, "mem socket: %d", mem_socket_fd);

    int fs_socket_fd = conn_create(SERVER, LOCALHOST, config->PUERTO_ESCUCHA);

    log_info(log, "fs socket: %d", fs_socket_fd);

    pthread_t thread_id;

    int client_socket_fd, error;
    
    while ((client_socket_fd = conn_accept(fs_socket_fd)))
    {
        error = pthread_create(&thread_id, NULL, connection_handler, (void *) &client_socket_fd);
        assert(error == 0);
    }
    assert(client_socket_fd >= 0);

    fs_config_destroy(config);

    log_destroy(log);
        
    return 0;
}

void *connection_handler(void *p_socket_fd)
{
    int n; char buffer[256];

    int socket_fd = *(int *)p_socket_fd;
    
    bzero(buffer, 256);

    n = read(socket_fd, buffer, 255);

    assert(n < 0);

    printf("Bytes received: %s\n", buffer);
    
    conn_close(socket_fd);

    pthread_exit(NULL);
}
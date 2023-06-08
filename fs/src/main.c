#include <commons/memory.h>
#include <log.h>
#include <ipc.h>
#include <utils.h>
#include <config.h>
#include <pthread.h>

void init_config(t_log **log, fs_config **config);
void init_disk(t_log *log, fs_config *config, Disk **disk);
void init_sockets(t_log *log, fs_config *config, int *fs_socket, int *mem_socket);
void init_threads(t_log *log, fs_config *config, int fs_socket, int mem_socket);
void cleanup(t_log *log, fs_config *config, Disk *disk, int fs_socket, int mem_socket);
void * kernel_handler(void *socket_fd);

int main(int argc, char *argv[])
{
    t_log *log;
    fs_config *config;
    Disk *disk;
    int fs_socket;
    int mem_socket;

    init_config(&log, &config);

    init_disk(log, config, &disk);

    init_sockets(log, config, &fs_socket, &mem_socket);

    init_threads(log, config, fs_socket, mem_socket);

    cleanup(log, config, disk, fs_socket, mem_socket);
        
    return 0;
}

void *kernel_handler(void *arg)
{
    int fs_socket = *(int *)arg;
    free(arg);

    t_log *log = log_create_fs();

    int kr_socket;
    while((kr_socket = conn_accept(fs_socket)) != -1)
    {
        char buf[100] = {0};

        read_socket(kr_socket, buf, sizeof(buf));

        log_info(log, "Unknown data from kernel:%s\n", mem_hexstring(buf, sizeof(buf)));

        // TODO: recibir paquetes de kernel
        // dispatchear instrucciones kernel
        // segun la instruccion crear thread
        // para manejar memoria y responder al kernel

        conn_close(kr_socket);

        if (!conn_is_open(fs_socket)) break;
    }

    log_destroy(log);

    pthread_exit((void *) 0);
}

void init_config(t_log **log, fs_config **config)
{
    *log = log_create_fs();

    *config = config_create_fs();

    print_cwd();

    config_print_fs(*config);
}

void init_disk(t_log *log, fs_config *config, Disk **disk)
{
    *disk = disk_create(config);
}

void init_sockets(t_log *log, fs_config *config, int *fs_socket, int *mem_socket)
{
    *mem_socket = conn_create(CLIENT, config->IP_MEMORIA, config->PUERTO_MEMORIA);

    t_paquete *paquete = paquete_create(FILE_SYSTEM);

    write_socket_paquete(*mem_socket, (void *)paquete);

    paquete_destroy(paquete);

    log_info(log, "Conectado a memoria en %s:%s", config->IP_MEMORIA, config->PUERTO_MEMORIA);

    *fs_socket = conn_create(SERVER, config->IP_FSYSTEM, config->PUERTO_ESCUCHA);

    log_info(log, "Escuchando kernel en %s:%s", config->IP_FSYSTEM, config->PUERTO_ESCUCHA);
}

void init_threads(t_log *log, fs_config *config, int fs_socket, int mem_socket)
{
    pthread_t thread_id;

    int status = pthread_create(&thread_id, NULL, kernel_handler, (void *)alloc_int(fs_socket));
    if (status != 0) perror("pthread_create");
    assert(status == 0);

    pthread_join(thread_id, (void **)&status);
    if (status != 0) perror("pthread_join");
    assert(status == 0);
}

void cleanup(t_log *log, fs_config *config, Disk *disk, int fs_socket, int mem_socket)
{
    conn_close(fs_socket);

    conn_close(mem_socket);

    disk_destroy(disk);

    config_destroy_fs(config);

    log_destroy(log);
}
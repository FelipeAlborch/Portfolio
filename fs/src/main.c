#include <commons/memory.h>
#include <ipc.h>
#include <utils.h>
#include <config.h>
#include <pthread.h>

void init_fs(char *config_path, FS **fs);
void init_sockets(FS *fs, int *fs_socket, int *mem_socket);
void init_threads(FS *fs, int fs_socket, int mem_socket);
void cleanup(FS *fs, int fs_socket, int mem_socket);
void * kernel_handler(void *socket_fd);

int main(int argc, char *argv[])
{
    FS *fs;
    int fs_socket;
    int mem_socket;

    init_fs("fs.config", &fs);

    init_sockets(fs, &fs_socket, &mem_socket);

    init_threads(fs, fs_socket, mem_socket);

    cleanup(fs, fs_socket, mem_socket);
        
    return 0;
}

void *kernel_handler(void *arg)
{
    int fs_socket = *(int *)arg;
    free(arg);

    t_log *log = log_create("fs.log", "FS", 1, LOG_LEVEL_INFO);

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

void init_fs(char *config_path, FS **fs)
{
    *fs = fs_create(config_path);
}

void init_sockets(FS *fs, int *fs_socket, int *mem_socket)
{
    *mem_socket = conn_create(CLIENT, fs->config->IP_MEMORIA, fs->config->PUERTO_MEMORIA);

    t_paquete *paquete = paquete_create(FILE_SYSTEM);

    write_socket_paquete(*mem_socket, (void *)paquete);

    paquete_destroy(paquete);

    log_info(fs->log, "Conectado a memoria en %s:%s", fs->config->IP_MEMORIA, fs->config->PUERTO_MEMORIA);

    *fs_socket = conn_create(SERVER, fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);

    log_info(fs->log, "Escuchando kernel en %s:%s", fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);
}

void init_threads(FS *fs, int fs_socket, int mem_socket)
{
    pthread_t thread_id;

    int status = pthread_create(&thread_id, NULL, kernel_handler, (void *)alloc_int(fs_socket));
    if (status != 0) perror("pthread_create");
    assert(status == 0);

    pthread_join(thread_id, (void **)&status);
    if (status != 0) perror("pthread_join");
    assert(status == 0);
}

void cleanup(FS *fs, int fs_socket, int mem_socket)
{
    conn_close(fs_socket);

    conn_close(mem_socket);

    fs_destroy(fs);
}
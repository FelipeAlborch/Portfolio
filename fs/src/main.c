#include <commons/memory.h>
#include <log.h>
#include <ipc.h>
#include <utils.h>
#include <config.h>
#include <pthread.h>

char *BITMAP_FILE = "bitmap.bin";
char *STORE_FILE = "blocks.txt";
const int BLOCK_SIZE = 64;
const int BLOCK_COUNT = 65536;

FCB_table fcb_table;

void * kernel_handler(void *socket_fd);

int 
main(int argc, char *argv[])
{
    int fs_socket, mem_socket, status;

    pthread_t thread_id;

    t_log *log = log_create_fs();

    fs_config *config = config_create_fs();

    print_cwd();

    config_print_fs(config);

    status = fcb_table_init(&fcb_table, BITMAP_FILE, STORE_FILE, BLOCK_SIZE, BLOCK_COUNT);

    mem_socket = conn_create(CLIENT, config->IP_MEMORIA, config->PUERTO_MEMORIA);

    t_paquete *paquete = paquete_create(FILE_SYSTEM);

    write_socket_paquete(mem_socket, (void *)paquete);

    paquete_destroy(paquete);

    log_info(log, "Conectado a memoria en %s:%s", config->IP_MEMORIA, config->PUERTO_MEMORIA);

    fs_socket = conn_create(SERVER, config->IP_FSYSTEM, config->PUERTO_ESCUCHA);

    log_info(log, "Escuchando kernel en %s:%s", config->IP_FSYSTEM, config->PUERTO_ESCUCHA);

    status = pthread_create(&thread_id, NULL, kernel_handler, (void *)alloc_int(fs_socket));
    if (status != 0) perror("pthread_create");

    pthread_join(thread_id, (void **)&status);
    if (status != 0) perror("pthread_join");

    conn_close(fs_socket);

    config_destroy_fs(config);

    log_destroy(log);
        
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
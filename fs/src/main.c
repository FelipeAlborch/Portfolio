#include <commons/memory.h>
#include <pthread.h>
#include <config.h>
#include <utils.h>
#include <ipc.h>

void validate_args(int argc, char **argv);
void init_fs(char *config_path, FS **fs);
void init_sockets(FS *fs);
void init_threads(FS *fs);
void teardown(FS *fs);
void *kernel_handler(void *fs);

int main(int argc, char **argv)
{
    FS *fs;

    validate_args(argc, argv);

    init_fs(argv[1], &fs);

    init_sockets(fs);

    init_threads(fs);

    teardown(fs);

    return 0;
}

void *kernel_handler(void *arg)
{
    FS *fs = (FS *)arg;
    t_paquete *paquete;

    fs->socket_accept = conn_accept(fs->socket_listen);
    if (fs->socket_accept == -1)
    {
        log_error(fs->log, "Error al aceptar conexión");
        pthread_exit((void *)1);
    }

    while (conn_is_open(fs->socket_accept))
    {
        if (socket_recv(fs->socket_accept, &paquete) == -1)
        {
            log_error(fs->log, "Error al recibir paquete");
            continue;
        }

        switch (paquete->codigo_operacion)
        {
            log_info(log, "Operacion de kernel: %d", operacion_de_cpu);
        case ABRIR_ARCHIVO:
            log_info(log, "[ABRIR_ARCHIVO]");
            int detalle = 0; // 0 = ARCHIVO EXISTE, -1 = ARCHIVO NO EXISTE
            // TODO: handle crear archivo
            send(kr_socket, &detalle, sizeof(int), 0);

            break;
        case CREAR_ARCHIVO:
            log_info(log, "[CREAR_ARCHIVO]");

            // TODO: handle crear archivo

            int ack = 0; // 0 = OK, -1 = ERROR
            send(kr_socket, &ack, sizeof(int), 0);
            break;
        case TRUNCAR_ARCHIVO:
            log_info(log, "[TRUNCAR_ARCHIVO]");
            break;
        case LEER_ARCHIVO:
            log_info(log, "[LEER_ARCHIVO]");
            break;
        case ESCRIBIR_ARCHIVO:
            log_info(log, "[ESCRIBIR_ARCHIVO]");
            break;
        default:
            log_info(log, "[OPERACION_DESCONOCIDA]");
            break;
        }

        paquete_destroy(paquete);
    }

    pthread_exit((void *)0);
}

void validate_args(int argc, char **argv)
{
    if (argc < 2)
    {
        if (access("fs.config", F_OK) == 0)
        {
            argv[1] = "fs.config";
        }
        else
        {
            printf("No se encontró el archivo de configuración\n");
            exit(1);
        }
    }
}

void init_fs(char *config_path, FS **fs)
{
    *fs = fs_create(config_path);
}

void init_sockets(FS *fs)
{
    fs->socket_memory = conn_create(CLIENT, fs->config->IP_MEMORIA, fs->config->PUERTO_MEMORIA);

    if (fs->socket_memory == -1)
    {
        log_error(fs->log, "No se pudo conectar a memoria en %s:%s", fs->config->IP_MEMORIA, fs->config->PUERTO_MEMORIA);
        // exit(1);
    }
    else
    {
        t_paquete *paquete = paquete_create(FILE_SYSTEM);
        write_socket_paquete(fs->socket_memory, paquete);
        paquete_destroy(paquete);

        log_info(fs->log, "Conectado a memoria en %s:%s", fs->config->IP_MEMORIA, fs->config->PUERTO_MEMORIA);
    }

    fs->socket_listen = conn_create(SERVER, fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);

    if (fs->socket_listen == -1)
    {
        log_error(fs->log, "No se pudo crear el socket de escucha en %s:%s", fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);
        exit(1);
    }

    log_info(fs->log, "Escuchando kernel en %s:%s", fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);
}

void init_threads(FS *fs)
{
    pthread_t thread_id;

    int status = pthread_create(&thread_id, NULL, kernel_handler, (void *)fs);
    assert(status == 0);

    pthread_join(thread_id, (void **)&status);
    assert(status == 0);
}

void teardown(FS *fs)
{
    conn_close(fs->socket_listen);

    conn_close(fs->socket_memory);

    fs_destroy(fs);
}
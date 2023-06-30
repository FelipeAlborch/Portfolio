#include <commons/memory.h>
#include <pthread.h>
#include <config.h>
#include <utils.h>
#include <ipc.h>
#include <fs_kernel.h>

void validate(int argc, char **argv);
void init(FS **fs, char *config_path);
void init_sockets(FS *fs);
void init_threads(FS *fs);
void teardown(FS *fs);
void *kernel_handler(void *fs);

int main(int argc, char **argv)
{
    FS *fs;

    validate(argc, argv);

    init(&fs, argv[1]);

    init_sockets(fs);

    init_threads(fs);

    teardown(fs);

    return 0;
}

void *kernel_handler(void *arg)
{
    FS *fs = (FS *)arg; int error;
    t_parametros_kernel *params;
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
        case ABRIR_ARCHIVO:
        {
            log_info(fs->log, "[ABRIR_ARCHIVO]");


            int res = -1; // 0 = ARCHIVO EXISTE, -1 = ARCHIVO NO EXISTE
            parametros = deserializar_parametros_fopen(paquete->buffer);
            res = f_open(parametros->nombre_archivo, fs);

            t_respuesta_fs* respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = parametros->nombre_archivo;
            if (res == 0) {
                respuesta->error = ARCHIVO_SUCCESS;
            } else {
                respuesta->error = ARCHIVO_NO_EXISTE;
            }
            respuesta->tamanio = 0;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);


            params = deserializar_parametros_fopen(paquete->buffer);
            error = f_open(params->nombre_archivo, fs);
            socket_write_int(fs->socket_accept, &error);
            parametros_destroy(params);
            paquete_destroy(paquete);

            break;
        }
        case CREAR_ARCHIVO:
        {
            log_info(fs->log, "[CREAR_ARCHIVO]");


            int res = -1; // 0 = OK, -1 = ERROR
            parametros = deserializar_parametros_fcreate(paquete->buffer);
            res = f_create(parametros->nombre_archivo, fs);

            t_respuesta_fs* respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = parametros->nombre_archivo;
            if (res == 0) {
                respuesta->error = ARCHIVO_SUCCESS;
            } else {
                respuesta->error = ARCHIVO_NO_EXISTE; // error al crear el archivo (no deberia pasar)
            }
            respuesta->tamanio = 0;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            params = deserializar_parametros_fcreate(paquete->buffer);
            error = f_create(params->nombre_archivo, fs);
            socket_write_int(fs->socket_accept, &error);
            parametros_destroy(params);
            paquete_destroy(paquete);
            break;
        }
        case TRUNCAR_ARCHIVO:
        {
            log_info(fs->log, "[TRUNCAR_ARCHIVO]");

            int res = -1; // 0 = OK, -1 = ERROR
            parametros = deserializar_parametros_ftruncate(paquete->buffer);
            res = f_truncate(parametros->nombre_archivo, parametros->tamanio, fs);

            t_respuesta_fs* respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = parametros->nombre_archivo;
            if (res == 0) {
                respuesta->error = ARCHIVO_SUCCESS;
            } else {
                respuesta->error = ERROR_DE_FS;
            }

            respuesta->tamanio = parametros->tamanio;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            params = deserializar_parametros_ftruncate(paquete->buffer);
            error = f_truncate(params->nombre_archivo, params->tamanio, fs);
            socket_write_int(fs->socket_accept, &error);
            parametros_destroy(params);
            paquete_destroy(paquete);
            break;
        }
        case LEER_ARCHIVO:
        {
            log_info(fs->log, "[LEER_ARCHIVO]");


            int res_fs = -1;  // 0 = OK, -1 = ERROR
            int res_mem = -1; // 0 = OK, -1 = ERROR
            parametros = deserializar_parametros_fread(paquete->buffer);
            char *buff = malloc(parametros->tamanio);
            res_fs = f_read(parametros->nombre_archivo, parametros->posicion, parametros->tamanio, parametros->dir, buff, fs);
            // TODO: escribir el contenido de buff en direccion de memoria
            // res_mem = escribir_memoria(&buff, parametros->posicion, parametros->tamanio);

            // mock escribir memoria
            res_mem = 0;

            t_respuesta_fs* respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = parametros->nombre_archivo;
            if (res_fs == 0 && res_mem == 0) {
                respuesta->error = ARCHIVO_SUCCESS;
            } else if (res_fs == 0) {
                respuesta->error = ERROR_DE_FS;
            } else if (res_mem == 0) {
                respuesta->error = ERROR_DE_MEMORIA;
            }

            respuesta->tamanio = parametros->tamanio;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            char *bytes;
            params = deserializar_parametros_fread(paquete->buffer);
            error = f_read(params->nombre_archivo, params->posicion, params->tamanio, (void **)&bytes, fs);

            if (error == 0) {
                paquete_destroy(paquete);
                paquete = paquete_create_mwrite(params->dir, bytes, params->tamanio);
                error = socket_write_paquete(fs->socket_memory, paquete);
            }

            socket_write_int(fs->socket_accept, &error);
            parametros_destroy(params);
            paquete_destroy(paquete);
            break;
        }
        case ESCRIBIR_ARCHIVO:
        {
            log_info(fs->log, "[ESCRIBIR_ARCHIVO]");

            int res_fs = -1;  // 0 = OK, -1 = ERROR
            int res_mem = -1; // 0 = OK, -1 = ERROR
            parametros = deserializar_parametros_fwrite(paquete->buffer);
            char *buff = malloc(parametros->tamanio);
            // TODO: leer de direccion de memoria y guardar en buff
            // res_mem = leer_memoria(&buff, parametros->posicion, parametros->tamanio);

            // mock leer memoria
            res_mem = 0;
            memcpy(buff, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", parametros->tamanio);

            res_fs = f_write(parametros->nombre_archivo, parametros->posicion, parametros->tamanio, parametros->dir, buff, fs);

            t_respuesta_fs* respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = parametros->nombre_archivo;
            if (res_fs == 0 && res_mem == 0) {
                respuesta->error = ARCHIVO_SUCCESS;
            } else if (res_mem == 0) {
                respuesta->error = ERROR_DE_MEMORIA;
            } else if (res_fs == 0) {
                respuesta->error = ERROR_DE_FS;
            } 

            respuesta->tamanio = parametros->tamanio;
            respuesta->buffer_size = parametros->tamanio;
            respuesta->buffer = buff;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            params = deserializar_parametros_fwrite(paquete->buffer);
            paquete_destroy(paquete);

            paquete = paquete_create_mread(params->dir, params->tamanio);
            error = socket_write_paquete(fs->socket_memory, paquete);
            paquete_destroy(paquete);

            error = socket_recv(fs->socket_memory, &paquete);
            if (error == -1 || paquete->codigo_operacion != (op_code)M_READ) {
                log_error(fs->log, "Error al recibir paquete");
                break;
            }
            error = f_write(params->nombre_archivo, params->posicion, paquete->buffer->size, paquete->buffer->stream, fs);
            parametros_destroy(params);
            paquete_destroy(paquete);
            break;
        }
        default:
            log_info(fs->log, "[OPERACION_DESCONOCIDA]");
            break;
        }
    }

    pthread_exit((void *)0);
}

void validate(int argc, char **argv)
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

void init(FS **fs, char *config_path)
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
        socket_write_paquete(fs->socket_memory, paquete);
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

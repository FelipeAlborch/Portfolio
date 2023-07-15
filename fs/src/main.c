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
FS *fs;
int main(int argc, char **argv)
{
    startSigHandlers();

    //FS *fs;

    validate(argc, argv);

    init(&fs, argv[1]);

    init_sockets(fs);

    init_threads(fs);

    teardown(fs);

    return 0;
}

void *kernel_handler(void *arg)
{
    FS *fs = (FS *)arg;
    int error;
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

            t_parametros_kernel *params = deserializar_parametros_fopen(paquete->buffer);
            error = f_open(params->nombre_archivo, fs);

            t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = params->nombre_archivo;
            if (error == 0)
            {
                respuesta->error = ARCHIVO_SUCCESS;
            }
            else
            {
                respuesta->error = ARCHIVO_NO_EXISTE;
            }
            respuesta->tamanio = 0;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            parametros_destroy(params);
            paquete_destroy(paquete);
            free(respuesta);
            break;
        }
        case CREAR_ARCHIVO:
        {
            log_info(fs->log, "[CREAR_ARCHIVO]");

            t_parametros_kernel *params = deserializar_parametros_fcreate(paquete->buffer);
            error = f_create(params->nombre_archivo, fs);

            t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = params->nombre_archivo;
            if (error == 0)
            {
                respuesta->error = ARCHIVO_SUCCESS;
            }
            else
            {
                respuesta->error = ARCHIVO_NO_EXISTE; // error al crear el archivo (no deberia pasar)
            }
            respuesta->tamanio = 0;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            parametros_destroy(params);
            paquete_destroy(paquete);
            free(respuesta);
            break;
        }
        case TRUNCAR_ARCHIVO:
        {
            log_info(fs->log, "[TRUNCAR_ARCHIVO]");

            t_parametros_kernel *params = deserializar_parametros_ftruncate(paquete->buffer);
            error = f_truncate(params->nombre_archivo, params->tamanio, fs);

            t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = params->nombre_archivo;
            if (error == 0)
            {
                respuesta->error = ARCHIVO_SUCCESS;
            }
            else
            {
                respuesta->error = ERROR_DE_FS;
            }
            respuesta->tamanio = params->tamanio;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            parametros_destroy(params);
            paquete_destroy(paquete);
            free(respuesta);
            break;
        }
        case LEER_ARCHIVO:
        {
            log_info(fs->log, "[LEER_ARCHIVO]");

            int error;
            int res_fs = -1;  // 0 = OK, -1 = ERROR
            msj_memoria res_mem;

            char *bytes;
            t_parametros_kernel *params = deserializar_parametros_fread(paquete->buffer);
            log_info(fs->log, "[PARAMS] archivo: %s - dir:  %d - posicion: %d - tamanio: %d - offset: %d, - pid: %d", params->nombre_archivo, params->df_real, params->posicion, params->tamanio, params->offset_dir, params->p_id);
            res_fs = f_read(params->nombre_archivo, params->posicion, params->tamanio, params->df_real, (void **)&bytes, fs);

            if (res_fs == 0)
            {
                paquete_destroy(paquete);
                paquete = paquete_create_mwrite(params->dir, bytes, params->tamanio, params->offset_dir, params->p_id);
                error = socket_send(fs->socket_memory, paquete);
                assert(error == 0);
                error = recv(fs->socket_memory, &res_mem, sizeof(msj_memoria), MSG_WAITALL);
                assert(error == sizeof(msj_memoria));
            }

            t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = params->nombre_archivo;
            if (res_fs == 0 && res_mem == M_WRITE)
            {
                respuesta->error = ARCHIVO_SUCCESS;
            }
            else if (res_fs != 0)
            {
                respuesta->error = ERROR_DE_FS;
            }
            else if (res_mem != 0)
            {
                respuesta->error = ERROR_DE_MEMORIA;
            }
            respuesta->tamanio = params->tamanio;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            parametros_destroy(params);
            paquete_destroy(paquete);
            free(respuesta);
            free(bytes);
            break;
        }
        case ESCRIBIR_ARCHIVO:
        {
            log_info(fs->log, "[ESCRIBIR_ARCHIVO]");

            int res_fs = -1;  // 0 = OK, -1 = ERROR
            int res_mem = -1; // 0 = OK, -1 = ERROR

            t_parametros_kernel *params = deserializar_parametros_fwrite(paquete->buffer);
            paquete_destroy(paquete);

            paquete = paquete_create_mread(params->dir, params->tamanio, params->offset_dir, params->p_id);
            res_mem = socket_send(fs->socket_memory, paquete);

            log_info(fs->log, "[PARAMS] archivo: %s - dir:  %d - posicion: %d - tamanio: %d - offset: %d, - pid: %d", params->nombre_archivo, params->df_real, params->posicion, params->tamanio, params->offset_dir, params->p_id);
            paquete_destroy(paquete);

            error = socket_recv(fs->socket_memory, &paquete);
            if (error == -1 || paquete->codigo_operacion != (op_code)M_READ)
            {
                log_error(fs->log, "Error al recibir paquete");
                parametros_destroy(params);
                break;
            }

            int tamanio;
            memcpy(&tamanio, paquete->buffer->stream, sizeof(int));
            char *dato = malloc(tamanio);
            memcpy(dato, paquete->buffer->stream + sizeof(int), tamanio);

            log_info(fs->log, "Dato: %s - Tamaño: %d", dato, tamanio);

            res_fs = f_write(params->nombre_archivo, params->posicion, params->tamanio, params->df_real, dato, fs);

            t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));
            respuesta->nombre_archivo = params->nombre_archivo;
            if (res_fs == 0 && res_mem == 0)
            {
                respuesta->error = ARCHIVO_SUCCESS;
            }
            else if (res_mem != 0)
            {
                respuesta->error = ERROR_DE_MEMORIA;
            }
            else if (res_fs != 0)
            {
                respuesta->error = ERROR_DE_FS;
            }
            respuesta->tamanio = 0;
            respuesta->buffer_size = 0;
            respuesta->buffer = NULL;

            enviar_respuesta_a_kernel(fs->socket_accept, respuesta);

            parametros_destroy(params);
            paquete_destroy(paquete);
            free(respuesta);
            free(dato);
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
            perror("No se encontró el archivo de configuración\n");
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
        paquete->buffer = buffer_create(1);
        socket_send(fs->socket_memory, paquete);
        paquete_destroy(paquete);

        log_info(fs->log, "Conectado con memoria en %s:%s", fs->config->IP_MEMORIA, fs->config->PUERTO_MEMORIA);
    }

    fs->socket_listen = conn_create(SERVER, fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);

    if (fs->socket_listen == -1)
    {
        log_error(fs->log, "No se pudo crear el socket de escucha en %s:%s", fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);
        // exit(1);
    }
    else
    {
        log_info(fs->log, "Escuchando en %s:%s", fs->config->IP_FSYSTEM, fs->config->PUERTO_ESCUCHA);
    }
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

void startSigHandlers(void) {
	signal(SIGINT, sigHandler_sigint);
}

void sigHandler_sigint(int signo) {
	log_warning(fs->log,"Tiraste un CTRL+C, macho, abortaste el proceso.");

	teardown(fs);
	printf("-------------------FINAL POR CTRL+C-------------------\n");

	exit(-1);
}
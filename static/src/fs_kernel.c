#include <fs_kernel.h>

t_parametros_kernel *deserializar_parametros_fopen(t_buffer *buffer)
{
    int offset = 0;
    int tamanio_nombre;
    t_parametros_kernel *params = malloc(sizeof(t_parametros_kernel));

    memcpy(&tamanio_nombre, buffer->stream, sizeof(int));
    params->nombre_archivo = malloc(tamanio_nombre);
    offset += sizeof(int);
    memcpy(params->nombre_archivo, buffer->stream + offset, tamanio_nombre);

    return params;
}

t_parametros_kernel *deserializar_parametros_fcreate(t_buffer *buffer)
{
    int offset = 0;
    int tamanio_nombre;
    t_parametros_kernel *params = malloc(sizeof(t_parametros_kernel));

    memcpy(&tamanio_nombre, buffer->stream, sizeof(int));
    params->nombre_archivo = malloc(tamanio_nombre);
    offset += sizeof(int);
    memcpy(params->nombre_archivo, buffer->stream + offset, tamanio_nombre);

    return params;
}

t_parametros_kernel *deserializar_parametros_ftruncate(t_buffer *buffer)
{
    int offset = 0;
    int tamanio_nombre;
    t_parametros_kernel *params = malloc(sizeof(t_parametros_kernel));

    memcpy(&tamanio_nombre, buffer->stream, sizeof(int));
    params->nombre_archivo = malloc(tamanio_nombre);
    offset += sizeof(int);
    memcpy(params->nombre_archivo, buffer->stream + offset, tamanio_nombre);
    offset += tamanio_nombre;
    memcpy(&params->tamanio, buffer->stream + offset, sizeof(int));

    return params;
}

t_parametros_kernel *deserializar_parametros_fread(t_buffer *buffer)
{
    int offset = 0;
    int tamanio_nombre;
    t_parametros_kernel *params = malloc(sizeof(t_parametros_kernel));

    memcpy(&tamanio_nombre, buffer->stream, sizeof(int));
    params->nombre_archivo = malloc(tamanio_nombre);
    offset += sizeof(int);

    memcpy(params->nombre_archivo, buffer->stream + offset, tamanio_nombre);
    offset += tamanio_nombre;

    memcpy(&params->posicion, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->dir, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->tamanio, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->offset_dir, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->p_id, buffer->stream + offset, sizeof(int));

    return params;
}

t_parametros_kernel *deserializar_parametros_fwrite(t_buffer *buffer)
{
    int offset = 0;
    int tamanio_nombre;
    t_parametros_kernel *params = malloc(sizeof(t_parametros_kernel));

    memcpy(&tamanio_nombre, buffer->stream, sizeof(int));
    params->nombre_archivo = malloc(tamanio_nombre);
    offset += sizeof(int);

    memcpy(params->nombre_archivo, buffer->stream + offset, tamanio_nombre);
    offset += tamanio_nombre;

    memcpy(&params->posicion, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->dir, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->tamanio, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->offset_dir, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    memcpy(&params->p_id, buffer->stream + offset, sizeof(int));

    return params;
}

void parametros_destroy(t_parametros_kernel *params)
{
    free(params->nombre_archivo);
    free(params);
}

void enviar_respuesta_a_kernel(int socket, t_respuesta_fs *respuesta)
{
    t_paquete *paquete_respuesta_fs = malloc(sizeof(t_paquete));
    paquete_respuesta_fs->codigo_operacion = PAQUETE;
    paquete_respuesta_fs->buffer = malloc(sizeof(t_buffer));
    paquete_respuesta_fs->buffer->size = 0;
    paquete_respuesta_fs->buffer->stream = NULL;

    agregar_a_paquete(paquete_respuesta_fs, respuesta->nombre_archivo, strlen(respuesta->nombre_archivo) + 1);
    agregar_entero_a_paquete(paquete_respuesta_fs, &respuesta->error);
    agregar_entero_a_paquete(paquete_respuesta_fs, &respuesta->tamanio);
    agregar_entero_a_paquete(paquete_respuesta_fs, &respuesta->buffer_size);
    if (respuesta->buffer_size > 0)
    {
        agregar_a_paquete(paquete_respuesta_fs, respuesta->buffer, respuesta->tamanio);
    }

    enviar_paquete(paquete_respuesta_fs, socket);
    paquete_destroy(paquete_respuesta_fs);
}

// deserializar respuesta de fs
t_respuesta_fs *recibir_respuesta_de_fs(int socket)
{
    t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));

    t_paquete *paquete = recibir_paquete(socket);
    int offset = 0;
    int tamanio_nombre;
    int tamanio_error;

    // respuesta->nombre_archivo (size)
    memcpy(&tamanio_nombre, paquete->buffer->stream, sizeof(int));
    offset += sizeof(int);

    // respuesta->nombre_archivo
    respuesta->nombre_archivo = malloc(tamanio_nombre);
    memcpy(respuesta->nombre_archivo, paquete->buffer->stream + offset, tamanio_nombre);
    offset += tamanio_nombre;

    // respuesta->error (== sizeof(FS_Error))
    // memcpy(&tamanio_error, paquete->buffer->stream + offset, sizeof(int));
    // offset += sizeof(int);

    // respuesta->error
    memcpy(&respuesta->error, paquete->buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    // respuesta->tamanio
    memcpy(&respuesta->tamanio, paquete->buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    // respuesta->buffer_size
    memcpy(&respuesta->buffer_size, paquete->buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    if (respuesta->buffer_size > 0)
    {
        // respuesta->buffer
        respuesta->buffer = malloc(respuesta->buffer_size);
        memcpy(respuesta->buffer, paquete->buffer->stream + offset, respuesta->tamanio);
        offset += respuesta->buffer_size;
    }

    return respuesta;
}

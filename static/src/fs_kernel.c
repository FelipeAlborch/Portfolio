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

    offset += sizeof(int);
    memcpy(&params->posicion, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    offset += sizeof(int);
    memcpy(&params->dir, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    offset += sizeof(int);
    memcpy(&params->tamanio, buffer->stream + offset, sizeof(int));

    printf("PARAMS: %s - %d - %d - %d", params->nombre_archivo, params->posicion, params->dir, params->tamanio);

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

    offset += sizeof(int);
    memcpy(&params->posicion, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    offset += sizeof(int);
    memcpy(&params->dir, buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    offset += sizeof(int);
    memcpy(&params->tamanio, buffer->stream + offset, sizeof(int));

    printf("PARAMS: %s - %d - %d - %d", params->nombre_archivo, params->posicion, params->dir, params->tamanio);

    return params;
}

void parametros_destroy(t_parametros_kernel *params)
{
    free(params->nombre_archivo);
    free(params);
}

void enviar_respuesta_a_kernel(int socket, t_respuesta_fs *respuesta)
{
    assert(respuesta->error == 0);
    t_paquete *paquete_respuesta_fs = crear_paquete_operacion(PAQUETE);
    agregar_a_paquete(paquete_respuesta_fs, respuesta->nombre_archivo, strlen(respuesta->nombre_archivo) + 1);
    agregar_a_paquete(paquete_respuesta_fs, &respuesta->error, sizeof(FS_Error));
    agregar_a_paquete(paquete_respuesta_fs, &respuesta->tamanio, sizeof(int));
    agregar_a_paquete(paquete_respuesta_fs, &respuesta->buffer_size, sizeof(int));
    if (respuesta->buffer_size > 0)
    {
        agregar_a_paquete(paquete_respuesta_fs, respuesta->buffer, respuesta->tamanio);
    }

    enviar_paquete(paquete_respuesta_fs, socket);
}

// deserializar respuesta de fs
t_respuesta_fs *recibir_respuesta_de_fs(int socket)
{
    t_respuesta_fs *respuesta = malloc(sizeof(t_respuesta_fs));

    t_paquete *paquete = recibir_paquete(socket);
    int offset = 0;
    int tamanio_nombre;

    // respuesta->nombre_archivo (size)
    memcpy(&tamanio_nombre, paquete->buffer->stream, sizeof(int));
    offset += sizeof(int);

    // respuesta->nombre_archivo
    respuesta->nombre_archivo = malloc(tamanio_nombre);
    memcpy(respuesta->nombre_archivo, paquete->buffer->stream + offset, tamanio_nombre);
    offset += tamanio_nombre;

    // respuesta->error
    // FIXME: al serializar se agrega un int para el tamanio del del dato (aunque sea un int),
    // no estoy seguro por qué ni como arreglarlo, pero por ahora lo dejo así para que funcione

    offset += sizeof(int);
    memcpy(&respuesta->error, paquete->buffer->stream + offset, sizeof(FS_Error));
    offset += sizeof(FS_Error);

    // respuesta->tamanio
    // FIXME: idem anterior
    offset += sizeof(int);
    memcpy(&respuesta->tamanio, paquete->buffer->stream + offset, sizeof(int));
    offset += sizeof(int);

    // respuesta->buffer_size
    // FIXME: idem anterior
    offset += sizeof(int);
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

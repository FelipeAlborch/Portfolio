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

    return params;
}

void parametros_destroy(t_parametros_kernel *params)
{
    free(params->nombre_archivo);
    free(params);
}

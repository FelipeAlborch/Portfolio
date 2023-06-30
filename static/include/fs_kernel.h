#ifndef FS_KERNEL_H_
#define FS_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <utils/buffer.h>
#include <utils/paquete.h>

typedef enum FS_Error
{
    ARCHIVO_NO_EXISTE = -100,
    ERROR_DE_FS = -200,
    ERROR_DE_MEMORIA = -300,
    ERROR_DESCONOCIDO = -1,
    ARCHIVO_SUCCESS = 0
} FS_Error;

typedef struct t_parametros_kernel
{
    char *nombre_archivo;
    int posicion;
    int dir;
    int tamanio;
} t_parametros_kernel;

typedef struct t_respuesta_fs
{
    char *nombre_archivo;
    FS_Error error;
    int tamanio;
    int buffer_size;
    char *buffer;
} t_respuesta_fs;

t_parametros_kernel *deserializar_parametros_fcreate(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_fopen(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_ftruncate(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_fread(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_fwrite(t_buffer *buffer);

void enviar_respuesta_a_kernel(int socket, t_respuesta_fs *respuesta);
t_respuesta_fs *recibir_respuesta_de_fs(int socket);

void parametros_destroy(t_parametros_kernel *params);

#endif
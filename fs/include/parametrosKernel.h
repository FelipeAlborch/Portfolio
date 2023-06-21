#ifndef PARAMETROS_KERNEL
#define PARAMETROS_KERNEL

#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

typedef struct t_parametros_kernel
{
    char *nombre_archivo;
    int posicion;
    int dir;
    int tamanio;
} t_parametros_kernel;

t_parametros_kernel *deserializar_parametros_fcreate(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_fopen(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_ftruncate(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_fread(t_buffer *buffer);
t_parametros_kernel *deserializar_parametros_fwrite(t_buffer *buffer);

void parametros_destroy(t_parametros_kernel *params);

#endif
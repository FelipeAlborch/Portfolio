#ifndef CONSOLA_UTILS_H
#define CONSOLA_UTILS_H

#define SUCCESS 0
#define FAILURE -1

#include <consolaConfig.h>
#include <commons/txt.h>

extern int socket_kernel;

int conectar_con_kernel();

#endif
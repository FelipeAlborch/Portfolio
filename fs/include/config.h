#ifndef FSCONFIG_H_
#define FSCONFIG_H_
#include <commons/config.h>

typedef struct fs_config {
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_ESCUCHA;
    char *PATH_SUPERBLOQUE;
    char *PATH_BITMAP;
    char *PATH_BLOQUES;
    char *PATH_FCB;
    int RETARDO_ACCESO_BLOQUE;
} fs_config;

fs_config *fs_config_create(char *);

void fs_config_print(fs_config *);

void fs_config_destroy(fs_config *);

#endif
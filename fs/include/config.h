#ifndef FSCONFIG_H_
#define FSCONFIG_H_
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct fs_config {
    char *IP_FSYSTEM;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_ESCUCHA;
    char *PATH_SUPERBLOQUE;
    char *PATH_BITMAP;
    char *PATH_BLOQUES;
    char *PATH_FCB;
    int RETARDO_ACCESO_BLOQUE;
} fs_config;

fs_config *config_create_fs();

fs_config *config_create_fs_from_file(char *);

void config_print_fs(fs_config *);

void config_destroy_fs(fs_config *);

#endif
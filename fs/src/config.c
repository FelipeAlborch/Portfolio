#include <stdio.h>
#include <stdlib.h>
#include <config.h>
#include <string.h>

fs_config *fs_config_create(char *path) {

    t_config *t_config = config_create(path);

    fs_config *fs_config = malloc(sizeof(struct fs_config));

    fs_config->IP_MEMORIA = strdup(config_get_string_value(t_config, "IP_MEMORIA"));
    fs_config->PUERTO_MEMORIA = strdup(config_get_string_value(t_config, "PUERTO_MEMORIA"));
    fs_config->PUERTO_ESCUCHA = strdup(config_get_string_value(t_config, "PUERTO_ESCUCHA"));
    fs_config->PATH_SUPERBLOQUE = strdup(config_get_string_value(t_config, "PATH_SUPERBLOQUE"));
    fs_config->PATH_BITMAP = strdup(config_get_string_value(t_config, "PATH_BITMAP"));
    fs_config->PATH_BLOQUES = strdup(config_get_string_value(t_config, "PATH_BLOQUES"));
    fs_config->PATH_FCB = strdup(config_get_string_value(t_config, "PATH_FCB"));
    fs_config->RETARDO_ACCESO_BLOQUE = config_get_int_value(t_config, "RETARDO_ACCESO_BLOQUE");

    config_destroy(t_config);
    
    return fs_config;
}

void fs_config_print(fs_config *config) {
    printf("IP_MEMORIA = %s\n", config->IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", config->PUERTO_MEMORIA);
    printf("PUERTO_ESCUCHA = %s\n", config->PUERTO_ESCUCHA);
    printf("PATH_SUPERBLOQUE = %s\n", config->PATH_SUPERBLOQUE);
    printf("PATH_BITMAP = %s\n", config->PATH_BITMAP);
    printf("PATH_BLOQUES = %s\n", config->PATH_BLOQUES);
    printf("PATH_FCB = %s\n", config->PATH_FCB);
    printf("RETARDO_ACCESO_BLOQUE = %d\n", config->RETARDO_ACCESO_BLOQUE);
}

void fs_config_destroy(fs_config *config) {
    free(config->IP_MEMORIA);
    free(config->PUERTO_MEMORIA);
    free(config->PUERTO_ESCUCHA);
    free(config->PATH_SUPERBLOQUE);
    free(config->PATH_BITMAP);
    free(config->PATH_BLOQUES);
    free(config->PATH_FCB);
    free(config);
}
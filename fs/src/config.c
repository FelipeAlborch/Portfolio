#include <config.h>

fs_config *config_create_fs_from_file(char *file_path) {

    assert(access(file_path, F_OK) == 0);

    t_config *t_config = config_create(file_path);

    fs_config *config = malloc(sizeof(fs_config));

    config->IP_FSYSTEM = strdup(config_get_string_value(t_config, "IP_FSYSTEM"));
    config->IP_MEMORIA = strdup(config_get_string_value(t_config, "IP_MEMORIA"));
    config->PUERTO_MEMORIA = strdup(config_get_string_value(t_config, "PUERTO_MEMORIA"));
    config->PUERTO_ESCUCHA = strdup(config_get_string_value(t_config, "PUERTO_ESCUCHA"));
    config->PATH_SUPERBLOQUE = strdup(config_get_string_value(t_config, "PATH_SUPERBLOQUE"));
    config->PATH_BITMAP = strdup(config_get_string_value(t_config, "PATH_BITMAP"));
    config->PATH_BLOQUES = strdup(config_get_string_value(t_config, "PATH_BLOQUES"));
    config->PATH_FCB = strdup(config_get_string_value(t_config, "PATH_FCB"));
    config->RETARDO_ACCESO_BLOQUE = config_get_int_value(t_config, "RETARDO_ACCESO_BLOQUE");

    config_destroy(t_config);
    
    return config;
}

void config_print_fs(fs_config *config) {
    printf("IP_MEMORIA = %s\n", config->IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", config->PUERTO_MEMORIA);
    printf("PUERTO_ESCUCHA = %s\n", config->PUERTO_ESCUCHA);
    printf("PATH_SUPERBLOQUE = %s\n", config->PATH_SUPERBLOQUE);
    printf("PATH_BITMAP = %s\n", config->PATH_BITMAP);
    printf("PATH_BLOQUES = %s\n", config->PATH_BLOQUES);
    printf("PATH_FCB = %s\n", config->PATH_FCB);
    printf("RETARDO_ACCESO_BLOQUE = %d\n", config->RETARDO_ACCESO_BLOQUE);
}

void config_destroy_fs(fs_config *config) {
    free(config->IP_FSYSTEM);
    free(config->IP_MEMORIA);
    free(config->PUERTO_MEMORIA);
    free(config->PUERTO_ESCUCHA);
    free(config->PATH_SUPERBLOQUE);
    free(config->PATH_BITMAP);
    free(config->PATH_BLOQUES);
    free(config->PATH_FCB);
    free(config);
}
#ifndef FSCONFIG_H_
#define FSCONFIG_H_
#include <commons/config.h>

typedef struct fs_config{
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_ESCUCHA;
    char *PATH_SUPERBLOQUE;
    char *PATH_BITMAP;
    char *PATH_BLOQUES;
    char *PATH_FCB;
    int RETARDO_ACCESO_BLOQUE;
}fs_config;

/**
* @NAME: obtener_valores_de_configuracion_filesystem
* @DESC: A partir de un t_config*, carga todos sus valores en el config_de_filesystem
* @PARAMS:
*        t_config* filesystem_config - La configuracion que se toma como primer parametro del main.
* @RETURN:
*        retorna la config_de_filesystem creada.
*/
fs_config *fs_config_create(char *);

/**
* @NAME: mostrar_valores_de_configuracion_filesystem
* @DESC: Printea en pantalla los valores de una config_de_filesystem
* @PARAMS:
*        config_de_filesystem configuracion_filesystem - La configuracion creada previamente a partir del t_config*
*/
void fs_config_print(fs_config *);

void fs_config_destroy(fs_config *);

#endif
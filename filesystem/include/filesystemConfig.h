#ifndef FILESYSTEMCONFIG_H_
#define FILESYSTEMCONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

typedef struct{
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA;
    char* PATH_SUPERBLOQUE;
    char* PATH_BITMAP;
    char* PATH_BLOQUES;
    char* PATH_FCB;
    int RETARDO_ACCESO_BLOQUE;
}config_de_filesystem;

extern config_de_filesystem configuracion_filesystem;

/**
* @NAME: obtener_valores_de_configuracion_filesystem
* @DESC: A partir de un t_config*, carga todos sus valores en el config_de_filesystem
* @PARAMS:
*        t_config* filesystem_config - La configuracion que se toma como primer parametro del main.
* @RETURN:
*        retorna la config_de_filesystem creada.
*/
config_de_filesystem obtener_valores_de_configuracion_filesystem(t_config*);

/**
* @NAME: mostrar_valores_de_configuracion_filesystem
* @DESC: Printea en pantalla los valores de una config_de_filesystem
* @PARAMS:
*        config_de_filesystem configuracion_filesystem - La configuracion creada previamente a partir del t_config*
*/
void mostrar_valores_de_configuracion_filesystem(config_de_filesystem);

#endif
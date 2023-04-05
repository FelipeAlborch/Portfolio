#ifndef MEMORIACONFIG_H_
#define MEMORIACONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>


typedef struct{
    char* PUERTO_ESCUCHA; 
    int TAM_MEMORIA;
    int TAM_SEGMENTO_0;
    int CANT_SEGMENTOS;
    int RETARDO_MEMORIA;
    int RETARDO_COMPACTACION;
    char* ALGORITMO_ASIGNACION;
}config_de_memoria;

extern config_de_memoria configuracion_memoria;

/**
* @NAME: obtener_valores_de_configuracion_memoria
* @DESC: A partir de un t_config*, carga todos sus valores en el config_de_memoria
* @PARAMS:
*        t_config* memoria_config - La configuracion que se toma como primer parametro del main.
* @RETURN:
*        retorna la config_de_memoria creada.
*/
config_de_memoria obtener_valores_de_configuracion_memoria(t_config*);

/**
* @NAME: mostrar_valores_de_configuracion_memoria
* @DESC: Printea en pantalla los valores de una config_de_memoria
* @PARAMS:
*        config_de_memoria configuracion_memoria - La configuracion creada previamente a partir del t_config*
*/
void mostrar_valores_de_configuracion_memoria(config_de_memoria);

#endif
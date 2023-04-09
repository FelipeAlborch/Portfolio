#ifndef CONSOLA_CONFIG_H_
#define CONSOLA_CONFIG_H

#include <commons/config.h>
#include <utils/conexion.h>

extern int prueba;

typedef struct{
   char* IP_KERNEL;
   char* PUERTO_KERNEL;
}config_de_consola;

extern config_de_consola configuracion_consola;


/**
* @NAME: obtener_valores_de_configuracion_consola
* @DESC: A partir de un t_config*, carga todos sus valores en el config_de_consola
* @PARAMS:
*        t_config* consolaConfig - La configuracion que se toma como primer parametro del main.
* @RETURN:
*        retorna la config_de_consola creado.
*/
config_de_consola obtener_valores_de_configuracion_consola(t_config*);

/**
* @NAME: mostrar_valores_de_configuracion_consola
* @DESC: Printea en pantalla los valores de una config_de_consola
* @PARAMS:
*        config_de_consola configuracion_consola - La configuracion creada previamente a partir del t_config*
*/
void mostrar_valores_de_configuracion_consola(config_de_consola);


#endif
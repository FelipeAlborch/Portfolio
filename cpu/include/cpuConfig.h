#ifndef CPUCONFIG_H_
#define CPUCONFIG_H_

#include <commons/config.h>
#include <utils/conexion.h>
#include <MMU.h>


typedef struct{
    int RETARDO_INSTRUCCION;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA;
    int TAM_MAX_SEGMENTO;
}config_de_cpu;

extern config_de_cpu configuracionCPU;


/**
* @NAME: obtener_valores_de_configuracion_cpu
* @DESC: A partir de un t_config*, carga todos sus valores en el config_de_cpu
* @PARAMS:
*        t_config* cpuConfig - La configuracion que se toma como primer parametro del main.
* @RETURN:
*        retorna la config_de_cpu creada.
*/
config_de_cpu obtener_valores_de_configuracion_cpu(t_config*);

/**
* @NAME: mostrar_valores_de_configuracion_cpu
* @DESC: Printea en pantalla los valores de una config_de_cpu
* @PARAMS:
*        config_de_cpu configuracion_cpu - La configuracion creada previamente a partir del t_config*
*/
void mostrar_valores_de_configuracion_cpu(config_de_cpu);


#endif
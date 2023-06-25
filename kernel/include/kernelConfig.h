#ifndef CONFIGURACION_DE_KERNEL_H_
#define CONFIGURACION_DE_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <utils/conexion.h>

typedef struct{
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* IP_FILESYSTEM;
    char* PUERTO_FILESYSTEM;
    char* IP_CPU;
    char* PUERTO_CPU;
    char* PUERTO_ESCUCHA;
    char* ALGORITMO_PLANIFICACION;
    int ESTIMACION_INICIAL;
    double HRRN_ALFA;
    int GRADO_MAX_MULTIPROGRAMACION;
    char** RECURSOS;
    char** INSTANCIAS_RECURSOS;
    char* IP_SERVER;
}config_de_kernel;

typedef struct{
    char* nombre;
    int instancias;
    t_queue* cola_bloqueados;
    pthread_mutex_t mutex_cola;
    int posicion;
}t_recurso;

extern config_de_kernel configuracionKernel;
extern t_dictionary* diccionario_recursos;
extern t_dictionary* tabla_de_procesos;


/**
* @NAME: obtener_valores_de_configuracion_kernel
* @DESC: A partir de un t_config*, carga todos sus valores en el config_de_kernel
* @PARAMS:
*        t_config* kernelConfig - La configuracion que se toma como primer parametro del main.
* @RETURN:
*        retorna la config_de_kernel creada.
*/
config_de_kernel obtener_valores_de_configuracion_kernel(t_config*);

/**
* @NAME: mostrar_valores_de_configuracion_kernel
* @DESC: Printea en pantalla los valores de una config_de_kernel
* @PARAMS:
*        config_de_kernel configuracion_kernel - La configuracion creada previamente a partir del t_config*
*/
void mostrar_valores_de_configuracion_kernel(config_de_kernel);

void iniciar_tabla_de_procesos();

void agregar_proceso_a_tabla(pcb*);

void leer_tabla_procesos();

//void destruir_tabla_de_procesos();

void crear_diccionario_recursos();

void leer_diccionario_recursos();

t_recurso* crear_recurso(char*, int);

void liberar_recurso(t_recurso*);

#endif
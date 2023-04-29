#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <utils/conexion.h>
#include <commons/collections/queue.h>
//#include <kernelConfig.h>

#include <kernelUtils.h>
#include <kernelArchivos.h>

//extern int socket_cpu_planificador;
//extern int socket_memoria_planificador;
//extern int socket_fs_planificador;

// variable global para el process id
extern int pid_global;

// Cola de planificacion new global.
//extern t_queue* cola_new;

// Tabla de manejo de archivos abiertos.
extern t_dictionary* tabla_global_archivos_abiertos;

/*****************************************************************************
 *              FUNCIONES PARA LAS ESTRUCTURAS DE PLANIFICACION
******************************************************************************/

void inicializar_estructuras_planificacion();

void destruir_estructuras_planificacion();


/*****************************************************************************
 *              FUNCIONES PARA LOS PLANIFICADORES
******************************************************************************/

void iniciar_planificadores();

void* planificador_largo_plazo();

void* planificador_corto_plazo_fifo();

void* planificador_corto_plazo_hrrn();

void ejecutar(pcb*);




#endif
#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <utils/conexion.h>
#include <commons/collections/queue.h>
//#include <kernelConfig.h>

#include <kernelUtils.h>

//extern int socket_cpu_planificador;
//extern int socket_memoria_planificador;
//extern int socket_fs_planificador;

// variable global para el process id
extern int pid_global;

// Cola de planificacion new global.
extern t_queue* cola_new;

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

/*****************************************************************************
 *              FUNCIONES PARA MOVER PROCESOS POR LAS COALS
******************************************************************************/

void agregar_proceso_new(pcb*);

void agregar_proceso_ready(pcb*);

void agregar_proceso_exec(pcb*);

void agregar_proceso_terminated(pcb*);

void agregar_proceso_block(pcb*);

pcb* obtener_proceso_new();

pcb* obtener_proceso_ready();

pcb* obtener_proceso_block();

pcb* desalojar_proceso_en_exec();



void loguear_cola_de_procesos(t_queue*);

void* queue_peek_at(t_queue*, int);

void loguear_procesos_en_cola(t_queue*);

void loguear_procesos_en_lista(t_list*);


#endif
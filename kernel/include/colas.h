#ifndef COLAS_H_
#define COLAS_H_

#include <kernelConfig.h>

// LOGGER PARA EL PLANIFICADOR
extern t_log* logger_planificador_obligatorio;
extern t_log* logger_planificador_extra;

// Colas de planificacion
extern t_queue* cola_new;
extern t_queue* cola_ready_fifo;
extern t_queue* cola_exec;
extern t_queue* cola_terminated;

// Lista para procesos en ready por HRRN. tiene que ser una lista asi la puedo filtrar por Response Ration
extern t_list* lista_ready_hrrn;

// Semaforos mutex para las colas de los procesos.
extern pthread_mutex_t mutex_new;
extern pthread_mutex_t mutex_ready;
extern pthread_mutex_t mutex_exec;
extern pthread_mutex_t mutex_terminated;

// Semaforos para activar los hilos de planificacion.
extern sem_t activar_largo_plazo;
extern sem_t activar_corto_plazo;

// Un semaforo para saber si hay un proceso ejecutando en el cpu o no.
extern sem_t sem_hay_en_running;

// Semaforo para la mutliprogramacion.
extern sem_t grado_multiprogramacion;

extern bool es_fifo;


/*****************************************************************************
 *              FUNCIONES PARA MOVER PROCESOS POR LAS COALS
******************************************************************************/

void agregar_proceso_new(pcb*);

void agregar_proceso_ready(pcb*);

void agregar_proceso_exec(pcb*);

void agregar_proceso_terminated(pcb*);

pcb* obtener_proceso_new();

pcb* obtener_proceso_ready();

pcb* desalojar_proceso_en_exec();

/*****************************************************************************
 *              FUNCIONES UTILES PARA LAS COLAS
******************************************************************************/

void loguear_cola_de_procesos(t_queue*);

void* queue_peek_at(t_queue*, int);

void loguear_procesos_en_cola(t_queue*);

void loguear_procesos_en_lista(t_list*);

#endif
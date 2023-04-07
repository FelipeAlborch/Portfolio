#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <utils/conexion.h>
#include <commons/collections/queue.h>
#include <kernelConfig.h>

extern int socket_cpu_planificador;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
extern t_dictionary* diccionario_de_consolas;

// variable global para el process id
extern int pid_global;

// Cola de planificacion new global.
extern t_queue* cola_new;

void inicializar_planificadores(config_de_kernel);

void planificador_largo_plazo();

void planificador_corto_plazo();

void agregar_proceso_new(pcb*);

void agregar_proceso_ready(pcb*);

void agregar_proceso_exec(pcb*);

void agregar_proceso_terminated(pcb*);

void agregar_proceso_block(pcb*);

#endif
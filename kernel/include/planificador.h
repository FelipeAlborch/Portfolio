#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <utils/conexion.h>
#include <commons/collections/queue.h>
#include <kernelConfig.h>

extern t_queue* cola_new;
extern int socket_cpu_dispatch_planificador;
extern int socket_cpu_interrupt_planificador;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
extern t_dictionary* diccionario_de_consolas;

// variable global para el process id
extern int pid_global;


#endif
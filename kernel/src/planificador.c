#include <planificador.h>

// SOCKETS AL CPU

int socket_cpu_dispatch_planificador;
int socket_cpu_interrupt_planificador;

// LOGGER PARA EL PLANIFICADOR
t_log* logger_planificador_obligatorio;
t_log* logger_planificador_extra;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
t_dictionary* diccionario_de_consolas;

#ifndef KERNELUTILS_H_
#define KERNELUTILS_H_

//#include <kernelConexion.h>

#include <kernelConfig.h>

extern t_log* logger_kernel_util_obligatorio;
extern t_log* logger_kernel_util_extra;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
extern t_dictionary* diccionario_de_consolas;

// Los sockets que usaremos en todo el modulo kernel, para las diferentes conexiones salvo las consolas.
extern int socketMemoria;
extern int socketCPU;
extern int socketFS;

void agregar_socket_a_diccionario(int, int);

// Esta funcion es solo para probar que se estan guardando bien los sockets.
void leer_diccionario_consolas();

double calcular_response_ratio(pcb*);

bool comparar_response_ratio(pcb*, pcb*);

#endif
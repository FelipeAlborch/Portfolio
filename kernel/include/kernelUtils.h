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

/**
* @NAME: agregar_socket_a_diccionario
* @DESC: Dado un PID y un socket correspondiente a una consola, guarda el PID como key, del value socket.
* @PARAMS:
*        int pcb_pid - El pid que sera utilizado como key.
*        int socket_consola - El socket que sera el value guardado en el diccionario.
*/
void agregar_socket_a_diccionario(int, int);

// Esta funcion es solo para probar que se estan guardando bien los sockets.
void leer_diccionario_consolas();

/**
* @NAME: calcular_response_ratio
* @DESC: Dado un pcb* calcula el response ratio para el mismo.
* @PARAMS:
*        pcb* un_pcb - El proceso que se desea calcular el response ratio.
*
* @RETURN:
*        Retorna el response ratio calculado.
*/
double calcular_response_ratio(pcb*);

/**
* @NAME: calcular_response_ratio
* @DESC: Dados dos pcb, calcula si el primero tiene mayor response ratio que el segundo.
* @PARAMS:
*        pcb* un_pcb - El proceso que se desea ver si tiene mayor response ratio.
*        pcb* otro_pcb - El otro proceso.
* @RETURN:
*        Retorna true o false.
*/
bool comparar_response_ratio(pcb*, pcb*);

/**
* @NAME: estimar_proxima_rafaga
* @DESC: Dado un pcb* (QUE HAYA EJECUTADO) calcula una estimacion para la proxima rafaga de cpu.
* @PARAMS:
*        pcb* un_pcb - El proceso al que se desea calcular una estimacion de proxima rafaga.
* @RETURN:
*        Retorna el valor estimado.
* @ACLARACION:
*        EL PROCESO DEBE HABER EJECUTADO POR LO MENOS UNA VEZ
*/
double estimar_proxima_rafaga(pcb*);

#endif
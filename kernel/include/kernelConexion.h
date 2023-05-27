#ifndef KERNELCONEXION_H_
#define KERNELCONEXION_H_

#include <planificador.h>

int iniciar_servidor_para_consolas(config_de_kernel);

/**
* @NAME: crear_conexion_a_memoria
* @DESC: Se conecta a la MEMORIA y realiza el handshake con la misma (Aclaracion: la MEMORIA ya tiene que estar abierta).
* @PARAMS:
*        config_de_kernel configuracion_kernel. Es la configuracion creada a partir del archivo de config que viene por parametro en el main
* @RETURN:
*        retorna el socket de conexion a MEMORIA.
*/
int conectar_con_memoria(config_de_kernel);

/**
* @NAME: conectar_con_cpu
* @DESC: Se conecta al CPU por conexion crea el socketCPU (Aclaracion: El CPU ya tiene que estar abierto).
* @PARAMS:
*        config_de_kernel configuracionKernel. Es la configuracion creada a partir del archivo de config que viene por parametro en el main
* @RETURN:
*        retorna el socket de conexion a cpu.
*/
int conectar_con_cpu(config_de_kernel);

/**
* @NAME: conectar_con_filesystem
* @DESC: Se conecta al FileSystem, crea el socketFS (Aclaracion: El FS ya tiene que estar abierto).
* @PARAMS:
*        config_de_kernel configuracionKernel. Es la configuracion creada a partir del archivo de config que viene por parametro en el main
* @RETURN:
*        retorna el socket de conexion a FS.
*/
int conectar_con_filesystem(config_de_kernel);

/**
* @NAME: esperar_consolas
* @DESC: Rutina del hilo del main, encargado de recibir un cliente consola, y mandar el socket a operar. Crea un hilo por cada consola conectada.
* @PARAMS:
*        int socketServidorConsolas. El socket de servicio para las consolas creado previamente
*/
void* esperar_consolas(int);

/**
* @NAME: escuchar_consola
* @DESC: Rutina de los hilos para las consolas, encargado de recibir una operacion de la consola, debe crear el PCB y mandarlo a la cola_new.
* @PARAMS:
*        int socketConsola. El socket de una consola.
*/
void escuchar_consola(int socketConsola);

void deserializar_lista_de_consola(t_list*, t_list*, int, int);




#endif
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

#endif
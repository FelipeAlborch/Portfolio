#ifndef MEMORIACONEXION_H_
#define MEMORIACONEXION_H_

#include <memoriaConfig.h>
#include <utils/conexion.h>

extern t_log* logger_memoria;


/**
* @NAME: iniciar_servicio_memoria
* @DESC: Crea el socket_servicio_memoria ya en listen.
* @PARAMS:
*        config_de_memoria configuracion_memoria - Es la configuracion creada a partir del archivo de config que viene por parametro en el main
* @RETURN:
*        retorna el socket para la conexion con el KERNEL y el CPU en listen.
*/
int iniciar_servicio_memoria(config_de_memoria);

/**
* @NAME: esperar_clientes_a_memoria
* @DESC: Espera a que se conecte un cliente. Cuando alguno se conecta, recibe una operacion por parte de este, y analiza si es el KERNEL o el CPU
* @PARAMS:
*        int socket_socket_de_un_cliente - El socket de un cliente cualquiera
*/
void esperar_clientes_a_memoria(int);

/**
* @NAME: es_kernel
* @DESC: Confirma si el cliente conectado es el KERNEL.
* @PARAMS:
*        int socket_de_un_cliente - El socket de un cliente cualquiera
* @RETURN:
*        retorna true si es KERNEL, y false si es CPU.
*/
bool es_kernel(int);

/**
* @NAME: escucha_kernel
* @DESC: Recibe operaciones por el socket del KERNEL y respondende dependiendo de cual sea.
* @PARAMS:
*        int socket_a_kernel - El socket del kernel
*/
void escucha_kernel(int);

/**
* @NAME: escucha_cpu
* @DESC: Recibe operaciones por el socket del CPU y respondende dependiendo de cual sea.
* @PARAMS:
*        int socket_a_cpu - El socket del cpu
*/
void escucha_cpu(int);

void manejar_paquetes_clientes(int);
bool es_kernel(int);
bool es_cpu(int);

#endif
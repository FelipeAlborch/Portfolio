#ifndef GENERALES_H_
#define GENERALES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <assert.h>
#include <commons/config.h>
#include <commons/temporal.h>

typedef t_config Config;
typedef t_list Lista;
typedef t_log Logger;
typedef pthread_t Hilo;
typedef sem_t Semaforo;
typedef enum
{
	KERNEL_LOGGER,
	CPU_LOGGER,
	CONSOLA_LOGGER,
	MEMORIA_LOGGER,
	FILESYSTEM_LOGGER,
	KERNEL_LOGGER_EXTRA,
	CPU_LOGGER_EXTRA,
	CONSOLA_LOGGER_EXTRA,
	MEMORIA_LOGGER_EXTRA,
	FILESYSTEM_LOGGER_EXTRA
}codigo_logger;

typedef enum
{
	CONSOLA,
	KERNEL,
	CPU,
	FILE_SYSTEM,
	MEMORIA
}modulo;

typedef enum
{
	CREATE_SEGMENT = 100, //se envia el pid y el tamanio del segmento
	DELETE_SEGMENT,	//	se envia el pid y el id del segmento
	INICIO_COMPACTAR,	
	FIN_COMPACTAR,
	M_READ,		//se envia el pid, el id del segmento y el offset
	M_WRITE,	//se envia el pid, el id del segmento, el offset y el valor a escribir
	INICIO_PROCESO,
	FIN_PROCESO,
	OK,
	ERROR,
	OUT_OF_MEMORY,
	TABLAS_PROCESOS,
	MOV_OUT,	//se escribe el valor del registro en memoria
	MOV_IN		//se lee el valor del registro en memoria
	
}msj_memoria;

typedef enum
{
	SIGNAL_SUCCESS,
	SIGNAL_FAILURE,
	WAIT_SUCCESS,
	WAIT_FAILURE,
	WAIT_BLOCKED
}recurso_comunicacion;

typedef struct{
	int pid;
	t_list* lista_segmentos;

}t_proceso;


/**
* @NAME: iniciar_logger_modulo
* @DESC: Crea un logger en un determinado path para el modulo correspondiente.
* @PARAMS:
*        int codigo_logger - Es el codigo_logger del enum. Puede ser KERNEL_LOGGER, CPU_LOGGER, CONSOLA_LOGGER y MEMORIA_LOGGER.
* @RETURN:
*        retorna el logger listo para usar.
*/
t_log* iniciar_logger_modulo(int);

#endif
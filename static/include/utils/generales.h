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
	CREATE_SEGMENT = 100,
	DELETE_SEGMENT,
	INICIO_COMPACTAR,
	FIN_COMPACTAR,
	M_READ,
	M_WRITE,
	INICIO_PROCESO,
	FIN_PROCESO,
	OK,
	ERROR

}msj_memoria;

typedef enum
{
	SIGNAL_SUCCESS,
	SIGNAL_FAILURE,
	WAIT_SUCCESS,
	WAIT_FAILURE
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
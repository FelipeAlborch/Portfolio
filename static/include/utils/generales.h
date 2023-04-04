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
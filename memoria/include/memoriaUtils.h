#ifndef MEMORIAUTILS_H_
#define MEMORIAUTILS_H_


#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include "memoriaConfig.h"


typedef struct
{
	bool libre;     //libre --> 1 ocupado -->0
	void* inicio_segmento;
	int tamanio_segmento;
	int id_segmento;
	int id_proceso;
    int base;
    int direcion_fisica;
    pthread_mutex_t* mutex_segmento;

}t_segmento;

extern config_de_memoria configuracionMemoria;

void terminar_programa(int conexion, t_log* logger, t_config* configMemoria);
void liberar_memoria();
void liberar_listas();
void liberar_conexion_memoria(int conexion);
void liberar_t_config();

#endif
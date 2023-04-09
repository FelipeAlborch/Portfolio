#ifndef MEMORIAUTILS_H_
#define MEMORIAUTILS_H_

// Este include ya tiene incluidos muchos archivos de las commons y cosas tipo stdlib, stdio etc, tambien pcb y cosas del estilo.
#include <utils/conexion.h>

extern t_log* loggerMemoria;
extern int conexion;
extern t_config* memoriaConfig;

/*typedef struct
{
	bool libre;     //libre --> 1 ocupado -->0
	void* inicio_segmento;
	int tamanio_segmento;
	int id_segmento;
	int id_proceso;
    int base;
    int direcion_fisica;
    pthread_mutex_t* mutex_segmento;

}t_segmento;*/


void sigHandler_sigint(int signo);
void startSigHandlers(void);

void terminar_programa(int conexion, t_log* logger, t_config* configMemoria);
void liberar_memoria();
void liberar_listas();
void liberar_conexion_memoria(int conexion);
void liberar_t_config();

void inicializar_configuracion();
void inicializar_memoria();
void inicializar_segmentos();


// Lo del conexion:
int iniciar_servicio_memoria(config_de_memoria);

void manejar_paquetes_clientes(int);

modulo interpretar_origen_conexion(int);

void escucha_kernel(int);

void escucha_cpu(int);


// Lo del config:

typedef struct{
    char* PUERTO_ESCUCHA; 
    int TAM_MEMORIA;
    int TAM_SEGMENTO_0;
    int CANT_SEGMENTOS;
    int RETARDO_MEMORIA;
    int RETARDO_COMPACTACION;
    char* ALGORITMO_ASIGNACION;
    int tam_maximo;

}config_de_memoria;

extern config_de_memoria config_memo;

void obtener_valores_de_configuracion_memoria(t_config*);

void mostrar_valores_de_configuracion_memoria();

#endif
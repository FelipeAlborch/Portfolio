#ifndef MEMORIAUTILS_H_
#define MEMORIAUTILS_H_

// Este include ya tiene incluidos muchos archivos de las commons y cosas tipo stdlib, stdio etc, tambien pcb y cosas del estilo.
#include <utils/conexion.h>

extern t_log* mlogger;
extern t_log* loggerMemoria;
extern int clientes[4];
extern int conexion;
extern int running;
extern int server_m;
extern t_config* memoriaConfig;
extern pthread_t hilo_cpu;
extern pthread_t hilo_kernel;
extern pthread_t hilo_fs;
extern pthread_t hiloConexion;

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

void terminar_programa(t_log* logger, t_config* configMemoria);
void liberar_memoria();
void liberar_listas();
void liberar_conexion_memoria();
void liberar_t_config();

void inicializar_configuracion();
void inicializar_memoria();
void inicializar_segmentos();


// Lo del conexion:
void conectar();
void conectar_cpu();
void conectar_kernel();
void conectar_fs();
int iniciar_servicio_memoria(config_de_memoria);

void manejar_paquetes_clientes(int);

modulo interpretar_origen_conexion(int);

void ejecutar_kernel();
void ejecutar_cpu();
void ejecutar_fs();




// Lo del config:

typedef struct{
    char* puerto;
    char* algoritmo;
    char* ip; 
    int tam_memo;
    int tam_seg_0;
    int cant_seg;
    int retardo;
    int compactacion;
    int tam_maximo;
    int cpu;    // para guardar la conexion
    int fs;     // para guardar la conexion
    int kernel; // para guardar la conexion

}config_de_memoria;

extern config_de_memoria config_memo;

void obtener_valores_de_configuracion_memoria(t_config*);

void mostrar_valores_de_configuracion_memoria();


void loggear(int tipo, int level,void* algo,...);

#endif
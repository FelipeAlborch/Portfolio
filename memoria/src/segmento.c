#include <memoriaUtils.h>

/*
log_info(logger_ram_hq,"Creando estructuras administrativas para esquema de memoria: Segmentacion");
		numero_segmento_global = 0;
		t_segmento* segmento = malloc(sizeof(t_segmento));
		segmento->inicio_segmento =  memoria_principal;
		segmento->tamanio_segmento = mi_ram_hq_configuracion->TAMANIO_MEMORIA;
		segmento->libre = true;
		segmento->mutex_segmento = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(segmento->mutex_segmento,NULL);
		log_info(logger_ram_hq,"Se reservaron %i bytes de memoria que comienzan en %i",mi_ram_hq_configuracion->TAMANIO_MEMORIA,memoria_principal);
		list_add(segmentos_memoria,segmento);
		signal(SIGUSR1, sighandlerCompactar); //10
*/
/*
Para implementar un módulo de memoria con segmentación en C, es necesario implementar varias funciones. En este caso, se pide implementar las siguientes:
*/
void crear_estructuras(){}
void liberar_proceso(int pid){} 
void* leer_dato(int pid, int direccion, int size){
	void* info;
	return info;
}
void escribir_dato(int pid, int direccion, int size, void* valor){}
void crear_segmento(int pid, int size){}
void eliminar_segmento(int pid, int direccion){}
void compactar(){}
void crear_segmento_(int pid, int size){}
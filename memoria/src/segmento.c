#include <memoriaUtils.h>
t_list* tabla_segmentos;
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

void* leer_dato(int pid, int direccion, int size){
	void* info;
	return info;
}
void escribir_dato(int pid, int direccion, int size, void* valor){}
void crear_segmento(int pid, int size){}
void eliminar_segmento(int pid, int direccion){}
void compactar(){}
void crear_segmento_(int pid, int size){}

t_tabla_segmentos* crear_tabla_segmentos(int pid, int cant_seg, int tam_seg){
	t_tabla_segmentos* tabla_seg = malloc(sizeof(t_tabla_segmentos));
    tabla_seg->pid = pid;
    tabla_seg->tabla_segmentos = list_create();

	for (int i = 0; i < cant_seg; i++) {
      
        void* direccion_base = malloc(tam_seg);

        t_segmento* segmento = malloc(TAM_CABECERA);
        segmento->id_seg = i;
        segmento->size = tam_seg;
        segmento->libre = 1;
        list_add(tabla_seg->tabla_segmentos, segmento);

        memcpy(direccion_base, segmento, TAM_CABECERA);
        segmento->libre = 0;
    }
	list_add(tabla_segmentos, tabla_seg);

    return tabla_seg;
}
t_tabla_segmentos* buscar_en_tabla(int pid){

}
t_segmento* buscar_segmento(int id_seg, t_list* segmentos){
	for (int i = 0; i < list_size(segmentos); i++) {
        t_segmento* segmento = list_get(segmentos, i);
        if (segmento->id_seg == id_seg) {
            return segmento;
        }
    }

    return NULL;
}
void liberar_proceso(int pid) {
    t_tabla_segmentos* tabla_seg;
   // = buscar_tabla_segmentos(pid);

    for (int i = 0; i < list_size(tabla_seg->tabla_segmentos); i++) {
        t_segmento* segmento = list_get(tabla_seg->tabla_segmentos, i);
        free(segmento + TAM_CABECERA);
    }
	//list_remove_by_condition(tabla_segmentos, (void*) es_tabla_segmentos_de_pid);
    free(tabla_seg);
}
#include <memoriaUtils.h>
t_list* huecos_libres;
t_list *tabla_segmentos_gral;
//config_de_memoria config_memo;
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

void inicializar_segmentos(){
    t_tabla_segmentos* tabla = crear_tabla_segmentos(0,0,0);
    tabla_segmentos_gral = list_create();

    for (size_t i = 0; i < config_memo.cant_seg_max; i++)
    {
        modificar_tabla_segmentos(tabla,-1,-1,i,-1,0);
        list_add_in_index(tabla_segmentos_gral,i,tabla);
    }
    modificar_tabla_segmentos(tabla,-1,-1,0,0,config_memo.tam_seg_0);
    list_add_in_index(tabla_segmentos_gral,0,tabla);
    
    free(tabla->segmento);
    free(tabla);
}
void modificar_tabla_segmentos(t_tabla_segmentos* tabla,int pid, int dir, int index,int base, int size){
    tabla->pid = pid;
    tabla->direcion_fisica = dir;
    tabla->index = index;
    tabla->segmento->base = base;
    tabla->segmento->size = size;
}
t_segmento* crear_segmento(int base, int size){
    t_segmento* segmento = malloc(sizeof(TAM_CABECERA));
    
    segmento->base=base;
    segmento->size=size;
    
    return segmento;
}
void crear_estructuras(){
    huecos_libres = list_create();
    t_segmento* segmento = crear_segmento(0,config_memo.tam_seg_0);
    
    for (size_t i = 0; i < config_memo.cant_seg_max; i++)
    {
        list_add_in_index(huecos_libres,i,true);
    }
    list_add_in_index(huecos_libres,0,false);

    inicializar_segmentos();
    
    free(segmento);
}


void* leer_dato(int pid, int direccion, int size){
	void* info;
	return info;
}
void escribir_dato(int pid, int direccion, int size, void* valor){

}

void eliminar_segmento(int pid, int direccion){}
void compactar(){}
void crear_segmento_(int pid, int size){}


t_tabla_segmentos* crear_tabla_segmentos(int pid, int dir, int index){
	t_tabla_segmentos* tabla_seg = malloc(sizeof(t_tabla_segmentos));
    int size = config_memo.tam_seg_0;
    int base =0;
    tabla_seg->pid = pid;
    tabla_seg->direcion_fisica=dir;
    tabla_seg->index=index;
    tabla_seg->segmento = crear_segmento(base,size); 

    return tabla_seg;
}
t_tabla_segmentos* buscar_en_tabla(int pid){

}

t_segmento* buscar_segmento(int base, t_list* segmentos){
	for (int i = 0; i < list_size(segmentos); i++) {
        t_segmento* segmento = list_get(segmentos, i);
        if (segmento->base == base) {
            return segmento;
        }
    }

    return NULL;
}
void liberar_proceso(int pid) {
    t_segmento* tabla_seg;
   // = buscar_tabla_segmentos(pid);

    if (tabla_seg == NULL) {
        return;
    }

    eliminar_segmento_list(tabla_seg, tabla_segmentos_gral);
	//list_remove_by_condition(tabla_segmentos, (void*) es_tabla_segmentos_de_pid);
    free(tabla_seg);
}

void eliminar_segmento_list(t_segmento* segmento, t_list* segmentos){
    list_remove_by_condition(segmentos, (void*) free);
    free(segmento);
}

void eliminar_segmento_por_id(int id_seg, t_list* segmentos){
    t_segmento* segmento = buscar_segmento(id_seg, segmentos);
    eliminar_segmento_list(segmento, segmentos);
}

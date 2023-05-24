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
    t_tabla_segmentos* tabla = crear_tabla_segmentos(0,0,0,config_memo.tam_seg_0);
    tabla_segmentos_gral = list_create();
    int j = 0;
    list_add_in_index(tabla_segmentos_gral,0,tabla);
    for (size_t i = 1; i < config_memo.cant_seg_max; i++)
    {
        modificar_tabla_segmentos(tabla,j,-1,i,-1,0);
        list_add_in_index(tabla_segmentos_gral,i,tabla);
       // printf("%d- pid: %d  base: %d  size %d\n",i, tabla->pid, tabla->segmento->base, tabla->segmento->size);
        j--;
    }
   // modificar_tabla_segmentos(tabla,-1,0,0,0,config_memo.tam_seg_0);
    //list_replace(tabla_segmentos_gral,0,tabla);
    
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
    t_hueco_libre* hueco = crear_hueco_libre(0,config_memo.tam_seg_0,OCUPADO);
    list_add_in_index(huecos_libres,0,hueco);
    list_add_in_index(huecos_libres,1,crear_hueco_libre(config_memo.tam_seg_0+1,config_memo.bytes_libres,LIBRE));

    inicializar_segmentos();
    
    free(hueco);
}


void* leer_dato(int pid, int direccion, int size){
	void* info;
	return info;
}
void escribir_dato(int pid, int direccion, int size, void* valor){

}

void eliminar_segmento(int pid, int direccion){
    int index = buscar_en_tabla_index(pid);
    t_tabla_segmentos* tabla = malloc(sizeof(t_tabla_segmentos));
    tabla = list_get(tabla_segmentos_gral,index);
    modificar_tabla_segmentos(tabla,-1,-1,tabla->index,-1,tabla->segmento->size);

        _Bool remover(t_tabla_segmentos* element) {
            return element->pid == pid;
        }
    
    list_replace_by_condition(tabla_segmentos_gral,(void*)remover, tabla);
    
    actualizar_memoria(tabla->segmento->size,index,LIBRE);

    free(tabla->segmento);
    free(tabla);
}
void compactar(){
    recibir_operacion(config_memo.kernel);
    loggear(INICIO_COMPACTAR,0,NULL,0,0,0);
}
void crear_segmento_(int pid, int size){}


t_tabla_segmentos* crear_tabla_segmentos(int pid, int index, int base, int size){
	t_tabla_segmentos* tabla_seg = malloc(sizeof(t_tabla_segmentos));
    int dir= base + size;
    
    tabla_seg->pid = pid;
    tabla_seg->direcion_fisica=dir;
    tabla_seg->index=index;
    tabla_seg->segmento = crear_segmento(base,size); 

    return tabla_seg;
}
t_tabla_segmentos* buscar_en_tabla(int pid){}
int buscar_en_tabla_index(int pid){
    for (int i = 0; i < list_size(tabla_segmentos_gral); i++) {
        t_tabla_segmentos* tabla = list_get(tabla_segmentos_gral, i);
        if (tabla->pid == pid) {
            return i;
        }
    }

    return NULL;
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
    t_list_iterator* iterator= list_iterator_create(tabla_segmentos_gral);
    while (list_iterator_has_next(iterator)) {
        t_tabla_segmentos* tabla = list_iterator_next(iterator);
        if (tabla->pid == pid) {
            eliminar_segmento(pid, tabla->direcion_fisica);
        }
        free(tabla);
    }
    
    list_iterator_destroy(iterator);
    
}

int buscar_hueco_libre(int size){
   int index = -2;
   switch (config_memo.algoritmo_int)
   {
    case FIRST_FIT:
    index = first_fit(size);
    break;
    case BEST_FIT:
    index = best_fit(size);
    case WORST_FIT:
    index = worst_fit(size);
   default:
    break;
   }
}
t_hueco_libre* crear_hueco_libre(int inicio, int tam, int estado){
    t_hueco_libre* hueco = malloc(sizeof(t_hueco_libre));
    hueco->inicio = inicio;
    hueco->tamanio = tam;
    hueco->estado = estado;

    return hueco;
}
void eliminar_segmento_list(t_segmento* segmento, t_list* segmentos){
    list_remove_by_condition(segmentos, (void*) free);
    free(segmento);
}

void eliminar_segmento_por_id(int id_seg, t_list* segmentos){
    t_segmento* segmento = buscar_segmento(id_seg, segmentos);
    eliminar_segmento_list(segmento, segmentos);
}
t_list* crear_tabla_proceso(int pid){
    t_list* lista=list_create();

    t_segmento* segmento = crear_segmento(0,config_memo.tam_seg_0);
    list_add_in_index(lista,0,segmento);
    for (size_t i = 1; i < config_memo.cant_seg; i++)
    {
        segmento->base=0;
        segmento->size=0;
        list_add_in_index(lista,i,segmento);
    }

    imprimir_tabla(lista);
    //free(segmento);   // Si liberas el segmento despues de crearlo, lo estas perdiendo por completo. Cuando lo intentes mandar al kernel, no va a existir.
    return lista;
}
void imprimir_tabla(t_list* lista){
    for (size_t i = 0; i < list_size(lista); i++)
    {
        t_segmento* segmento = list_get(lista,i);
        printf("%d - base: %d, size: %d\n",i,segmento->base,segmento->size);
    }
    
}
void actualizar_memoria(int size, int indice, int estado){
    list_replace(huecos_libres,indice,estado);
    if(estado){
        config_memo.bytes_libres=config_memo.bytes_libres+size;
    }else{
        config_memo.bytes_libres=config_memo.bytes_libres-size;
    }
}
void imprimir_tabla_gral(){
    t_list_iterator* iterador = list_iterator_create(tabla_segmentos_gral);
    int i = 0;
    t_tabla_segmentos* tabla;
    while(list_iterator_has_next(iterador)){
        tabla= list_iterator_next(iterador);
        printf("%d pid: %d, dir: %d, index: %d, base: %d, size: %d\n",i,tabla->pid,tabla->direcion_fisica,tabla->index,tabla->segmento->base,tabla->segmento->size);
        i++;
        free(tabla);
    }
    list_iterator_destroy(iterador);
}

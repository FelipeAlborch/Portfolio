#include <memoriaUtils.h>
t_list* huecos_libres;
t_list *tabla_segmentos_gral;
//config_de_memoria config_memo;


void crear_estructuras(){
    memoria=malloc(config_memo.tam_memo);
    huecos_libres = list_create();
    int cero= config_memo.tam_seg_0;
    t_hueco_libre* hueco = malloc(sizeof(t_hueco_libre));
    hueco->estado = LIBRE;
    hueco->inicio = 0;
    hueco->tamanio = config_memo.tam_memo;    
    list_add(huecos_libres,hueco);
    modificar_hueco(0,0,cero,OCUPADO);
    inicializar_segmentos();
    
    free(hueco);
}
void inicializar_segmentos(){
    t_tabla_segmentos* tabla = crear_tabla_segmentos(0,0,0,config_memo.tam_seg_0);
    tabla_segmentos_gral = list_create();
    
    tabla->segmento->size = config_memo.tam_seg_0;
    list_add_in_index(tabla_segmentos_gral,0,tabla);
    
    
    free(tabla);
}
void modificar_tabla_segmentos(t_tabla_segmentos* tabla,int pid, int dir, int id,int base, int size){
    tabla->pid = pid;
    tabla->direcion_fisica = dir;
    tabla->index = id;
    tabla->segmento->base = base;
    tabla->segmento->size = size;
}
t_segmento* crear_segmento(int base, int size){
    t_segmento* segmento = malloc(sizeof(t_segmento));
    
    segmento->base=base;
    segmento->size=size;
    
    return segmento;
}



void* leer_dato(int pid, int direccion, int size){
	void* info;
	return info;
}
void escribir_dato(int pid, int direccion, int size, void* valor){

}

void eliminar_segmento(int pid, int id){
    int indice = buscar_en_tabla_id(pid,id);
    t_tabla_segmentos* tabla = list_get(tabla_segmentos_gral,indice);
    
    int base = buscar_hueco_base(tabla->segmento->base);
    modificar_hueco(base,tabla->segmento->base,tabla->segmento->size,LIBRE);
    modificar_tabla_segmentos(tabla,pid,-1,id,-1,-1);

    list_replace(tabla_segmentos_gral,indice,tabla);    
    t_list* nueva = tabla_proceso(pid);

    if (list_size(nueva) > 1)
    {
        t_paquete* paquete = crear_paquete_operacion(DELETE_SEGMENT);
        serializar_tabla_segmentos(paquete, nueva);
        enviar_paquete(paquete,config_memo.kernel);
        eliminar_paquete(paquete);
    }
    list_destroy(nueva);
    free(tabla->segmento);
    free(tabla);
}

void compactar(/* TO DO */){
    recibir_operacion(config_memo.kernel);
    loggear(INICIO_COMPACTAR,0,NULL,0,0,0);
}



t_tabla_segmentos* crear_tabla_segmentos(int pid, int index, int base, int size){
	t_tabla_segmentos* tabla_seg = malloc(sizeof(t_tabla_segmentos));
    int dir= base + size;
    
    tabla_seg->pid = pid;
    tabla_seg->direcion_fisica=dir;
    tabla_seg->index=index;
    tabla_seg->segmento = crear_segmento(base,size); 

    return tabla_seg;
}

int buscar_en_tabla_index(int pid){
    for (int i = 0; i < list_size(tabla_segmentos_gral); i++) {
        t_tabla_segmentos* tabla = list_get(tabla_segmentos_gral, i);
        if (tabla->pid == pid) {
            printf("El indice es: %d\n",i);
            return i;
        }
    }

    return M_ERROR;
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
            if (tabla->segmento->size > 0)
            {
                int index = buscar_hueco_base(tabla->segmento->base);
                modificar_hueco(index,tabla->segmento->base,tabla->segmento->size,LIBRE);
            }
            
            list_remove_element(tabla_segmentos_gral, tabla);
        }
        free(tabla);
    }
    
    list_iterator_destroy(iterator);
    
}
void* buscar_en_tabla_id(int pid, int id){
     
    t_tabla_segmentos* tabla1;
    
    bool _buscar_en_tabla (t_tabla_segmentos* tabla) {
        if(tabla->pid == pid && tabla->index == id){
            return true;
        }else{
            return false;
        }
        
    }
    tabla1 = list_find(tabla_segmentos_gral, (void*) _buscar_en_tabla); 
    if (tabla1 == NULL) {
        log_error(mlogger,"No se encontro el segmento");
        tabla1= crear_tabla_segmentos(M_ERROR,M_ERROR,M_ERROR,M_ERROR);
        return tabla1;
    
    }
    return tabla1;
}
int buscar_hueco_libre(int size){
   int index = -2;
   switch (config_memo.algoritmo_int)
   {
    case FIRST_FIT:
    index = first_fit(size);
    log_info(mlogger,"El indice del hueco libre es: %d",index);
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


t_list* crear_tabla_proceso(int pid){
    t_list* lista = list_create();
    int cero = config_memo.tam_seg_0;
    t_tabla_segmentos* tabla_seg = crear_tabla_segmentos(pid,0,0,cero);
    
    t_segmento* segmento = crear_segmento(0,cero);
    
    int indice =list_size(tabla_segmentos_gral);
    list_add(tabla_segmentos_gral,tabla_seg);
    
    list_add_in_index(lista,0,segmento); 
    int i = 1;
    for (i; i < config_memo.cant_seg; i++)
    {
        segmento = crear_segmento(0,0);
        tabla_seg = crear_tabla_segmentos(pid,i,0,0);
        list_add_in_index(lista,i,segmento);
        list_add(tabla_segmentos_gral,tabla_seg);
    
    }
    
    /* modificar_tabla_segmentos(tabla_seg,pid,cero,0,0,cero);
    segmento->size=cero;
    
    list_replace(tabla_segmentos_gral,indice + 1,tabla_seg); */
    /* segmento->base=cero+1;
    segmento->size=0;
    list_replace(lista,1,segmento); */
    
    free(tabla_seg->segmento);
    free(tabla_seg);
    free(segmento);   // Si liberas el segmento despues de crearlo, lo estas perdiendo por completo. Cuando lo intentes mandar al kernel, no va a existir.
    imprimir_tabla(lista);
    return lista;
}
void imprimir_tabla(t_list* lista){
    int size = list_size(lista);
 /*    t_segmento* segmento0; 
    segmento0 = list_get(lista,0);
    printf("%d - base: %d, size: %d\n",0,segmento0->base,segmento0->size);

    t_segmento* segmento1; 
    segmento1 = list_get(lista,1);
    printf("%d - base: %d, size: %d\n",1,segmento1->base,segmento1->size);

    segmento0 = list_get(lista,2);
    printf("%d - base: %d, size: %d\n",2,segmento0->base,segmento0->size); */
    
    for (size_t i = 0; i < size ; i++)
    {
        t_segmento* segmentos = list_get(lista,i);
        printf("%d - base: %d, size: %d\n",i,segmentos->base,segmentos->size);
    } 
     
}

void imprimir_tabla_gral(){
    t_list_iterator* iterador = list_iterator_create(tabla_segmentos_gral);
    int i = 0;

    while(list_iterator_has_next(iterador)){
       t_tabla_segmentos* tabla= list_iterator_next(iterador);
        printf("%d pid: %d, dir: %d, index: %d, base: %d, size: %d\n",i,tabla->pid,tabla->direcion_fisica,tabla->index,tabla->segmento->base,tabla->segmento->size);
        i++;
       free(tabla);
    }
    list_iterator_destroy(iterador);
}

void modificar_tabla_proceso(int pid, int index, int base, int size){
   /* 
    t_tabla_segmentos* tabla1 = crear_tabla_segmentos(pid,index,base,size);
    log_debug(mlogger,"pid: %d, dir: %d, index: %d, base: %d, size: %d",tabla1->pid,tabla1->direcion_fisica,tabla1->index,tabla1->segmento->base,tabla1->segmento->size);
     */
    bool _buscar_en_tabla (t_tabla_segmentos* tabla) {
        if (tabla->pid == pid && tabla->index == index){
            tabla->direcion_fisica = base + size;
            tabla->segmento->base = base;
            tabla->segmento->size = size;

            return true;
        }else{
            return false;
        }
        
    }
    
    //list_remove_and_destroy_by_condition(tabla_segmentos_gral, (void*) _buscar_en_tabla , tabla1);  // No se si esto esta bien, pero es lo que se me ocurrio. Si no funciona, lo podemos cambiar.
    t_tabla_segmentos* tabla1 =list_find(tabla_segmentos_gral, (void*) _buscar_en_tabla);
    tabla1 = buscar_en_tabla_id(pid,index);
    log_warning(mlogger,"pid: %d, dir: %d, index: %d, base: %d, size: %d",tabla1->pid,tabla1->direcion_fisica,tabla1->index,tabla1->segmento->base,tabla1->segmento->size);
    
    free(tabla1->segmento);
    free(tabla1);
}
void modificar_hueco(int index, int inicio, int tam, int estado){

    if(estado == OCUPADO){
        t_hueco_libre* hueco = list_get(huecos_libres,index);
        int anterior = hueco->tamanio;
        inicio = hueco->inicio;
        hueco->tamanio = tam;
        hueco->estado = estado;
        list_replace(huecos_libres,index,hueco);
        if(anterior - tam > 0){
            hueco->estado = LIBRE;
            hueco->inicio = inicio + tam+1;
            hueco->tamanio = anterior - tam;
            list_add_in_index(huecos_libres,index+1,hueco); 
            
        }
        
        
        actualizar_memoria(tam,OCUPADO);
        return;
    }
    liberar_hueco(index);
    actualizar_memoria(tam,LIBRE);
}
void liberar_hueco(int index){
    t_hueco_libre* hueco = list_get(huecos_libres,index);
    if (list_size(huecos_libres) >= index + 1)
    {
       t_hueco_libre* huecoSig = list_get(huecos_libres,index + 1);
       if (huecoSig->estado == LIBRE)
        {
            
            hueco->tamanio = hueco->tamanio + huecoSig->tamanio;
            list_remove(huecos_libres,index + 1);
        }
        free(huecoSig);
    }
    
    hueco->estado = LIBRE;
    list_replace(huecos_libres,index,hueco);
    free(hueco);
    if (index > 1)
    {
        t_hueco_libre* huecoAnt = list_get(huecos_libres,index - 1);
        if (huecoAnt->estado == LIBRE)
        {
            liberar_hueco(index - 1);
        }
        free(huecoAnt);
    }
}
int buscar_hueco_base(int base){
    for (int i = 0; i < list_size(huecos_libres); i++) {
        t_hueco_libre* tabla = list_get(tabla_segmentos_gral, i);
        if (tabla->inicio == base) {
            return i;
        }
    }
    return M_ERROR;
}
int base_hueco(int index){
    t_hueco_libre* hueco = list_get(huecos_libres,index);
    printf("base hueco: %d\n",hueco->inicio);
    return hueco->inicio;
}
t_list* tabla_proceso(int pid){
    t_list* listar=list_create();
    for (int i = 0; i < list_size(tabla_segmentos_gral); i++) {
        t_tabla_segmentos* tabla = list_get(tabla_segmentos_gral, i);
        if (tabla->pid == pid) {
            list_add(listar,tabla->segmento);
        }
    }
    return listar;
}   
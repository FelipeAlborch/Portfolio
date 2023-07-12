#include <memoriaUtils.h>
//t_log* klogger;
int running_k;
void conectar_kernel(){
    pthread_mutex_lock(&m_config);
    config_memo.kernel=esperar_cliente(server_m);
    pthread_mutex_unlock(&m_config);

    t_paquete* paquete; 
    
    pthread_mutex_lock(&m_config);
    paquete = recibir_paquete(config_memo.kernel);
    pthread_mutex_unlock(&m_config);

    if(paquete->codigo_operacion != KERNEL){
        log_error(klogger,"Vos no sos el kernel. Se cancela la conexión %d",paquete->codigo_operacion);
        paquete_destroy(paquete);
       
		pthread_exit(&hilo_kernel);
    }
    log_info(klogger,"Se conectó el kernel: %d \n",config_memo.kernel);
		
    paquete_destroy(paquete);
    running_k=true;
    ejecutar_kernel();        //DESCOMENTAR ESTA LINEA PARA EJECUTAR EL KERNEL
    //ejecutar_kernel_test();     //COMENTAR ESTA LINEA PARA EJECUTAR EL KERNEL
}
void ejecutar_kernel(){
    pthread_mutex_lock(&m_config);
    int conectar=config_memo.kernel;
    pthread_mutex_unlock(&m_config);

    log_trace(mlogger, "Por ejecutar las tareas del kernel");

    t_list* lista;// = list_create();
    int pid = -1;
    int run = 6;
    while (running_k) {
    
        switch (recibir_operacion(conectar))
        {
            case INICIO_PROCESO:
                lista = _recibir_paquete(conectar); // Agregue esta linea
                pid=*(int*)list_get(lista,0);   // Cambie el 1 por un 0
                crear_proceso(pid);
                
                break;
            case FIN_PROCESO:
                lista = _recibir_paquete(conectar);    // Agregue esta linea
                pid=*(int*)list_get(lista,0);   // Cambie el 1 por un 0
                eliminar_proceso(pid);
                
                break;
            case CREATE_SEGMENT:
                lista = _recibir_paquete(conectar);    // Agregue esta linea
                int id=*(int*)list_get(lista,0);    // Cambie los indices
                pid=*(int*)list_get(lista,2);
                int tam=*(int*)list_get(lista,1);
                create_segment(pid,tam,id);
                
                break;
            case DELETE_SEGMENT:
                lista = _recibir_paquete(conectar);    // Agregue esta linea
                id=*(int*)list_get(lista,0);    // Cambie los indices
                pid=*(int*)list_get(lista,1);
                
                eliminar_segmento(pid, id);
                
                break;
            case FIN_MODULO:
                log_info(klogger,"Se desconecto el kernel");
                running_k=false;
                break;
            default:
            break;
        }

    list_destroy_and_destroy_elements(lista,free);
    
    }
    //list_destroy_and_destroy_elements(lista,free);

    log_info(klogger,"Terminando de ejecutar las tareas del kernel");

    terminar_programa(loggerMemoria);
      //imprimir_tabla_gral();
}


void crear_proceso(int pid){

    t_list* listaS=crear_tabla_proceso(pid);
    loggear(INICIO_PROCESO,pid,NULL,0,0,0);

    respuestas(config_memo.kernel,INICIO_PROCESO,listaS);
    list_destroy_and_destroy_elements(listaS,free);
}

void eliminar_proceso(int pid){
    int indice= buscar_en_tabla_index(pid);
    

    if(indice== M_ERROR){
        log_error(klogger,"No se encontro el proceso %d",pid);
        //respuestas(config_memo.kernel,FIN_PROCESO,NULL);
        return;
    }
    liberar_proceso(pid);
    log_trace(klogger,"Se elimino el proceso %d",pid); 
    loggear(FIN_PROCESO,pid,NULL,0,0,0);
    //imprimir_huecos();
    //imprimir_tabla_gral();
}


void create_segment(int pid,int tam,int id){
    
    int bytes=config_memo.bytes_libres;

    log_debug(klogger,"Por crear el segmento %d del proceso %d de %d",id,pid,tam);
    if ( bytes < tam){

        respuestas(config_memo.kernel,OUT_OF_MEMORY,NULL);

        log_error(klogger,"No hay memoria suficiente para crear el segmento");
        loggear(OUT_OF_MEMORY,pid,NULL,tam,0,0);
        return;
    }
    int indice =buscar_hueco_libre(tam);
    if(indice == -2){
        respuestas(config_memo.kernel,INICIO_COMPACTAR,NULL);
        
        log_warning(klogger,"No hay hueco para crear el segmento, hay que compactar");
        compactar();
        tablas_compactadas();
        respuestas(config_memo.kernel,FIN_COMPACTAR,NULL);
        
        return;
    }else{
        modificar_hueco(indice,M_ERROR,tam,OCUPADO);
        int base = base_hueco(indice);
        modificar_tabla_proceso(pid,id,base,tam);
        respuestas(config_memo.kernel,CREATE_SEGMENT,&base);
        log_info(klogger,"Se creo el segmento %d",id);
    
        loggear(CREATE_SEGMENT,pid,NULL,id,tam,base);
    }   
    
}
void eliminar_segmento(int pid, int id){
    
    t_tabla_segmentos* tabla = buscar_en_tabla_id(pid,id);
    
    if (tabla->index == M_ERROR){
        log_error(klogger,"No se encontro el segmento %d del proceso %d",id,pid);
        respuestas(config_memo.kernel,M_ERROR,NULL);
        loggear(M_ERROR,pid,NULL,id,0,0);
        return;
    }
    modificar_hueco(M_ERROR,tabla->segmento->base,tabla->segmento->size,LIBRE);
    modificar_tabla_segmentos(tabla,pid,-1,id,-1,-1);

    modificar_tabla_proceso(pid,id,0,0);  
    t_list* nueva = tabla_proceso(pid);

    if (list_size(nueva) > 1)
    {
        t_paquete* paquete = crear_paquete_operacion(DELETE_SEGMENT);
        serializar_tabla_segmentos(paquete, nueva);
        enviar_paquete(paquete,config_memo.kernel);
        eliminar_paquete(paquete);
        imprimir_tabla(nueva);
    }
    list_destroy(nueva);
    loggear(DELETE_SEGMENT,pid,NULL,id,0,0);
    log_info(klogger,"Se elimino el segmento %d",id);
}
void tablas_compactadas(){
    t_list* lista = list_create();
    t_paquete* paquete;
    
    int size = list_size(tabla_segmentos_gral);
    int i = 1;
    int n = config_memo.cant_seg;
    int pid;
    while(i < size){
        for(int j = 0; j < n; j++){
            t_tabla_segmentos* tabla = list_get(tabla_segmentos_gral,i+j);
            list_add(lista,tabla->segmento);
            loggear(FIN_COMPACTAR,tabla->pid,NULL,tabla->index,tabla->segmento->size,tabla->segmento->base);
            pid = tabla->pid;
        }
        
        paquete = crear_paquete_operacion(INICIO_COMPACTAR);
        agregar_a_paquete(paquete,&pid,sizeof(int));
        serializar_tabla_segmentos(paquete,lista);
        enviar_paquete(paquete,config_memo.kernel);
        eliminar_paquete(paquete); 

       // imprimir_tabla(lista);
        i = i + n; 
        list_clean(lista);
       // list_clean_and_destroy_elements(lista,free); 
    }
}
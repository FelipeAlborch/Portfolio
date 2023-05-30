#include <memoriaUtils.h>
//t_log* klogger;
int running_k;
void conectar_kernel(){
    config_memo.kernel=esperar_cliente(server_m);
    t_paquete* paquete; // =malloc(TAM_PAQ);
    paquete = recibir_paquete(config_memo.kernel);
    if(paquete->codigo_operacion != KERNEL){
        log_error(klogger,"Vos no sos el kernel. Se cancela la conexión %d",paquete->codigo_operacion);
        eliminar_paquete(paquete);
        pthread_detach(hilo_kernel);
		pthread_exit(&hilo_kernel);
    }
    log_info(klogger,"Se conectó el kernel: %d \n",config_memo.kernel);
		
    //eliminar_paquete(paquete);
    running_k=true;
    ejecutar_kernel();        //DESCOMENTAR ESTA LINEA PARA EJECUTAR EL KERNEL
    //ejecutar_kernel_test();     //COMENTAR ESTA LINEA PARA EJECUTAR EL KERNEL
}
void ejecutar_kernel(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");

   
    t_list* lista;
    int pid = -1;
    int run = 5;
    while (run > 0) {
    
        //lista=_recibir_paquete(conectar);
        //int codigo=*(int*)list_get(lista,0);
        switch (recibir_operacion(config_memo.kernel))
        {
          case INICIO_PROCESO:
            lista = _recibir_paquete(config_memo.kernel); // Agregue esta linea
            pid=*(int*)list_get(lista,0);   // Cambie el 1 por un 0
            crear_proceso(pid);
            
          
            break;
          case FIN_PROCESO:
            lista = _recibir_paquete(config_memo.kernel);    // Agregue esta linea
            pid=*(int*)list_get(lista,0);   // Cambie el 1 por un 0
            eliminar_proceso(pid);

            break;
          case CREATE_SEGMENT:
            lista = _recibir_paquete(config_memo.kernel);    // Agregue esta linea
            int id=*(int*)list_get(lista,0);    // Cambie los indices
            pid=*(int*)list_get(lista,2);
            int tam=*(int*)list_get(lista,1);
            create_segment(pid,tam,id);
            break;
          case DELETE_SEGMENT:
            lista = _recibir_paquete(config_memo.kernel);    // Agregue esta linea
            id=*(int*)list_get(lista,0);    // Cambie los indices
            pid=*(int*)list_get(lista,1);
            eliminar_segmento(pid, id);
            break;
           
          default:
            break;
      }
      run--;
      
      //running_k=false;    // Por que pones esto en fales?, hace que salga del while, y no se quede esperando a la proxima tarea del kernel
      log_trace(klogger,"ejecute kernel");
  }
  list_destroy(lista);
  //eliminar_paquete(paquete);  // Este free esta tirando seg fault porque la funcion _recibir_paquete(socket) no recibe realmente un t_paquete, sino que enlista los valores que haya en el buffer del socket
  log_info(klogger,"Terminando de ejecutar las tareas del kernel");
  imprimir_tabla_gral();
}


void crear_proceso(int pid){

    t_list* listaS=crear_tabla_proceso(pid);
    loggear(INICIO_PROCESO,pid,NULL,0,0,0);
    

    t_paquete* paquete = crear_paquete_operacion(INICIO_PROCESO);
    serializar_tabla_segmentos(paquete,listaS);
    enviar_paquete(paquete,config_memo.kernel);
    
    
    eliminar_paquete(paquete);
    //list_destroy(listaS);
}

void eliminar_proceso(int pid){
    int indice= buscar_en_tabla_index(pid);
    

    if(indice== M_ERROR){
        log_error(klogger,"No se encontro el proceso %d",pid);
        t_paquete* paquete = crear_paquete_operacion(FIN_PROCESO);
        respuestas(config_memo.kernel,FIN_PROCESO,paquete);
        eliminar_paquete(paquete);
        return;
    }
    liberar_proceso(pid);
    log_trace(klogger,"Se elimino el proceso %d",pid);
    
    t_paquete* paquete = crear_paquete_operacion(FIN_PROCESO);
    
    enviar_paquete(paquete,config_memo.kernel);
    
    eliminar_paquete(paquete);
    loggear(FIN_PROCESO,pid,NULL,0,0,0);
}

void ejecutar_kernel_test(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");
    int pid =221;
    int tam = 4096 + 4096;
    crear_proceso(pid);
    log_trace(klogger,"ejecute la creación del proceso %d",pid);
    sleep(1);
    sleep(1);

    create_segment(pid,tam,1);
    
}
void create_segment(int pid,int tam,int id){
    
    int bytes=config_memo.bytes_libres;

    log_debug(klogger,"Por crear el segmento %d del proceso %d de %d",id,pid,bytes);
    if ( bytes < tam){
        respuestas(config_memo.kernel,OUT_OF_MEMORY,M_ERROR);
        log_error(klogger,"No hay memoria suficiente para crear el segmento");
        loggear(OUT_OF_MEMORY,pid,NULL,tam,0,0);
        return;
    }
    int indice =buscar_hueco_libre(tam);
    if(indice == -2){
        respuestas(config_memo.kernel,INICIO_COMPACTAR,NULL);
        log_warning(klogger,"No hay hueco para crear el segmento, hay que compactar");
        compactar();
        
        return;
    }else{
        modificar_hueco(indice,-1,tam,OCUPADO);
        
        //t_paquete* paquete = crear_paquete_operacion(CREATE_SEGMENT);
        int base = base_hueco(indice);
        //agregar_a_paquete(paquete, &base, sizeof(int));
        //enviar_paquete(paquete, config_memo.kernel);
        //eliminar_paquete(paquete);
        
        modificar_tabla_proceso(pid,id,base,tam);
        respuestas(config_memo.kernel,CREATE_SEGMENT,base);
        log_info(klogger,"Se creo el segmento %d",id);
      
        loggear(CREATE_SEGMENT,pid,NULL,id,tam,base);
    }   
    
}

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
		
    eliminar_paquete(paquete);
    running_k=true;
    //ejecutar_kernel();
    ejecutar_kernel_test();
}
void ejecutar_kernel(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");

    t_paquete* paquete; // =malloc(size_of(t_paquete));
    t_list* lista;
    int pid = -1;
    while (running_k)
    {
    
        lista=_recibir_paquete(conectar);
        int codigo=*(int*)list_get(lista,0);
        switch (codigo)
        {
          case INICIO_PROCESO:
            pid=*(int*)list_get(lista,1);
            crear_proceso(pid);
            
            break;
          case FIN_PROCESO:
            pid=*(int*)list_get(lista,1);
            eliminar_proceso(pid);

            break;
          case CREATE_SEGMENT:
            int id=*(int*)list_get(lista,1);
            pid=*(int*)list_get(lista,3);
            int tam=*(int*)list_get(lista,2);
            
            break;
          //case 
          default:
            break;
      }
      list_destroy(lista);
      running_k=false;
      log_trace(klogger,"ejecute kernel");
  }
  eliminar_paquete(paquete);
  log_info(klogger,"Terminando de ejecutar las tareas del kernel");
}


void crear_proceso(int pid){

    t_list* listaS=crear_tabla_proceso(pid);
    loggear(INICIO_PROCESO,pid,NULL,0,0,0);
    
    t_paquete* paquete = malloc(TAM_PAQ);
    serializar_tabla_segmentos(paquete,listaS);
    
    respuestas(config_memo.kernel,INICIO_PROCESO,paquete);
    
    eliminar_paquete(paquete);
    list_destroy(listaS);
}

void eliminar_proceso(int pid){
    int indice= buscar_en_tabla_index(pid);
    t_paquete* paquete = malloc(TAM_PAQ);

    if(indice==-1){
        log_error(klogger,"No se encontro el proceso %d",pid);
        respuestas(config_memo.kernel,FIN_PROCESO,paquete);
        return;
    }
    liberar_proceso(pid);
    log_trace(klogger,"Se elimino el proceso %d",pid);
    
    respuestas(config_memo.kernel,FIN_PROCESO,paquete);
    eliminar_paquete(paquete);
    loggear(FIN_PROCESO,pid,NULL,0,0,0);
}

void ejecutar_kernel_test(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");
    int pid =221;
    crear_proceso(pid);
    log_trace(klogger,"ejecute la creación del proceso %d",pid);
    
}
void create_segment(int pid,int tam,int id){
    if (config_memo.bytes_libres<tam)
    {
        respuestas(config_memo.kernel,OUT_OF_MEMORY,NULL);
        log_error(klogger,"No hay memoria suficiente para crear el segmento");
        loggear(OUT_OF_MEMORY,pid,NULL,tam,0,0);
        return;
    }else
    {
        /* code */
    }
    
    
    crear_segmento(pid,tam);
    loggear(CREATE_SEGMENT,pid,NULL,tam,0,0);
}
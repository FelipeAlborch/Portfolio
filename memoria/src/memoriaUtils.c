#include <memoriaUtils.h>
//#include "../include/memoriaUtils.h"

t_log* mlogger;
t_log* klogger;
extern t_log* clogger;
extern t_log* flogger;
t_log* loggerMemoria;
int conexion;
extern int running_cpu;
//int running_fs;
//int running_k;
int server_m;
int clientes[4];
t_config* memoriaConfig;
pthread_t hiloConexion;
pthread_mutex_t m_config;
pthread_mutex_t m_memoria;
pthread_mutex_t m_tabla_segmentos;
pthread_mutex_t m_huecos_libres;
sem_t sem_fs_conectado;
sem_t sem_cpu_conectado;

void* memoria;


void startSigHandlers(void) {
	signal(SIGINT, sigHandler_sigint);
}

void sigHandler_sigint(int signo) {
	log_warning(loggerMemoria, "Tiraste un CTRL+C, macho, abortaste el proceso.");

	terminar_programa(loggerMemoria);
	printf("-------------------FINAL POR CTRL+C-------------------\n");

	exit(-1);
}


/**
 * 
 *          FUNCIONES PARA TERMINAR EL PROGRAMA
 * 
*/
void terminar_programa(t_log* milogger){
    /*LEAKS*/
	log_destroy(milogger);
	liberar_conexion_memoria();
  
	
  liberar_listas();
  liberar_t_config();
  liberar_mutex();
  liberar_hilos();
  liberar_memoria();
    
    
  printf("----------FIN------------\n");
  
};
void liberar_memoria(){
    free(memoria);
    log_debug(mlogger,"memoria liberada");  
    log_destroy(clogger);
    log_destroy(flogger);
    log_destroy(klogger);
    log_debug(mlogger,"logs liberados");
    log_destroy(mlogger);
};
void liberar_listas(){
    list_destroy_and_destroy_elements(tabla_segmentos_gral,(void*)liberar_t_segmento);
    list_destroy_and_destroy_elements(huecos_libres,free);
    log_debug(mlogger,"listas liberadas");
};
void liberar_huecos(t_hueco_libre* hueco){
    free(hueco->estado);
    /* free(hueco->inicio);
    free(hueco->tamanio); */ 
    free(hueco);
};
void liberar_t_segmento(t_tabla_segmentos* segmento){
    free(segmento->segmento);
    free(segmento);
};  
void liberar_conexion_memoria(){
    
    liberar_conexion(config_memo.cpu);
    liberar_conexion(config_memo.fs);
    liberar_conexion(config_memo.kernel); 
    liberar_conexion(server_m);
    log_debug(mlogger,"conexiones liberadas");
};
void liberar_t_config(){
  //free(config_memo.algoritmo);
  free(config_memo.ip);
  //free(config_memo.puerto);
  log_debug(mlogger,"config liberada");
};
void liberar_mutex(){
    pthread_mutex_destroy(&m_config);
    pthread_mutex_destroy(&m_memoria);
    pthread_mutex_destroy(&m_tabla_segmentos);
    pthread_mutex_destroy(&m_huecos_libres);
    log_debug(mlogger,"mutex liberados");
};
void liberar_hilos(){
    pthread_cancel(&hilo_cpu);
    pthread_cancel(&hilo_fs);
    pthread_cancel(&hilo_kernel);
    log_debug(mlogger,"hilos liberados");
};
/**
 * 
 *          FUNCIONES PARA INICIAR PROGRAMA
 * 
*/
void inicializar_mutex(){
    pthread_mutex_init(&m_config,NULL);
    pthread_mutex_init(&m_memoria,NULL);
    pthread_mutex_init(&m_tabla_segmentos,NULL);
    pthread_mutex_init(&m_huecos_libres,NULL);
   // log_debug(mlogger,"mutex inicializados");
};
void inicializar_configuracion(){
    obtener_valores_de_configuracion_memoria(memoriaConfig);
    mostrar_valores_de_configuracion_memoria();
    sem_init(&sem_fs_conectado,0,0);
    sem_init(&sem_cpu_conectado,0,0);
};
void inicializar_memoria(){

    inicializar_logs();
    inicializar_mutex();
    inicializar_configuracion();
    
    crear_estructuras();   
    conectar();
    config_destroy(memoriaConfig);
};
void inicializar_logs(){
    loggerMemoria = log_create("logs/memoria.log","Memoria",true,LOG_LEVEL_TRACE);
    mlogger = log_create("logs/info.log","Info Memoria",true,LOG_LEVEL_TRACE);
    klogger = log_create("logs/kernel.log","Memoria -> Kernel",false,LOG_LEVEL_TRACE);
    clogger = log_create("logs/cpu.log","Memoria -> CPU",false,LOG_LEVEL_TRACE);
    flogger = log_create("logs/file_system.log","Memoria -> FileSystem",false,LOG_LEVEL_TRACE);
}
void inicializar_segmentos(){
    t_tabla_segmentos* tabla = crear_tabla_segmentos(0,0,0,config_memo.tam_seg_0);
    tabla_segmentos_gral = list_create();
    
    tabla->segmento->size = config_memo.tam_seg_0;
    list_add_in_index(tabla_segmentos_gral,0,tabla);
}

/**
 * 
 *          CONEXION
 * 
*/
void conectar(){

  server_m= iniciar_servidor_en(config_memo.ip,config_memo.puerto);
  if(server_m < 0){
    log_error(mlogger,"Error creando el servicio de memoria");
    return EXIT_FAILURE;
  }
  log_info(mlogger, "El servidor Memoria se inició correctamente");
  
}
void respuestas(int cliente, int code,void* algo){
  if (algo == NULL){
    enviar_operacion(cliente,code);
    return;
  }

  //log_info(mlogger, "[RESPUESTAS]: %d - %s ", code, algo);
  
  t_paquete* paquete=crear_paquete_operacion(code);
  
  if(code == INICIO_PROCESO || code == DELETE_SEGMENT){
    serializar_tabla_segmentos(paquete,(t_list*)algo);
    enviar_paquete(paquete,config_memo.kernel);
    eliminar_paquete(paquete);
    return;
  }
  if(code == MOV_IN_SUCCES || code == M_READ){
    agregar_a_paquete(paquete,algo,strlen((char*)algo)+1);
    enviar_paquete(paquete,cliente);
    eliminar_paquete(paquete);
    return;
  }
  agregar_a_paquete(paquete,algo,sizeof(algo)+1);
  enviar_paquete(paquete,cliente);
  eliminar_paquete(paquete);
}
/**
 * 
 *          CONFIGURACION
 * 
*/
config_de_memoria config_memo;

void obtener_valores_de_configuracion_memoria(t_config* memoriaConfig){
    config_memo.ip = config_get_string_value(memoriaConfig, "IP_MEMORIA");
    config_memo.puerto = config_get_string_value(memoriaConfig, "PUERTO_ESCUCHA");
    config_memo.tam_memo = config_get_int_value(memoriaConfig, "TAM_MEMORIA");
    config_memo.tam_seg_0 = config_get_int_value(memoriaConfig,"TAM_SEGMENTO_0");
    config_memo.cant_seg = config_get_int_value(memoriaConfig,"CANT_SEGMENTOS");
    config_memo.retardo = config_get_int_value(memoriaConfig,"RETARDO_MEMORIA");
    config_memo.compactacion = config_get_int_value(memoriaConfig,"RETARDO_COMPACTACION");
    config_memo.algoritmo = config_get_string_value(memoriaConfig,"ALGORITMO_ASIGNACION");
    config_memo.cant_seg_max = config_memo.tam_memo / config_memo.tam_seg_0;
    
    if(config_memo.ip == NULL){
      config_memo.ip = string_duplicate(LOCALHOST);
    }
    
    config_memo.bytes_libres=config_memo.tam_memo;
    algoritmos();
}
void mostrar_valores_de_configuracion_memoria (){
    printf("ip = %s\n", config_memo.ip);
    printf("puerto = %s\n", config_memo.puerto);
    printf("tam_memo = %d\n" , config_memo.tam_memo);
    printf("tam_seg_0 = %d\n" , config_memo.tam_seg_0);
    printf("cant_seg = %d\n" , config_memo.cant_seg);
    printf("retardo = %d\n" , config_memo.retardo);
    printf("compactacion = %d\n", config_memo.compactacion);
    printf("algoritmo = %s, %d\n" , config_memo.algoritmo, config_memo.algoritmo_int);
    printf("Tamaño maximo = %d\n" , config_memo.cant_seg_max);

}

/**
 * 
 *          AUXILIARES
 * 
*/
void loggear(int code, int pid, void* algo, int id, int size, int base){
    
    switch (code){
      case CREATE_SEGMENT:
        log_info(loggerMemoria,"PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d",pid,id,base,size);
        break;
      case DELETE_SEGMENT:
        log_info(loggerMemoria,"PID: %d - Eliminar Segmento: %d - Base: %d - TAMAÑO: %d",pid,id,base,size);
        break;
      case INICIO_COMPACTAR:
        log_info(loggerMemoria,"Solicitud de Compactación");
        break;
      case FIN_COMPACTAR:
            /*Por cada segmento de cada proceso se deberá imprimir una línea con el siguiente formato:*/
        log_info(loggerMemoria,"PID: %d - Segmento: %d - Base: %d - Tamaño %d",pid,id,base,size);
        break;
      case M_READ:
        log_info(loggerMemoria,"PID: %d - Acción: <LEER> - Dirección física: %d - Tamaño: %d - Origen: %s",pid,id,size,algo);
        break;
      case MOV_IN_INSTRUCTION:
        log_info(loggerMemoria,"PID: %d - Acción: <LEER> - Dirección física: %d - Tamaño: %d - Origen: %s",pid,id,size,algo);
        break;
      case M_WRITE:
        log_info(loggerMemoria,"“PID: %d - Acción: <ESCRIBIR> - Dirección física: %d - Tamaño: %d - Origen: %s",pid,id,size,algo);
        break;
      case MOV_OUT_INSTRUCTION:
        log_info(loggerMemoria,"PID: %d - Acción: <ESCRIBIR> - Dirección física: %d - Tamaño: %d - Origen: %s",pid,id,size,algo);
        break;
      case INICIO_PROCESO:
        log_info(loggerMemoria,"Creación de Proceso PID: %d",pid);
        break;
      case FIN_PROCESO:
        log_info(loggerMemoria,"Eliminación de Proceso PID: %d",pid);
        break;
      case OUT_OF_MEMORY:
        log_error(loggerMemoria,"Error: OUT_OF_MEMORY");
        break;
      default:
        log_error(loggerMemoria,"Error en la operación, código de error: %d",code);
        break;
    }
  }
void actualizar_memoria(int size, int estado){
    pthread_mutex_lock(&m_config);
    if(estado == LIBRE){
        config_memo.bytes_libres=config_memo.bytes_libres+size;
    }else{
        config_memo.bytes_libres=config_memo.bytes_libres-size;
    }
    pthread_mutex_unlock(&m_config);
}
void imprimir_tabla(t_list* lista){
    int size = list_size(lista);
    for (size_t i = 0; i < size ; i++)
    {
        t_segmento* segmentos = list_get(lista,i);
        log_trace(mlogger,"%ld - base: %d, size: %d\n",i,segmentos->base,segmentos->size);    
    } 
}
void imprimir_tabla_gral(){
  pthread_mutex_lock(&m_tabla_segmentos);
    t_list_iterator* iterador = list_iterator_create(tabla_segmentos_gral);
  pthread_mutex_unlock(&m_tabla_segmentos);  
    int i = 0;

    while(list_iterator_has_next(iterador)){
        t_tabla_segmentos* tabla= list_iterator_next(iterador);
        //printf("%d- \tpid: %d, \tdir: %d, \tindex: %d,\tbase: %d,\t size: %d\n",i,tabla->pid,tabla->direcion_fisica,tabla->index,tabla->segmento->base,tabla->segmento->size);
        log_trace(mlogger,"%d- \tpid: %d, \tdir: %d, \tindex: %d,\tbase: %d,\t size: %d\n",i,tabla->pid,tabla->direcion_fisica,tabla->index,tabla->segmento->base,tabla->segmento->size);
        i++;
    }
    list_iterator_destroy(iterador);
}
void imprimir_huecos(){
  int inicio;
  int tam;
  pthread_mutex_lock(&m_huecos_libres);
    t_list_iterator* iterador = list_iterator_create(huecos_libres);
  pthread_mutex_unlock(&m_huecos_libres);
  t_hueco_libre* hueco;
 // log_debug(klogger,"Listado de Huecos libres: %d\n",list_size(huecos_libres));
  while (list_iterator_has_next(iterador)) {
    hueco = list_iterator_next(iterador);
    inicio = hueco->inicio; 
    tam = hueco->tamanio;
    int estado = hueco->estado;
    log_debug(klogger,"Hueco: %d - Base: %d  - Tamaño: %d - Estado: %d",list_iterator_index(iterador),inicio,tam,estado);    
    //free(hueco);
  }
  printf("\n");
  list_iterator_destroy(iterador);
  
}
/**
 * 
 *          ALGORITMOS
 * 
*/
void algoritmos(){
  if(strcmp(config_memo.algoritmo,"FF")==0){
    config_memo.algoritmo_int=FIRST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"FIRST")==0){
    config_memo.algoritmo_int=FIRST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"BEST")==0){
    config_memo.algoritmo_int=BEST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"BF")==0){
    config_memo.algoritmo_int=BEST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"WF")==0){
    config_memo.algoritmo_int=WORST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"WORST")==0){
    config_memo.algoritmo_int=WORST_FIT;
  }
  else{
    log_error(mlogger,"No se reconoce el algoritmo de asignación");
  }
}
int first_fit(int size){
  int index = -2; 
  t_list_iterator* iterador = list_iterator_create(huecos_libres);
  while (list_iterator_has_next(iterador)) {
    t_hueco_libre* hueco = list_iterator_next(iterador);
    if (hueco->tamanio >= size && hueco->estado == LIBRE) {
      index = list_iterator_index(iterador);
      
      list_iterator_destroy(iterador);
      return index;
    }
  }
  list_iterator_destroy(iterador);
  return index;
}
// 0-> ocupado 1-> libre 2-> no existe
int best_fit(int size){
  int index = -2;
  int min = config_memo.tam_memo + 1;

    for (int i = 0; i < list_size(huecos_libres); i++) {
        t_hueco_libre* hueco = list_get(huecos_libres, i);
        if (hueco->tamanio >= size && hueco->estado == LIBRE) {
            if (hueco->tamanio < min) {
                min = hueco->tamanio;
                index = i;
            }
        }
    }

    return index;
}
int worst_fit(int size){
  int index = -2;
    int max = -1;

    for (int i = 0; i < list_size(huecos_libres); i++) {
        t_hueco_libre* hueco = list_get(huecos_libres, i);
        if (hueco->tamanio >= size && hueco->estado == LIBRE) {
            if (hueco->tamanio > max) {
                max = hueco->tamanio;
                index = i;
            }
        }
    }

    return index;
}

/**
 * 
 *          COMPACTACION
 * 
*/
void compactar(){
    imprimir_huecos();
    //imprimir_tabla_gral();
    recibir_operacion(config_memo.kernel);
    loggear(INICIO_COMPACTAR,0,NULL,0,0,0); 
    sleep(config_memo.compactacion/1000); 
    
    bool _es_libre(t_hueco_libre* hueco){
        return hueco->estado == LIBRE;
    }
    pthread_mutex_lock(&m_huecos_libres);
    list_remove_and_destroy_all_by_condition(huecos_libres,(void*)_es_libre, (void*)free);
    int tam = list_size(huecos_libres);
    t_hueco_libre* hueco = list_get(huecos_libres,tam-1);
    int inicio = hueco->inicio + hueco->tamanio;

    t_hueco_libre* nuevo =crear_hueco_libre(inicio,config_memo.bytes_libres,LIBRE);
    list_add(huecos_libres,nuevo);
    pthread_mutex_unlock(&m_huecos_libres);
    //sleep(config_memo.compactacion/1000);
    
    mover_bases_huecos();
}
void mover_bases(int dir, int base){
    
    bool _es_dir(t_tabla_segmentos* tabla){
        if (tabla->pid !=0 && dir == tabla->direcion_fisica){
          
          modificar_tabla_proceso(tabla->pid,tabla->index,base,tabla->segmento->size);
          return true;
        }
        return false;
    }
    pthread_mutex_lock(&m_tabla_segmentos);
    bool i =list_any_satisfy(tabla_segmentos_gral,(void*)_es_dir);
    pthread_mutex_unlock(&m_tabla_segmentos);  
}
void mover_bases_huecos(){
  pthread_mutex_lock(&m_huecos_libres);
    t_list_iterator* iterador = list_iterator_create(huecos_libres);
  pthread_mutex_unlock(&m_huecos_libres);
  
  t_hueco_libre* hueco = calloc(0,sizeof(t_hueco_libre));
  int inicio;
  int tam=0;
  int baseNueva = config_memo.tam_seg_0;
  int dir =tam + baseNueva; 
  while (list_iterator_has_next(iterador)) {
    hueco = list_iterator_next(iterador);
    inicio = hueco->inicio; 
    tam = hueco->tamanio;
    if (inicio >= baseNueva){
      pthread_mutex_lock(&m_memoria);
        memcpy(memoria+baseNueva, memoria+inicio, tam);
      pthread_mutex_unlock(&m_memoria);
      dir = tam + inicio;
      mover_bases(dir,baseNueva);
      hueco->inicio = baseNueva;
      baseNueva = baseNueva + tam;
      
    }
  }
  //free(hueco);
  list_iterator_destroy(iterador);
  imprimir_huecos();
}
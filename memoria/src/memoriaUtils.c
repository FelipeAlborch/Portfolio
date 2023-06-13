#include <memoriaUtils.h>
//#include "../include/memoriaUtils.h"

t_log* mlogger;
t_log* klogger;
extern t_log* clogger;
t_log* flogger;
t_log* loggerMemoria;
int conexion;
extern int running_cpu;
int running_fs;
//int running_k;
int server_m;
int clientes[4];
t_config* memoriaConfig;
pthread_t hiloConexion;
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

void terminar_programa(t_log* milogger){
    /*LEAKS*/
	log_destroy(milogger);
	liberar_conexion_memoria();
  
	
  liberar_listas();
  liberar_t_config();
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
    list_destroy_and_destroy_elements(tabla_segmentos_gral,(void*)free);
    list_destroy_and_destroy_elements(huecos_libres,(void*)free);
    //list_destroy(huecos_libres);
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
    //free(segmento);
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
void inicializar_configuracion(){
    obtener_valores_de_configuracion_memoria(memoriaConfig);
    mostrar_valores_de_configuracion_memoria();
};

void inicializar_memoria(){

    inicializar_logs();
    inicializar_configuracion();
    
    crear_estructuras();   
    conectar();
    config_destroy(memoriaConfig);
};


void inicializar_logs(){
    loggerMemoria = log_create("logs/memoria.log","Memoria",true,LOG_LEVEL_TRACE);
    mlogger = log_create("logs/info.log","Info Memoria",false,LOG_LEVEL_TRACE);
    klogger = log_create("logs/kernel.log","Memoria -> Kernel",true,LOG_LEVEL_TRACE);
    clogger = log_create("logs/cpu.log","Memoria -> CPU",true,LOG_LEVEL_TRACE);
    flogger = log_create("logs/file_system.log","Memoria -> FileSystem",true,LOG_LEVEL_TRACE);
}
// Lo del memoria conexion: 


void conectar_fs(){
  config_memo.fs=esperar_cliente(server_m);
    t_paquete* paquete;// =malloc(TAM_PAQ);
    paquete = recibir_paquete(config_memo.fs);
    if(paquete->codigo_operacion != FILE_SYSTEM){
      log_error(flogger,"Vos no sos el FS. Se cancela la conexión %d ",paquete->codigo_operacion);
     // eliminar_paquete(paquete);
      pthread_detach(hilo_fs);
			pthread_exit(&hilo_fs);
    }
    log_info(flogger,"Se conectó el FileSystem: %d \n",config_memo.fs);
		

    free(paquete->buffer);
    free(paquete);

    running_fs=true;
    ejecutar_fs();
}
void conectar(){

  server_m= iniciar_servidor_en(config_memo.ip,config_memo.puerto);
  if(server_m < 0)
  {
    log_error(mlogger,"Error creando el servicio de memoria");
    return EXIT_FAILURE;
  }
  log_info(mlogger, "El servidor Memoria se inició correctamente");
  
}


// Lo del memoriaConfig:

config_de_memoria config_memo;

void obtener_valores_de_configuracion_memoria(t_config* memoriaConfig){
    config_memo.puerto = config_get_string_value(memoriaConfig, "PUERTO_ESCUCHA");
    config_memo.tam_memo = config_get_int_value(memoriaConfig, "TAM_MEMORIA");
    config_memo.tam_seg_0 = config_get_int_value(memoriaConfig,"TAM_SEGMENTO_0");
    config_memo.cant_seg = config_get_int_value(memoriaConfig,"CANT_SEGMENTOS");
    config_memo.retardo = config_get_int_value(memoriaConfig,"RETARDO_MEMORIA");
    config_memo.compactacion = config_get_int_value(memoriaConfig,"RETARDO_COMPACTACION");
    config_memo.algoritmo = config_get_string_value(memoriaConfig,"ALGORITMO_ASIGNACION");
    config_memo.cant_seg_max = config_memo.tam_memo / config_memo.tam_seg_0;
    config_memo.ip = string_duplicate("127.0.0.1");
    config_memo.bytes_libres=config_memo.tam_memo;
    algoritmos();
}

void mostrar_valores_de_configuracion_memoria (){
    printf("puerto = %s\n", config_memo.puerto);
    printf("tam_memo = %d\n" , config_memo.tam_memo);
    printf("tam_seg_0 = %d\n" , config_memo.tam_seg_0);
    printf("cant_seg = %d\n" , config_memo.cant_seg);
    printf("retardo = %d\n" , config_memo.retardo);
    printf("compactacion = %d\n", config_memo.compactacion);
    printf("algoritmo = %s, %d\n" , config_memo.algoritmo, config_memo.algoritmo_int);
    printf("Tamaño maximo = %d\n" , config_memo.cant_seg_max);

}


  
  
  void ejecutar_fs(){
    int conectar=config_memo.fs;
    log_info(flogger, "Por ejecutar las tareas del FileSystem");

    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_fs)
    {
      //printf("Por ejecutar las tareas del fileSystem\n");
      /*paquete_cpu=recibir_paquete(conectar);
      switch (paquete_cpu->codigo_operacion)
      {
          case :
            
            break;
          
          default:
            break;
      }
      eliminar_paquete(paquete_cpu);*/
      
    }
    running_fs=false;
    log_info(flogger,"Terminando de ejecutar las tareas del FileSystem");
    
  }

  void loggear(int code, int pid, void* algo, int id, int size, int base){
    
    switch (code)
    {
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
        log_info(loggerMemoria,"PID: %d - Segmento: %d - Base: %d - Tamaño %d",pid);
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
      
      default:
        log_error(loggerMemoria,"Error en la operación, código de error: %d",code);
        break;
    }
  }

void respuestas(int cliente, int code,void* algo){
  t_paquete* paquete=crear_paquete_operacion(code);
  agregar_a_paquete(paquete,&algo,sizeof(algo)+1);
  enviar_paquete(paquete,cliente);
  eliminar_paquete(paquete);
}

void actualizar_memoria(int size, int estado){
    
    if(estado == LIBRE){
        config_memo.bytes_libres=config_memo.bytes_libres+size;
    }else{
        config_memo.bytes_libres=config_memo.bytes_libres-size;
    }
}


void algoritmos(){
  if(strcmp(config_memo.algoritmo,"FF")==0){
    config_memo.algoritmo_int=FIRST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"BEST")==0){
    config_memo.algoritmo_int=BEST_FIT;
  }
  else if(strcmp(config_memo.algoritmo,"BF")==0){
    config_memo.algoritmo_int=BEST_FIT;
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
      //free(hueco);
      list_iterator_destroy(iterador);
      return index;
    }
    //free(hueco);
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

void imprimir_huecos(){
  int inicio;
  int tam;
  t_list_iterator* iterador = list_iterator_create(huecos_libres);
  t_hueco_libre* hueco = malloc(sizeof(t_hueco_libre));
  while (list_iterator_has_next(iterador)) {
    hueco = list_iterator_next(iterador);
    inicio = hueco->inicio; 
    tam = hueco->tamanio;
    int estado = hueco->estado;
    log_info(mlogger,"Hueco: %d - Base: %d  - Tamaño: %d - Estado: %d",list_iterator_index(iterador),inicio,tam,estado);
    //free(hueco);
  }
  list_iterator_destroy(iterador);
  
}

#include <memoriaUtils.h>
//#include "../include/memoriaUtils.h"

t_log* mlogger;
t_log* klogger;
t_log* clogger;
t_log* flogger;
t_log* loggerMemoria;
int conexion;
int running_cpu;
int running_k;
int running_fs;
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
	//metricas();
	//log_debug(mlogger,"metricas");
	terminar_programa(loggerMemoria, memoriaConfig);
	printf("-------------------FINAL POR CTRL+C-------------------");

	exit(-1);
}

void terminar_programa(t_log* milogger, t_config* memoria){
    /*LEAKS*/
	liberar_memoria();
	liberar_listas();
	liberar_conexion_memoria();
	
  log_destroy(milogger);
  liberar_t_config();
	config_destroy(memoria);
	printf("----------FIN------------\n");
};
void liberar_memoria(){
    log_destroy(mlogger);
    log_destroy(clogger);
    log_destroy(flogger);
    log_destroy(klogger);
};
void liberar_listas(){
    /* TO DO */
};
void liberar_conexion_memoria(){
    liberar_conexion(server_m);
};
void liberar_t_config(){
  free(config_memo.algoritmo);
  free(config_memo.ip);
  free(config_memo.puerto);
};
void inicializar_configuracion(){
    obtener_valores_de_configuracion_memoria(memoriaConfig);
    mostrar_valores_de_configuracion_memoria();
};

void inicializar_memoria(){

    inicializar_configuracion();
    inicializar_logs();
    inicializar_segmentos();
    conectar();
};
void inicializar_segmentos(){
    /* TO DO */
};	

void inicializar_logs(){
    loggerMemoria = log_create("logs/memoria.log","Memoria",true,LOG_LEVEL_TRACE);
    mlogger = log_create("logs/info.log","Info Memoria",true,LOG_LEVEL_TRACE);
    klogger = log_create("logs/kernel.log","Memoria -> Kernel",true,LOG_LEVEL_TRACE);
    clogger = log_create("logs/cpu.log","Memoria -> CPU",true,LOG_LEVEL_TRACE);
    flogger = log_create("logs/file_system.log","Memoria -> FileSystem",true,LOG_LEVEL_TRACE);
}
// Lo del memoria conexion: 
void conectar_cpu(){
    config_memo.cpu=esperar_cliente(server_m);
    t_paquete* paquete =malloc(sizeof(t_paquete));
    paquete = recibir_paquete(config_memo.cpu);
    if(paquete->codigo_operacion != CPU){
      log_error(clogger,"Vos no sos el CPU. Se cancela la conexión");
      pthread_detach(hilo_cpu);
			pthread_exit(&hilo_cpu);
    }
    log_info(clogger,"Se conectó el CPU: %d \n",config_memo.cpu);
		free(paquete);
    running_cpu=true;
    ejecutar_cpu();
}
void conectar_kernel(){
    config_memo.kernel=esperar_cliente(server_m);
    t_paquete* paquete =malloc(TAM_PAQ);
    paquete = recibir_paquete(config_memo.kernel);
    if(paquete->codigo_operacion != KERNEL){
      log_error(klogger,"Vos no sos el kernel. Se cancela la conexión %d",paquete->codigo_operacion);
      eliminar_paquete(paquete);
      pthread_detach(hilo_kernel);
			pthread_exit(&hilo_kernel);
    }
    log_info(klogger,"Se conectó el kernel: %d \n",config_memo.kernel);
		
    free(paquete);
    running_k=true;
    ejecutar_kernel();
}
void conectar_fs(){
  config_memo.fs=esperar_cliente(server_m);
    t_paquete* paquete =malloc(TAM_PAQ);
    paquete = recibir_paquete(config_memo.fs);
    if(paquete->codigo_operacion != FILE_SYSTEM){
      log_error(flogger,"Vos no sos el FS. Se cancela la conexión %d ",paquete->codigo_operacion);
      eliminar_paquete(paquete);
      pthread_detach(hilo_kernel);
			pthread_exit(&hilo_kernel);
    }
    log_info(flogger,"Se conectó el FileSystem: %d \n",config_memo.fs);
		
    free(paquete);
    running_k=true;
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
    config_memo.tam_maximo_seg = config_memo.tam_memo / config_memo.cant_seg;
    config_memo.ip = string_duplicate("127.0.0.1");
    config_memo.bytes_libres=config_memo.tam_memo;
}

void mostrar_valores_de_configuracion_memoria (){
    printf("puerto = %s\n", config_memo.puerto);
    printf("tam_memo = %d\n" , config_memo.tam_memo);
    printf("tam_seg_0 = %d\n" , config_memo.tam_seg_0);
    printf("cant_seg = %d\n" , config_memo.cant_seg);
    printf("retardo = %d\n" , config_memo.retardo);
    printf("compactacion = %d\n", config_memo.compactacion);
    printf("algoritmo = %s\n" , config_memo.algoritmo);
    printf("Tamaño maximo = %d\n" , config_memo.tam_maximo_seg);

}


  void ejecutar_kernel(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");

    t_paquete* paquete =malloc(TAM_PAQ);
    while (running_k)
    {
    
      paquete=recibir_paquete(conectar);
      switch (paquete->codigo_operacion)
      {
          case INICIO_PROCESO:
            crear_proceso(paquete);
            respuestas(config_memo.kernel,INICIO_PROCESO,paquete);
            break;
          case FIN_PROCESO:
            
            break;
          
          default:
            break;
      }
      eliminar_paquete(paquete);
      running_k=false;
      log_trace(klogger,"terminé de ejecutar kernel");
  }
  }
  void ejecutar_cpu(){
    int conectar=config_memo.cpu;
    log_trace(clogger, "Por ejecutar las tareas del CPU");

    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_cpu)
    {
      /*paquete_cpu=recibir_paquete(conectar);
      switch (paquete_cpu->codigo_operacion)
      {
          case :
            
            break;
          
          default:
            break;
      }
      eliminar_paquete(paquete_cpu);*/
      running_cpu=false;
    }
    log_info(clogger,"Terminando de ejecutar las tareas del CPU");
    
  }
  void ejecutar_fs(){
    int conectar=config_memo.fs;
    log_trace(flogger, "Por ejecutar las tareas del FileSystem");

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
      running_fs=false;
    }
    log_info(flogger,"Terminando de ejecutar las tareas del FileSystem");
    
  }

  void loggear(int code, int pid, void* algo, int id, int size, float base){
    
    switch (code)
    {
      case CREATE_SEGMENT:
        log_trace(loggerMemoria,"PID: %d - Crear Segmento: %d - Base: %f - TAMAÑO: %d",pid,id,base,size);
        break;
      case DELETE_SEGMENT:
        log_trace(loggerMemoria,"PID: %d - Eliminar Segmento: %d - Base: %f - TAMAÑO: %d",pid,id,base,size);
        break;
      case INICIO_COMPACTAR:
        log_trace(loggerMemoria,"Solicitud de Compactación");
        break;

      case FIN_COMPACTAR:
            /*Por cada segmento de cada proceso se deberá imprimir una línea con el siguiente formato:*/
        log_trace(loggerMemoria,"PID: %d - Segmento: %d - Base: %f - Tamaño %d",pid);
        break;

      case M_READ:
        log_trace(loggerMemoria,"PID: %d - Acción: <LEER> - Dirección física: %d - Tamaño: %d - Origen: %s",pid,id,size,algo);
        break;

      case M_WRITE:
        log_trace(loggerMemoria,"“PID: %d - Acción: <ESCRIBIR> - Dirección física: %d - Tamaño: %d - Origen: %s",pid,id,size,algo);
        break;

      case INICIO_PROCESO:
        log_trace(loggerMemoria,"Creación de Proceso PID: %d",pid);
        break;

      case FIN_PROCESO:
        log_trace(loggerMemoria,"Eliminación de Proceso PID: %d",pid);
        break;

      default:
        break;
    }
  }
void* crear_proceso(t_paquete* paquete){
  int pid=paquete->buffer->stream;
  
  return NULL;
}
void respuestas(int cliente, int code,t_paquete* paquete){
  
  paquete->codigo_operacion=code;
  enviar_paquete(paquete,cliente);
  eliminar_paquete(paquete);
}


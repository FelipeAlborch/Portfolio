#include <memoriaUtils.h>
//#include "../include/memoriaUtils.h"

t_log* mlogger;
t_log* loggerMemoria;
int conexion;
int running_cpu;
int running_k;
int running_fs;
int server_m;
int clientes[4];
t_config* memoriaConfig;
pthread_t hiloConexion;


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
	log_destroy(mlogger);
  log_destroy(milogger);
  liberar_t_config();
	config_destroy(memoria);
	printf("----------FIN------------\n");
};
void liberar_memoria(){
    /* TO DO */
};
void liberar_listas(){
    /* TO DO */
};
void liberar_conexion_memoria(){
    liberar_conexion(server_m);
};
void liberar_t_config(){
   // free(config_memo.algoritmo);
    //free(config_memo.ip);
  //  free(config_memo.puerto);
};
void inicializar_configuracion(){
    obtener_valores_de_configuracion_memoria(memoriaConfig);
    mostrar_valores_de_configuracion_memoria();
};

void inicializar_memoria(){
    loggerMemoria = iniciar_logger_modulo(MEMORIA_LOGGER);
    mlogger = log_create("logs/info.log","Memoria",true,LOG_LEVEL_TRACE);
    inicializar_configuracion();
    conectar();
};
void inicializar_segmentos(){
    /* TO DO */
};	

// Lo del memoria conexion: 

int SOCKET_KERNEL;
int SOCKET_CPU;
int SOCKET_FS;

int iniciar_servicio_memoria(config_de_memoria configuracion_memoria)
{
  t_log* mlogger = iniciar_logger_modulo(MEMORIA_LOGGER);
  log_info(mlogger,"iniciando servidor de memoria");

  int socket_servicio_memoria = iniciar_servidor_en("127.0.0.1",configuracion_memoria.puerto);
    
  if(socket_servicio_memoria < 0)
  {
    log_error(mlogger,"Error creando el servicio de memoria");
    return EXIT_FAILURE;
  }

  //log_info(mlogger, "Servicio de memoria abierto. Esperando conexiones");
	log_info(mlogger, "Servidor Memoria iniciado correctamente.");
  log_info(mlogger, "Esperando a los clientes CPU, Kernel y File System...");
  log_destroy(mlogger);

  return socket_servicio_memoria;
}

void manejar_paquetes_clientes(int socketCliente)
{
  Logger *mlogger = iniciar_logger_modulo(MEMORIA_LOGGER);


  switch (recibir_operacion(socketCliente))
  {
    case DESCONEXION:
      log_warning(mlogger, "Se desconecto un cliente.");
      return;

    case MENSAJE:
      
      switch(interpretar_origen_conexion(socketCliente))
      {
        case KERNEL:
          SOCKET_KERNEL = socketCliente;
          log_info(mlogger, "Se conecto Kernel.\n");
          //escuchar_modulo(socketCliente);
          //escuchar_kernel(socketCliente);
        break;
        case CPU:
          SOCKET_CPU = socketCliente;
          log_info(mlogger, "Se conecto CPU.");
          //escuchar_cpu(socketCliente);
        break;
        case FILE_SYSTEM:
          SOCKET_FS = socketCliente;
		  	  log_info(mlogger, "Se conecto FILE SYSTEM.");
		  	  //escuchar_file_system(socketCliente);
        break;
        default:
        log_info(mlogger, "Cliente desconocido.");
        break;
      }
  }

  log_destroy(mlogger);
}

modulo interpretar_origen_conexion(int socketAConexion)
{
  char* mensaje = obtener_mensaje_del_cliente(socketAConexion);
  if(!strcmp(mensaje,"Kernel")) return KERNEL;
  if(!strcmp(mensaje,"CPU")) return CPU;
  if(!strcmp(mensaje,"FileSystem")) return FILE_SYSTEM;
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
    config_memo.tam_maximo = config_memo.tam_memo / config_memo.cant_seg;
    config_memo.ip = string_duplicate("127.0.0.1");
}

void mostrar_valores_de_configuracion_memoria (){
    printf("puerto = %s\n", config_memo.puerto);
    printf("tam_memo = %d\n" , config_memo.tam_memo);
    printf("tam_seg_0 = %d\n" , config_memo.tam_seg_0);
    printf("cant_seg = %d\n" , config_memo.cant_seg);
    printf("retardo = %d\n" , config_memo.retardo);
    printf("compactacion = %d\n", config_memo.compactacion);
    printf("algoritmo = %s\n" , config_memo.algoritmo);
    printf("Tamaño maximo = %d\n" , config_memo.tam_maximo);

}
void conectar_cpu(){
    config_memo.cpu=esperar_cliente(server_m);
    t_paquete* paquete =malloc(sizeof(t_paquete));
    paquete = recibir_paquete(config_memo.cpu);
    if(paquete->codigo_operacion != CPU){
      log_error(mlogger,"Vos no sos el CPU. Se cancela la conexión");
      pthread_detach(hilo_cpu);
			pthread_exit(&hilo_cpu);
    }
    log_info(mlogger,"Se conectó el CPU: %d \n",config_memo.cpu);
		free(paquete);
    running_cpu=true;
    ejecutar_cpu();
}
void conectar_kernel(){
    config_memo.kernel=esperar_cliente(server_m);
    t_paquete* paquete =malloc(sizeof(t_paquete));
    paquete = recibir_paquete(config_memo.kernel);
    if(paquete->codigo_operacion != KERNEL){
      log_error(mlogger,"Vos no sos el kernel. Se cancela la conexión %d",paquete->codigo_operacion);
      eliminar_paquete(paquete);
      pthread_detach(hilo_kernel);
			pthread_exit(&hilo_kernel);
    }
    log_info(mlogger,"Se conectó el kernel: %d \n",config_memo.kernel);
		
    free(paquete);
    running_k=true;
    ejecutar_kernel();
}
void conectar_fs(){

}
void conectar(){
  server_m= iniciar_servidor_en(config_memo.ip,config_memo.puerto);
  log_info(mlogger, "iniciando servidor Memoria");
  
}
  void ejecutar_kernel(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");

    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_k)
    {
      printf("Por ejecutar las tareas del kernel\n");
      /*paquete_cpu=recibir_paquete(conectar);
      switch (paquete_cpu->codigo_operacion)
      {
          case :
            
            break;
          
          default:
            break;
      }
      eliminar_paquete(paquete_cpu);*/
      running_k=false;
      printf("termine de ejecutar kernel\n");
  }
  }
  void ejecutar_cpu(){
    int conectar=config_memo.cpu;
   // log_trace(mlogger, "Por ejecutar las tareas del kernel");

    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_cpu)
    {
      printf("Por ejecutar las tareas del cpu\n");
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
    
    
  }
  void ejecutarr_fs(){

  }
  void loggear(int tipo, int level, void* algo, ...)
  {
  }

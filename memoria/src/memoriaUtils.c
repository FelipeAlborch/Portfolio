#include <memoriaUtils.h>

t_log* loggerMemoria;
int conexion;
t_config* memoriaConfig;

void startSigHandlers(void) {
	signal(SIGINT, sigHandler_sigint);
}

void sigHandler_sigint(int signo) {
	log_warning(loggerMemoria, "Tiraste un CTRL+C, macho, abortaste el proceso.");
	//metricas();
	//log_debug(logger,"metricas");
	terminar_programa(conexion, loggerMemoria, memoriaConfig);
	printf("-------------------FINAL POR CTRL+C-------------------");

	exit(-1);
}

void terminar_programa(int conexion,t_log* logger, t_config* memoria){
    /*LEAKS*/
	liberar_memoria();
	liberar_listas();
	liberar_conexion_memoria(conexion);
	log_destroy(logger);
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
void liberar_conexion_memoria(int conexion){
    /* TO DO */
};
void liberar_t_config(){
    /* TO DO */
};
void inicializar_configuracion(){
    /* TO DO */
};

void inicializar_memoria(){
    /* TO DO */
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
  t_log* logger = iniciar_logger_modulo(MEMORIA_LOGGER);
  log_info(logger,"iniciando servidor de memoria");

  int socket_servicio_memoria = iniciar_servidor_en("127.0.0.1",configuracion_memoria.PUERTO_ESCUCHA);
    
  if(socket_servicio_memoria < 0)
  {
    log_error(logger,"Error creando el servicio de memoria");
    return EXIT_FAILURE;
  }

  //log_info(logger, "Servicio de memoria abierto. Esperando conexiones");
	log_info(logger, "Servidor Memoria iniciado correctamente.");
  log_info(logger, "Esperando a los clientes CPU, Kernel y File System...");
  log_destroy(logger);

  return socket_servicio_memoria;
}

void manejar_paquetes_clientes(int socketCliente)
{
  Logger *logger = iniciar_logger_modulo(MEMORIA_LOGGER);
  bool esKernel, esCPU;

  switch (recibir_operacion(socketCliente))
  {
    case DESCONEXION:
      log_warning(logger, "Se desconecto un cliente.");
      return;

    case MENSAJE:
      
      switch(interpretar_origen_conexion(socketCliente))
      {
        case KERNEL:
          SOCKET_KERNEL = socketCliente;
          log_info(logger, "Se conecto Kernel.\n");
          //escuchar_modulo(socketCliente);
          //escuchar_kernel(socketCliente);
        break;
        case CPU:
          SOCKET_CPU = socketCliente;
          log_info(logger, "Se conecto CPU.");
          //escuchar_cpu(socketCliente);
        break;
        case FILE_SYSTEM:
          SOCKET_FS = socketCliente;
		  	  log_info(logger, "Se conecto FILE SYSTEM.");
		  	  //escuchar_file_system(socketCliente);
        break;
        default:
        log_info(logger, "Cliente desconocido.");
        break;
      }
  }

  log_destroy(logger);
}

modulo interpretar_origen_conexion(int socketAConexion)
{
  char* mensaje = obtener_mensaje_del_cliente(socketAConexion);
  if(!strcmp(mensaje,"Kernel")) return KERNEL;
  if(!strcmp(mensaje,"CPU")) return CPU;
  if(!strcmp(mensaje,"FileSystem")) return FILE_SYSTEM;
}

/*void conexionesMemoria(){
	int mid;

	server_memo = iniciar_servidor(config_memoria->ip, config_memoria->puerto_memo);
	int k = 0;

	log_info(logger, "iniciando servidor Memoria");

	log_info(logger, "Esperando nueva conexion..");
	cliente_memoria[k] = esperar_cliente(server_memo);

	int running = 1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete = recibir_mensaje_carpincho(cliente_memoria[k]);
	while(running){

		int codigo = paquete->codigo_operacion;

		switch (codigo) {
		case KERNEL:
        log_warning(logger,
					"Operacion desconocida. No quieras meter la pata");
					pthread_detach(hiloConexion);
					pthread_exit(&hiloConexion);
					liberar_conexion(cliente_memo);
			break;

			}
	
		k++;
		eliminar_paquete(paquete);
		cliente_memoria[k] = esperar_cliente(server_memo);
		paquete = recibir_mensaje(cliente_memoria[k]);
		}
        */

// Lo del memoriaConfig:

config_de_memoria config_memo;

void obtener_valores_de_configuracion_memoria(t_config* memoriaConfig){
    config_memo.PUERTO_ESCUCHA = config_get_string_value(memoriaConfig, "PUERTO_ESCUCHA");
    config_memo.TAM_MEMORIA = config_get_int_value(memoriaConfig, "TAM_MEMORIA");
    config_memo.TAM_SEGMENTO_0 = config_get_int_value(memoriaConfig,"TAM_SEGMENTO_0");
    config_memo.CANT_SEGMENTOS = config_get_int_value(memoriaConfig,"CANT_SEGMENTOS");
    config_memo.RETARDO_MEMORIA = config_get_int_value(memoriaConfig,"RETARDO_MEMORIA");
    config_memo.RETARDO_COMPACTACION = config_get_int_value(memoriaConfig,"RETARDO_COMPACTACION");
    config_memo.ALGORITMO_ASIGNACION = config_get_string_value(memoriaConfig,"ALGORITMO_ASIGNACION");
    config_memo.tam_maximo = config_memo.TAM_MEMORIA / config_memo.CANT_SEGMENTOS;
    
}

void mostrar_valores_de_configuracion_memoria (){
    printf("PUERTO_ESCUCHA = %s\n", config_memo.PUERTO_ESCUCHA);
    printf("TAM_MEMORIA = %d\n" , config_memo.TAM_MEMORIA);
    printf("TAM_SEGMENTO_0 = %d\n" , config_memo.TAM_SEGMENTO_0);
    printf("CANT_SEGMENTOS = %d\n" , config_memo.CANT_SEGMENTOS);
    printf("RETARDO_MEMORIA = %d\n" , config_memo.RETARDO_MEMORIA);
    printf("RETARDO_COMPACTACION = %d\n", config_memo.RETARDO_COMPACTACION);
    printf("ALGORITMO_ASIGNACION = %s\n" , config_memo.ALGORITMO_ASIGNACION);
    printf("Tamaño maximo = %d\n" , config_memo.tam_maximo);
}
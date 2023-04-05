#include <memoriaConexion.h>

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
        esKernel = es_kernel(socketCliente);
		esCPU = es_cpu(socketCliente);

        if (esKernel)
        {
          SOCKET_KERNEL = socketCliente;
          log_info(logger, "Se conecto Kernel.\n");
          //escuchar_kernel(socketCliente);
        }
        else if(es_cpu)
        {
          SOCKET_CPU = socketCliente;
          log_info(logger, "Se conecto CPU.");
          //escuchar_cpu(socketCliente);
        }
		else
		{
			SOCKET_FS = socketCliente;
			log_info(logger, "Se conecto FILE SYSTEM.");
			//escuchar_file_system(socketCliente);
		}
        break;

    default:
        log_info(logger, "Cliente desconocido.");
        break;
  }

  log_destroy(logger);
}

bool es_kernel(int socketCliente)
{
  char *mensaje = obtener_mensaje_del_cliente(socketCliente);
  bool esKernel = strcmp(mensaje, "Kernel") == 0;
  free(mensaje);

  return esKernel;
}

bool es_cpu(int socketCliente)
{
  char *mensaje = obtener_mensaje_del_cliente(socketCliente);
  bool esCPU = strcmp(mensaje, "CPU") == 0;
  free(mensaje);

  return esCPU;
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
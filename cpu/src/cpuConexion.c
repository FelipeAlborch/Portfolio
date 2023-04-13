#include <cpuConexion.h>

int socketMemoriaUtil;

int conectar_con_memoria(config_de_cpu configuracionCPU){  

	Logger* logger = iniciar_logger_modulo(CPU_LOGGER);
	log_info(logger, "Conectando con memoria");

	int socketMemoria = crear_conexion(configuracionCPU.IP_MEMORIA, configuracionCPU.PUERTO_MEMORIA);

	if(socketMemoria < 0)
    {
		log_error(logger, "Error al conectarse con memoria");
		return EXIT_FAILURE;
	}

	log_info(logger, "Conexion con Memoria realizada");
  log_info(logger, "Enviando mensaje al servidor Memoria...");


  t_paquete* conectar= crear_paquete();
	conectar->codigo_operacion= CPU;
	enviar_paquete(conectar, socketMemoria);
	log_info(logger, "Mensaje enviado correctametne!");

	log_destroy(logger);
	return socketMemoria;
}

void esperar_kernel_dispatch(int socketCpu)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);
  log_info(logger, "Entre a la funcion para esperar Kernel");

  bool desconecto = false;

  while (true)
  {
    log_info(logger, "Esperando a Kernel que se conecte al puerto Dispatch...");
    int socketKernel = esperar_cliente(socketCpu);

    if (socketKernel < 0)
    {
      log_warning(logger, "Kernel desconectado.");
      log_destroy(logger);
      return;
    }

    log_info(logger, "Conexión con Kernel en puerto Dispatch establecida.");

    desconecto = manejar_paquete_kernel_dispatch(socketKernel);

    if (desconecto)
      log_destroy(logger);
    return;
  }
}

bool manejar_paquete_kernel_dispatch(int socketKernel)
{ 
  while (true)
  {
    Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

    switch (recibir_operacion(socketKernel))
    {
        case DESCONEXION:
            log_warning(logger, "Conexión con Kernel en puerto Dispatch terminada.");
            close(socketKernel);
            log_destroy(logger);
        return true;
        
        case MENSAJE:
            log_info(logger, "Mensaje recibido de Kernel.");
            char *mensaje = obtener_mensaje_del_cliente(socketKernel);
            log_info(logger, "Mensaje: %s", mensaje);
            free(mensaje);
        return;
			//break;

        case PCB:
            log_info(logger, "PCB recibido de Kernel.");
            t_list* lista_del_pcb = _recibir_paquete(socketKernel);
            pcb* pcb = recibir_pcb(lista_del_pcb);
            loguear_pcb(pcb, logger);
            
            //ejecutar_lista_instrucciones_del_pcb(pcb, socketKernel, socketMemoriaUtil);
            //liberar_pcb(pcb);
        break;
		
        default:
            break;
    } 
    log_destroy(logger);
  }
}

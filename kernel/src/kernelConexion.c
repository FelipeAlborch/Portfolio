#include "kernelConexion.h"

int iniciar_servidor_para_consolas(config_de_kernel configuracionKernel)
{
    Logger* logger = iniciar_logger_modulo(KERNEL_LOGGER);
    int socketServidorConsolas = iniciar_servidor_en("127.0.0.1",configuracionKernel.PUERTO_ESCUCHA);

    if(socketServidorConsolas < 0)
    {
        log_error(logger, "Error al iniciar el servicio de CONSOLAS");
        return EXIT_FAILURE;
    }

    log_info(logger, "Se inicio el servicio de CONSOLAS. Ya pueden ser recibidas");
    log_destroy(logger);

    return socketServidorConsolas;
}

int conectar_con_cpu(config_de_kernel configuracionKernel){  

	Logger* logger = iniciar_logger_modulo(KERNEL_LOGGER);
	log_info(logger, "Conectando con cpu");

	int socketCPU = crear_conexion(configuracionKernel.IP_CPU, configuracionKernel.PUERTO_CPU);
	
	if(socketCPU < 0)
    {
		log_error(logger, "Error al conectarse con cpu");
		return EXIT_FAILURE;
	}
	
	log_info(logger, "Conexion con la cpu realizada con exito!");
    
	log_destroy(logger);

	return socketCPU;
}

int conectar_con_memoria(config_de_kernel configuracion_kernel){  

	Logger* logger = iniciar_logger_modulo(KERNEL_LOGGER);
	log_info(logger, "Conectando con memoria");

	int socketMemoria = crear_conexion(configuracion_kernel.IP_MEMORIA, configuracion_kernel.PUERTO_MEMORIA);

	if(socketMemoria < 0)
    {
		log_error(logger, "Error al conectarse con memoria");
		return EXIT_FAILURE;
	}
	
	log_info(logger, "Conexion con la Memoria realizada con exito!");

	log_info(logger, "Enviando un mensaje a la Memoria...");
    enviar_mensaje("Kernel", socketMemoria);
    log_info(logger, "Mensaje enviado!");
    
	log_destroy(logger);

	return socketMemoria;
}


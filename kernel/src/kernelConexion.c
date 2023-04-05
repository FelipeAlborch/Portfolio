#include "kernelConexion.h"


int iniciar_servicio_memoria(config_de_kernel configuracion_kernel)
{
    t_log* logger = iniciar_logger_modulo(KERNEL_LOGGER);
    log_info(logger,"iniciando servidor de memoria");

    int socket_memoria = iniciar_servidor_en("127.0.0.1",configuracion_kernel.PUERTO_MEMORIA);
    
    if(socket_memoria < 0)
    {
        log_error(logger,"Error creando el servicio de memoria");
        return EXIT_FAILURE;
    }

    log_info(logger, "Conectado con memoria");

    return socket_memoria;
}

int conectar_con_memoria(config_de_kernel configuracion_kernel){  			// Se conecta a memoria

	t_log* logger = iniciar_logger_modulo(KERNEL_LOGGER);
	log_info(logger, "Conectando con memoria");

	int socket_memoria = crear_conexion(configuracion_kernel.IP_MEMORIA, configuracion_kernel.PUERTO_MEMORIA);
	if(socket_memoria < 0)
    {
		log_error(logger, "Error al conectarse con memoria");
		return EXIT_FAILURE;
	}
	log_info(logger, "Conexion con memoria realizada");
	log_destroy(logger);

	return socket_memoria;
}


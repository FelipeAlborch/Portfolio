#include <consolaConexion.h>

int conectar_con_kernel(config_de_consola configuracionConsola)
{
    Logger* logger = iniciar_logger_modulo(CONSOLA_LOGGER);
	log_info(logger, "Conectando con kernel");

	int socketKernel = crear_conexion(configuracionConsola.IP_KERNEL, configuracionConsola.PUERTO_KERNEL);
	
	if(socketKernel < 0)
    {
		log_error(logger, "Error al conectarse con kernel");
		return;
	}
	
	log_info(logger, "Conexion con el kernel realizada con exito!");
    
	log_destroy(logger);

	return socketKernel;
}
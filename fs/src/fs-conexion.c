#include <filesystemConexion.h>

int conectar_con_memoria(configDeFileSystem configuracionFileSystem){  

	Logger* logger = iniciar_logger_modulo(FILESYSTEM_LOGGER);
	log_info(logger, "Conectando con memoria");

	int socketMemoria = crear_conexion(configuracionFileSystem.IP_MEMORIA, configuracionFileSystem.PUERTO_MEMORIA);

	if(socketMemoria < 0)
    {
		log_error(logger, "Error al conectarse con Memoria");
		return EXIT_FAILURE;
	}
	
	log_info(logger, "Conexion con la Memoria realizada con exito!");

	log_info(logger, "Enviando un mensaje a la Memoria...");
    enviar_mensaje("FS", socketMemoria);
    log_info(logger, "Mensaje enviado!");
    
	log_destroy(logger);

	return socketMemoria;
}
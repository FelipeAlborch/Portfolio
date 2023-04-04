#include <memoriaConexion.h>

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

    log_info(logger, "Servicio de memoria abierto. Esperando conexiones");
    log_destroy(logger);

    return socket_servicio_memoria;
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
#include "kernelConexion.h"

int iniciar_servidor_para_consolas(config_de_kernel configuracionKernel)
{
    Logger* logger = iniciar_logger_modulo(KERNEL_LOGGER);
    int socketServidorConsolas = iniciar_servidor_en("127.0.0.1", configuracionKernel.PUERTO_ESCUCHA);

    if(socketServidorConsolas < 0)
    {
        log_error(logger, "Error al iniciar el servicio de CONSOLAS");
        return EXIT_FAILURE;
    }

    log_info(logger, "Se inicio el servicio de CONSOLAS. Ya pueden ser recibidas");
    log_destroy(logger);

    return socketServidorConsolas;
}

void* esperar_consolas(int socketServidorConsolas)
{

	Logger* logger = iniciar_logger_modulo(KERNEL_LOGGER);
	log_info(logger, "Esperando consolas");

	while(true)
	{
		int socketConsola = esperar_cliente(socketServidorConsolas);

		if(socketConsola < 0)
		{
			log_error(logger, "Error en la conexion de la consola");
			return;
		}
		log_info(logger, "Consola conectada: %d", socketConsola);

		Hilo hiloConsola; 
		pthread_create(&hiloConsola, NULL, (void*)escuchar_consola, (void*) socketConsola);
		//pthread_join(hiloConsola, NULL);
		pthread_detach(hiloConsola);
	}

	log_destroy(logger);
}

void escuchar_consola(int socketConsola)
{
	t_log* logger;
	t_list* lista_de_consola;
    while(true)
    {
        logger = iniciar_logger_modulo(KERNEL_LOGGER);

        switch(recibir_operacion(socketConsola))
        {
            case MENSAJE:
                recibir_mensaje(socketConsola, logger);
				
            break;
			
			
            case LISTA_INSTRUCCIONES:
            log_info(logger, "Me llego una lista de instrucciones");
			
			t_list* lista_instrucciones = list_create();
			lista_de_consola = _recibir_paquete(socketConsola);
			deserializar_lista_de_consola(lista_instrucciones, lista_de_consola, 0, 0);
			
			pid_global++;
			pcb* un_pcb = crear_pcb(lista_instrucciones, pid_global, configuracionKernel.ESTIMACION_INICIAL/1000);
			loguear_pcb(un_pcb,logger);
			// TO-DO:
			// t_list* tabla_segmentos = solicitar_tabla_de_segmentos(socketMemoria);
			//pcb->tabla_de_segmentos = list_duplicate(tabla_segmentos);
            break;

            case DESCONEXION:
			break;
            

		   
            default:
                log_warning(logger, "Operacion Desconocida");
                return;
            break;
        }
        log_destroy(logger);

    }
}

void deserializar_lista_de_consola(t_list* lista_de_instrucciones, t_list* lista_de_contenido_recibido, int indice_tamaño, int indice_lista)
{
    int cantidad_de_instrucciones = *(int *)list_get(lista_de_contenido_recibido, indice_tamaño);
    int i;
    int base;
    for(i = 0; i < cantidad_de_instrucciones; i++)
    {
        LineaInstruccion* una_instruccion = malloc(sizeof(LineaInstruccion));
        base = 4 * i;

        una_instruccion->identificador = string_duplicate(list_get(lista_de_contenido_recibido, base + indice_lista + 1));
        una_instruccion->parametros[0] = string_duplicate(list_get(lista_de_contenido_recibido, base + indice_lista + 2));
        una_instruccion->parametros[1]= string_duplicate(list_get(lista_de_contenido_recibido, base + indice_lista + 3));
		una_instruccion->parametros[2] = string_duplicate(list_get(lista_de_contenido_recibido, base + indice_lista + 4));

        list_add(lista_de_instrucciones, una_instruccion);
    }
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
    
	t_paquete* conectar= crear_paquete();
	conectar->codigo_operacion= KERNEL;
	enviar_paquete(conectar, socketMemoria);
    log_info(logger, "Mensaje enviado!");
    
	log_destroy(logger);
	
	return socketMemoria;
}

int conectar_con_filesystem(config_de_kernel configuracionKernel)
{
	Logger* logger = iniciar_logger_modulo(KERNEL_LOGGER);
	log_info(logger, "Conectando con memoria");

	int socketFS = crear_conexion(configuracionKernel.IP_FILESYSTEM, configuracionKernel.PUERTO_FILESYSTEM);

	if(socketFS < 0)
    {
		log_error(logger, "Error al conectarse con FileSystem");
		return EXIT_FAILURE;
	}
	
	log_info(logger, "Conexion con el FileSystem realizada con exito!");
    
	log_destroy(logger);

	return socketFS;
}
#include <utils/conexion.h>

t_log* logger;

int iniciar_servidor_en(char* ip, char* puerto)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);
	
	bool conecto = false;

	// Creamos el socket
	for(struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			continue; // Basicamente vamos al siguiente servinfo;
		}
    	//Asociamos el socket
    	if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) 
		{
    	    close(socket_servidor);
    	    continue;
    	}

		conecto = true;
		break;
	} 

	if(!conecto)
	{
		free(servinfo);
		return 0;
	}

    if(listen(socket_servidor, SOMAXCONN) == -1){					

		perror("Error al escuchar conexiones\n");
		//close(socket_servidor);
		//exit(EXIT_FAILURE);
	}	

	freeaddrinfo(servinfo);

	return socket_servidor;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);;

	// Ahora que tenemos el socket, vamos a conectarlo

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		printf("error conectandose al socket. Reintentando.\n");
		socket_cliente = -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

int esperar_cliente(int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);

	if(socket_cliente < 0){
		close(socket_servidor);
	    perror("Error al aceptar Cliente\n");
        exit(EXIT_FAILURE);
	}

	//struct sockaddr_in dir_cliente;
	//unsigned int tam_direccion = sizeof(struct sockaddr_in);
	//int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);


	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return DESCONEXION;
	}
}

void recibir_mensaje(int socket_cliente, t_log* logger)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

char* obtener_mensaje_del_cliente(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);

	return buffer;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
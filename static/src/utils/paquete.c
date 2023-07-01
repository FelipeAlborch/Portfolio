#include <utils/paquete.h>

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	char* stream =string_duplicate(" ");
	paquete->buffer = crear_buffer(stream);
	free(stream);
	
	return paquete;
}

t_paquete* crear_paquete_operacion(int codigo_operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	_crear_buffer(paquete);
	//paquete->buffer =malloc(sizeof(t_buffer));
	//paquete->buffer->stream = string_duplicate(" ");
	//paquete->buffer->size =sizeof(paquete->buffer->stream);
	return paquete;
}


void agregar_entero_a_paquete(t_paquete* paquete, void* valor)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, valor, sizeof(int));

	paquete->buffer->size += sizeof(int);
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	// FIXME: probablemente haya memleaks por comentar esta linea. 
	// la comenté para evitar un double-free al correr cpu.
	free(a_enviar);
}

t_list* _recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
		//free(valor);	//	Revisar el memcheck.log con y sin este free
	}

	
	free(buffer);
	return valores;
}

t_list* deserializar_mread (t_paquete* paquete) {

	int desplazamiento = 0;
	t_list* valores = list_create();

	while(desplazamiento < paquete->buffer->size)
	{
		int valor;
		memcpy(&valor, paquete->buffer->stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		list_add(valores, valor);
	}
	
	return valores;
}

// t_list* deserializar_mwrite (t_paquete* paquete) {

// 	int desplazamiento = 0;
// 	t_list* valores = list_create();

// 	int dir;
// 	int tamanio;
// 	char* dato;
// 	int offset;

// 	memcpy(&dir, paquete->buffer->stream + desplazamiento, sizeof(int));
// 	desplazamiento += sizeof(int);

// 	memcpy(&tamanio, paquete->buffer->stream + desplazamiento, sizeof(int));
// 	desplazamiento += sizeof(int);

// 	dato = malloc(tamanio);
// 	memcpy(dato, paquete->buffer->stream + desplazamiento, tamanio);
// 	desplazamiento += tamanio;

// 	memcpy(&offset, paquete->buffer->stream + desplazamiento, sizeof(int));
	
// 	list_add(valores, &dir);
// 	list_add(valores, &tamanio);
// 	list_add(valores, dato);
// 	list_add(valores, &offset);

// 	return valores;
// }

t_list* deserializar_mwrite(t_paquete* paquete) {
    int desplazamiento = 0;
    t_list* valores = list_create();

    int dir;
    int tamanio;
    char* dato;
    int offset;

    memcpy(&dir, paquete->buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&tamanio, paquete->buffer->stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    dato = malloc(tamanio);
    memcpy(dato, paquete->buffer->stream + desplazamiento, tamanio);
    desplazamiento += tamanio;

    memcpy(&offset, paquete->buffer->stream + desplazamiento, sizeof(int));

    list_add(valores, &dir);
    list_add(valores, dato);
    list_add(valores, &tamanio);
    list_add(valores, &offset);

    return valores;
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
t_paquete* recibir_paquete (int socket) {
	int codigo_operacion;

	t_log* logger = log_create("logs.log","",true,LOG_LEVEL_ERROR);

	if (recv(socket, &codigo_operacion, sizeof(int), MSG_WAITALL) <= 0) {
		log_error(logger, "Ocurrio un error al recibir op_code");
		codigo_operacion = ERROR;
	}

	int size;
	if (recv(socket, &size, sizeof(int), MSG_WAITALL) <= 0) {
		log_error(logger, "Ocurrio un error al recibir size");
		codigo_operacion = ERROR;
	}
	printf("el valor del size %d",size);
	void* stream = malloc(size);
	if (recv(socket, stream, size, MSG_WAITALL) <= 0) {
		log_error(logger, "Ocurrio un error al recibir stream");
		codigo_operacion = ERROR;
	}


	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer->stream = stream;
	paquete->buffer->size = size;

	//free(stream);
	log_destroy(logger);
	return paquete;
}

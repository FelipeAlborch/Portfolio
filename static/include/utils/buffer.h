#ifndef BUFFER_H_
#define BUFFER_H_

#include <utils/generales.h>

typedef enum
{
	DESCONEXION = -1,
	MENSAJE,
	PAQUETE,
	CONTENIDO_CONSOLA,
	INTERRUPCION,
	PCB,
	PCB_BLOCKED,
	PANTALLA,
	TECLADO
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

/**
* @NAME: recibir_buffer
* @DESC: Crea un buffer del tamaño de lo que se recibe por el socket de cliente.
* @PARAMS:
*        int* size - tamaño de lo que se recibe.
*		 int socket_cliente - Socket de algun cliente
* @RETURN:
*        retorna el buffer.
*/
void* recibir_buffer(int*, int);

void _crear_buffer(t_paquete*);
t_buffer* crear_buffer(void* stream);
void eliminar_buffer(t_buffer*);
#endif
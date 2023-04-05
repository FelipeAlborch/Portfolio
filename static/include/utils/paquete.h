#ifndef PAQUETE_H_
#define PAQUETE_H_

#include <utils/buffer.h>

/**
* @NAME: crear_paquete
* @DESC: Crea un t_paquete*.
* @RETURN:
*        retorna el paquete creado.
*/
t_paquete* crear_paquete(void);

/**
* @NAME: crear_paquete_operacion
* @DESC: Crea un t_paquete* con un codigo especifico.
* @PARAMS: int codigo_operacion - Algun valor del enum op_code que se encuentra en el buffer.h
* @RETURN:
*        retorna el paquete creado.
*/
t_paquete* crear_paquete_operacion(int);

/**
* @NAME: agregar_a_paquete
* @DESC: Agrega un elemento al paquete.
* @PARAMS:
*        t_paquete* paquete - El paquete al que queres agregar el elemento.
*		 void* valor - El elemento que queres agregar.      (Aclaracion: void* significa que puede ser de cualquier tipo)
*        int tamanio - el tamanio del elemento.
*/
void agregar_a_paquete(t_paquete*, void*, int);

/**
* @NAME: serializar_paquete
* @DESC: Serializa el paquete.
* @PARAMS:
*        t_paquete* paquete - El paquete al que queres serializar.
*        int bytes - Cantidad de bytes que se ira desplazando el puntero en el memcpy.
*/
void* serializar_paquete(t_paquete*, int);

/**
* @NAME: enviar_paquete
* @DESC: Envia un paquete a un cliente.
* @PARAMS:
*        t_paquete* paquete - El paquete que queres enviar.
*        int socket_cliente - El cliente al que le queres enviar el paquete
*/
void enviar_paquete(t_paquete*, int);

/**
* @NAME: recibir_paquete
* @DESC: Carga en una lista los valores recibidos del paquete.
* @PARAMS:
*        int socket_cliente - El cliente del que recibis el paquete.
* @RETURN:
*        retorna la lista con los valores cargados.
*/
t_list* recibir_paquete(int);

/**
* @NAME: eliminar_paquete
* @DESC: Se limpia la memoria que utilizaba el paquete.
* @PARAMS:
*        t_paquete* paquete - El paquete que queres eliminar.
*/
void eliminar_paquete(t_paquete*);

#endif
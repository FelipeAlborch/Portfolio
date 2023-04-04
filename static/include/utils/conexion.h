#ifndef CONEXION_H_
#define CONEXION_H_

#include <utils/PCB.h>



/**
* @NAME: iniciar_servidor_en
* @DESC: Crea un socket de servicio y lo deja bindeado y en listen.
* @PARAMS:
*        char* ip - la ip del servidor que queres abrir.
*		 char* puerto - El puerto al que queres que lleguen las conexiones
* @RETURN:
*        retorna el socket_servidor.
*/
int iniciar_servidor_en(char*, char*);

/**
* @NAME: crear_conexion
* @DESC: Crea un socket de cliente, conectandose a un servidor.
* @PARAMS:
*        char* ip - la ip del servidor que te queres conectar.
*		 char* puerto - El puerto al que te queres conectar
* @RETURN:
*        retorna el socket_cliente.
*/
int crear_conexion(char*, char*);

/**
* @NAME: crear_conexion
* @DESC: Espera a que un cliente se conecte a traves de la funcion accept.
* @PARAMS:
*        int socket_servidor - El socket del servidor que esta esperando.
* @RETURN:
*        retorna el socket_cliente.
*/
int esperar_cliente(int);

/**
* @NAME: recibir_operacion
* @DESC: Recibe una operacion por parte del cliente y analiza cual es.
* @PARAMS:
*        int socket_cliente - El socket del cliente que envia la operacion.
* @RETURN:
*        retorna la operacion que es o -1 en caso de error.
*/
int recibir_operacion(int);

/**
* @NAME: recibir_mensaje
* @DESC: Recibe un mensaje por parte de un cliente y lo loguea.
* @PARAMS:
*        int socket_cliente - El socket del cliente que envia el mensjae.
*        t_log* logger - El logger donde se loguea el mensaje.
*/
void recibir_mensaje(int, t_log*);

char* obtener_mensaje_del_cliente(int);

/**
* @NAME: enviar_mensaje
* @DESC: Envia un mensaje a otra conexion.
* @PARAMS:
*        char* mensaje - El mensaje a enviar
*        int socket_cliente - El socket del cliente al que le envias el mensaje.
* @RETURN:
*        retorna la operacion que es o -1 en caso de error.
*/
void enviar_mensaje(char*, int);

/**
* @NAME: liberar_conexion
* @DESC: Cierra el socket de una conexion.
* @PARAMS:
*        int socket_cliente - El socket que queres cerrar.
*/
void liberar_conexion(int);


#endif
#ifndef CONSOLA_UTILS_H
#define CONSOLA_UTILS_H

#define SUCCESS 0
#define FAILURE -1

#include <consolaConexion.h>
#include <commons/txt.h>

extern int socket_kernel;
/*
int conectar_con_kernel();
void agregar_linea_de_instruccion_a_lista(LineaInstruccion*, Lista*);
void asignar_tokens_a_linea_instruccion(LineaInstruccion*, char**);
void rellenar_parametros_de_instruccion(char**, int);
int cantidad_de_tokens(char*);
char **obtener_tokens(char*);
void eliminar_salto_linea(char*);
char *leer_linea(FILE*);
LineaInstruccion *leer_linea_de_instruccion(FILE*);
void rellenar_lista_de_instrucciones(Lista*, FILE*);
void enviar_instrucciones_a_kernel(FILE*);
int veces(char*, char);
void* liberar_tokens(char**);
*/
void empaquetar_instrucciones(t_paquete*, t_list*);



/**
* @NAME: leer_linea
* @DESC: Devuelve una linea de texto de un archivo, reemplazando el \n del final, por un \0.
* @PARAMS:
*        FILE * archivo - El archvio del cual leer la linea.
* @RETURN:
*        retorna la linea leida.
*/
char* leer_linea(FILE*);

/**
* @NAME: cantidad_de_valores
* @DESC: Calcula cuantos valores tiene la linea. Ej: SET AX 1 tiene 3 valores.
* @PARAMS:
*        char* linea - La linea de la cual queres saber cuantos valores tiene.
* @RETURN:
*        retorna la cantidad de valores.
*/
int cantidad_de_valores(char*);

/**
* @NAME: inicializar_parametros
* @DESC: Funcion especifica para la instruccion EXIT. En caso de ser EXIT, esta no tiene parametros, entonces se les guarda -1 y -1 a los parametros.
* @PARAMS:
*        char** parametros - La lista con cada parametro de la linea.
*        int cantidad_de_valores - La cantidad de valores de la linea.
*/
void inicializar_parametros(char**, int);

/**
* @NAME: identificar_parametros
* @DESC: Identifica cuales son los parametros de la linea.
* @PARAMS:
*        char* linea - La linea de la cual queres saber sus parametros.
* @RETURN:
*        retorna los parametros de la linea.
*/
char** identificar_parametros(char*);

/**
* @NAME: leer_instruccion
* @DESC: Lee una instruccion del archivo y crea una instruccion con la misma.
* @PARAMS:
*        FILE* archivo - El archivo del cual queres leer la instruccion.
* @RETURN:
*        retorna la instruccion.
*/
LineaInstruccion* leer_instruccion(FILE*);

/**
* @NAME: crear_instruccion
* @DESC: Dada una instruccion vacia, y la lista de parametros de una linea, rellena la instruccion con los valores correspondientes.
* @PARAMS:
*        instruccion* instruccion_leida - la instruccion vacia.
*        char** parametros - los parametros de la lina leida previamente que llenaran la instruccion.
*/
void crear_instruccion(LineaInstruccion*, char**);

/**
* @NAME: leer_archivo
* @DESC: Funcion encargada de leer todo el contenido de un archivo y logearlo. Solo nos sirve como un chequeo de que se esta leyendo todo correctamente.
* @PARAMS:
*        FILE* archivo - El archivo que se quiere leer.
*        t_log* logger - El logger sobre el cual se loggeara las instrucciones leidas.
*/
void leer_archivo(FILE*, t_log*);

/**
* @NAME: enlistar_instrucciones
* @DESC: Crea una lista con las listas de un archivo.
* @PARAMS:
*        FILE* archivo - El archivo del cual se quieren leer las instrucciones.
* @RETURN:
*        retorna una lista de instrucciones.
*/
t_list* enlistar_instrucciones(FILE*);

/**
* @NAME: leer_lista_de_instrucciones
* @DESC: Funcion encargada de leer todo el contenido de una lista de instrucciones. Solo nos sirve como un chequeo de que se esta leyendo todo correctamente.
* @PARAMS:
*        t_list* lista_instrucciones - La lista de instrucciones a leer.
*/
void leer_lista_de_instrucciones(t_list*);

/**
* @NAME: imprimir_instruccion
* @DESC: Funcion encargada de imprimir una instruccion utilizada en la funcion leer_lista_de_instrucciones.
* @PARAMS:
*        instruccion* una_instruccion - La instruccion a imprimir.
*        int nro_instruccion - El nro de instruccion en la lista.
*/
void imprimir_instruccion(LineaInstruccion*, int);
#endif
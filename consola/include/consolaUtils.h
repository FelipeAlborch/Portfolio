#ifndef CONSOLA_UTILS_H
#define CONSOLA_UTILS_H

#define SUCCESS 0
#define FAILURE -1

#include <consolaConexion.h>
#include <commons/txt.h>

extern int socket_kernel;

int conectar_con_kernel();
void dump(Lista*);
void agregar_linea_de_instruccion_a_lista(LineaInstruccion*, Lista*);
void asignar_tokens_a_linea_instruccion(LineaInstruccion*, char**);
void rellenar_parametros_de_instruccion(char**, int);
int cantidad_de_tokens(char*);
char **obtener_tokens(char*);
void eliminar_salto_linea(char*);
char *leer_linea(FILE*);
LineaInstruccion *leer_linea_de_instruccion(FILE*);
void rellenar_lista_de_instrucciones(Lista*, FILE*);
void enviar_instrucciones_a_kernel(char*);
int veces(char*, char);

#endif
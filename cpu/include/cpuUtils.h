#ifndef CPUUTILS_H_
#define CPUUTILS_H_

#include <cpuConexion.h>

void ejecutar_lista_instrucciones_del_pcb(pcb*, int, int);
Instruccion obtener_tipo_instruccion(char*);

#endif
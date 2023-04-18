#ifndef CPUUTILS_H_
#define CPUUTILS_H_

#include <cpuConexion.h>

void ejecutar_lista_instrucciones_del_pcb(pcb*, int, int);
Instruccion obtener_tipo_instruccion(char*);
void ejecutar_set(pcb*, LineaInstruccion*);
void estado_de_los_registros(pcb*);
void ejecutar_yield(pcb*, int);
void logear_instruccion(int, LineaInstruccion*);
void ejecutar_exit(pcb*, int);

#endif
#ifndef CPUUTILS_H_
#define CPUUTILS_H_

#include <cpuConexion.h>

void ejecutar_lista_instrucciones_del_pcb(pcb*, int, int);
Instruccion obtener_tipo_instruccion(char*);

// Funciones que ejecutan instrucciones

void ejecutar_set(pcb*, LineaInstruccion*);
void ejecutar_yield(pcb*, int);
void ejecutar_exit(pcb*, int);
void ejecutar_io(pcb*, LineaInstruccion*, int);

//

// Funciones para logear valores varios

void estado_de_los_registros(pcb*);
void logear_instruccion(int, LineaInstruccion*);

//

#endif
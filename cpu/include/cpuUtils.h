#ifndef CPUUTILS_H_
#define CPUUTILS_H_

#include <cpuConexion.h>

void ejecutar_lista_instrucciones_del_pcb(pcb*, int, int);
Instruccion obtener_tipo_instruccion(char*);
bool es_instruccion_de_corte(Instruccion);

// Funciones que ejecutan instrucciones

void ejecutar_set(pcb*, LineaInstruccion*);
void ejecutar_yield(pcb*, int);
void ejecutar_exit(pcb*, int);
void ejecutar_io(pcb*, LineaInstruccion*, int);
void ejecutar_wait(pcb*, LineaInstruccion*, int);
void ejecutar_signal(pcb*, LineaInstruccion*, int);
void ejecutar_create_segment(pcb*, LineaInstruccion*, int);
void ejecutar_delete_segment(pcb*, LineaInstruccion*, int);
void abrir_o_cerrar_archivo(pcb*, LineaInstruccion*, int, int);
void ejecutar_f_seek(pcb*, LineaInstruccion*, int);
void ejecutar_f_truncate(pcb*, LineaInstruccion*, int);
void ejecutar_f_read_o_f_write(pcb*, LineaInstruccion*, int, int);

//

// Funciones para logear valores varios

void estado_de_los_registros(pcb*);
void logear_instruccion(int, LineaInstruccion*);

//

#endif
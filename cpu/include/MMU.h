#ifndef MMU_H_
#define MMU_H_

#include <cpuUtils.h>
#include <math.h>

int obtener_direccion_fisica(char*, pcb*, int);
int obtener_num_segmento(int);
int obtener_desplazamiento_del_segmento(int);

#endif
#ifndef TEST_H_
#define TEST_H_

#include <utils/conexion.h>
#include <ipc.h>

int socketMemoria;

void conectar_con_memoria();
void proceso( int cod, int pid);
void respuesta_m();
void leer_segmentos(t_list* un_pcb);

#endif /* TEST_H_ */
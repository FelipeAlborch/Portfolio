#ifndef MAIN_H_
#define MAIN_H_

// Con este include ya estaria. No hace falta poner sdtlib, stdio, commons/list. Ya estan incluidas en las generales de la static y eso lo inclui en el utils
#include <memoriaUtils.h>



t_list* todos_los_segmentos; //tipo t_segmento
pthread_t hilo_cpu;
pthread_t hilo_kernel;
pthread_t hilo_fs;
//pthread_t hiloConexion;
int resto_memoria;

extern sem_t sem_fs_conectado;
extern sem_t sem_cpu_conectado;

#endif
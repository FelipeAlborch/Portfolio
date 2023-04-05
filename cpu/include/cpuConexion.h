#ifndef CPUCONEXION_H_
#define CPUCONEXION_H_

#include <cpuConfig.h>

extern int socketMemoriaUtil;

int conectar_con_memoria(config_de_cpu);
void esperar_kernel_dispatch(int);
bool manejar_paquete_kernel_dispatch(int);

#endif
#ifndef KERNELARCHIVOS_H_
#define KERNELARCHIVOS_H_

#include <commons/collections/dictionary.h>
#include "kernelConfig.h"

void inicializar_archivos_kernel();
void liberar_archivos_de_proceso(pcb* proceso);
bool archivo_esta_abierto(char* archivo);

#endif
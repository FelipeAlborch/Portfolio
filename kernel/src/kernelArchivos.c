#include "kernelArchivos.h"

t_dictionary *tabla_global_archivos_abiertos;

void inicializar_archivos_kernel() {
    tabla_global_archivos_abiertos = dictionary_create();
}

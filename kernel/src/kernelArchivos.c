#include "kernelArchivos.h"

t_dictionary *tabla_global_archivos_abiertos;

void inicializar_archivos_kernel() {
    tabla_global_archivos_abiertos = dictionary_create();
}

void archivo_abierto_init(char* nombre) {
    t_recurso* archivo_nuevo = malloc(sizeof(t_recurso));
    archivo_nuevo->nombre = nombre;
    archivo_nuevo->instancias = 1;

    return archivo_nuevo;
}
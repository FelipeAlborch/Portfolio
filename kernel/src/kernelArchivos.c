#include "kernelArchivos.h"

t_dictionary *tabla_global_archivos_abiertos;

void inicializar_archivos_kernel() {
    tabla_global_archivos_abiertos = dictionary_create();
}

void liberar_archivos_de_proceso(pcb* proceso) {
    // TODO: revisar implementacion de liberar archivos de proceso
    // si el archivo solo esta abierto por este proceso, se debe remover de la tabla global

    bool _archivo_esta_abierto(t_recurso* archivo) {
        return archivo_esta_abierto(archivo->nombre) == 1;
    }

    void _liberar_archivo(t_recurso* archivo) {
        // si el archivo solo esta abierto por este proceso, se debe remover de la tabla global
        if(list_any_satisfy(proceso->tabla_archivos_abiertos, (void*) _archivo_esta_abierto)) {
            dictionary_remove(tabla_global_archivos_abiertos, archivo->nombre);
        }
    }

    list_iterate(proceso->tabla_archivos_abiertos, (void*) _liberar_archivo);

    list_clean(proceso->tabla_archivos_abiertos);
}

bool archivo_esta_abierto(char* archivo)
{
    // contador de procesos que tienen el archivo abierto
    int contador = 0;

    // devuelve true si el nombre del recurso matchea con el del archivo
    void _recurso_es_el_archivo(t_recurso* recurso) {
        return string_equals_ignore_case(recurso->nombre, archivo);
    }

    // suma 1 a contador si el pcb tiene el archivo abierto
    void _pcb_tiene_el_archivo(char* ___, pcb* iteration) {
        // safeguard para evitar acceder a una lista ya destruida
        if(iteration == NULL) return;
        bool found = list_any_satisfy(iteration->tabla_archivos_abiertos, (void*) _recurso_es_el_archivo);
        if(found)
            contador++;   
    }

    dictionary_iterator(tabla_de_procesos, (void*) _pcb_tiene_el_archivo);

    return contador != 0;
}
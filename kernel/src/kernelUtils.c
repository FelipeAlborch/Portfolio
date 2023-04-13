#include "kernelUtils.h"

t_log* logger_kernel_util_obligatorio;
t_log* logger_kernel_util_extra;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
t_dictionary* diccionario_de_consolas;

// Los sockets que usaremos en todo el modulo kernel, para las diferentes conexiones salvo las consolas.
int socketMemoria;
int socketCPU;
int socketFS;



void agregar_socket_a_diccionario(int pcb_pid, int socket_consola)
{
    //int *p = malloc(sizeof(int));
	//*p=socket_consola;
	dictionary_put(diccionario_de_consolas, string_from_format("%d", pcb_pid), socket_consola);
}

void leer_diccionario_consolas()
{
    for(int i = 0; i < dictionary_size(diccionario_de_consolas); i++)
    {   
        char* key = string_from_format("%d", i+1);
        int socket_consola = dictionary_get(diccionario_de_consolas, key);
        printf("EL SOCKET ES: %d", socket_consola);
    }
}
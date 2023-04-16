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

double calcular_response_ratio(pcb* un_pcb)
{
    double espera_total = (double)temporal_gettime(un_pcb->llegada_ready)/1000;
    double response_ratio = 1 + (espera_total/un_pcb->estimado_prox_rafaga);

    return response_ratio;
}

bool comparar_response_ratio(pcb* un_pcb, pcb* otro_pcb)
{
    return (calcular_response_ratio(un_pcb) > calcular_response_ratio(otro_pcb));
}

double estimar_proxima_rafaga(pcb* un_pcb)
{
    double tiempo_ejecucion_real = temporal_gettime(un_pcb->tiempo_ejecucion)/1000;
    double alfa = configuracionKernel.HRRN_ALFA;
    double estimado_anterior = un_pcb->estimado_prox_rafaga;
    double nuevo_estimado = (alfa * tiempo_ejecucion_real) + ((1 - alfa) * estimado_anterior);
    return nuevo_estimado;
}

void liberar_contexto_ejecucion(pcb* un_contexto)
{
    free(un_contexto->AX);
    free(un_contexto->BX);
    free(un_contexto->CX);
    free(un_contexto->DX);
    free(un_contexto->EAX);
    free(un_contexto->EBX);
    free(un_contexto->ECX);
    free(un_contexto->EDX);
    free(un_contexto->RAX);
    free(un_contexto->RBX);
    free(un_contexto->RCX);
    free(un_contexto->RDX);
    list_destroy_and_destroy_elements(un_contexto->lista_de_instrucciones, (void*)liberar_instruccion);
    free(un_contexto);
}
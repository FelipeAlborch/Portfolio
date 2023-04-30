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
        free(key);
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



void avisar_cpu(int aviso)
{
    t_paquete* paquete_a_cpu = crear_paquete_operacion(aviso);
    enviar_paquete(paquete_a_cpu, socketCPU);
    eliminar_paquete(paquete_a_cpu);
}

void avisar_memoria(int aviso)
{
    t_paquete* paquete_a_memoria = crear_paquete_operacion(aviso);
    enviar_paquete(paquete_a_memoria, socketMemoria);
    eliminar_paquete(paquete_a_memoria);
}



void solicitar_creacion_segmento(int nro_segmento, int tam_segmento, int pid_proceso)
{
    t_paquete* paquete_cs = crear_paquete_operacion(CREATE_SEGMENT);
    agregar_a_paquete(paquete_cs, &nro_segmento, sizeof(int));
    agregar_a_paquete(paquete_cs, &tam_segmento, sizeof(int));
    agregar_a_paquete(paquete_cs, &pid_proceso, sizeof(int));
    enviar_paquete(paquete_cs, socketMemoria);
    eliminar_paquete(paquete_cs);
}

void solicitar_eliminacion_segmento(int nro_segmento, int pid_proceso)
{
    t_paquete* paquete_cs = crear_paquete_operacion(DELETE_SEGMENT);
    agregar_a_paquete(paquete_cs, &nro_segmento, sizeof(int));
    agregar_a_paquete(paquete_cs, &pid_proceso, sizeof(int));
    enviar_paquete(paquete_cs, socketMemoria);
    eliminar_paquete(paquete_cs);
}


void remover_recurso_si_esta(t_list* lista,recurso* un_recurso)
{
    for(int i = 0; i < list_size(lista); i++)
    {
        recurso* un_rec = list_get(lista, i);
        if(un_recurso->nombre == un_rec->nombre)
        {
            list_remove(lista, i);
        }
    }
}

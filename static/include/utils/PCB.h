#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>             // Para usar uint32_t etc.
#include <utils/paquete.h>

typedef enum
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} estado_pcb;

typedef struct pcb
{
    int pid;
    int program_counter;
    /*
    *   A completar
    */
    estado_pcb estado;
    t_list* lista_de_instrucciones;
    t_list* tabla_de_segmentos;
} pcb;



#endif
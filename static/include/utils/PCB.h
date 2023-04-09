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

    char AX[4];
    char BX[4];
    char CX[4];
    char DX[4];
    char EAX[8];
    char EBX[8];
    char ECX[8];
    char EDX[8];
    char RAX[16];
    char RBX[16];
    char RCX[16];
    char RDX[16];
    
    estado_pcb estado;
    t_list* lista_de_instrucciones;
    t_list* tabla_de_segmentos;

    int estimado_prox_rafaga;
} pcb;

typedef struct LineaInstruccion
{
  char *identificador;
  char *parametros[3];
} LineaInstruccion;

/*typedef enum parametros
{
  AX,
  BX,
  CX,
  DX,
  EAX,
  EBX,
  ECX,
  EDX,
  RAX,
  RBX,
  RCX,
  RDX,
  DISCO,
  ARCHIVO,
} parametros; */

void* liberar_instruccion(LineaInstruccion*);

#endif
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

typedef enum Instruccion
{
  DESCONOCIDA = -1,
  SET,
  MOV_IN_INSTRUCTION,
  MOV_OUT_INSTRUCTION,
  IO,
  F_OPEN,
  F_CLOSE,
  F_SEEK,
  F_READ,
  F_WRITE,
  F_TRUNCATE,
  WAIT,
  SIGNAL,
  CREATE_SEGMENT_INSTRUCTION,
  DELETE_SEGMENT_INSTRUCTION,
  YIELD,
  EXIT
} Instruccion;

typedef struct pcb
{
    int pid;
    int program_counter;
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
    t_list* tabla_archivos_abiertos;
    t_list* recursos_asignados;

    t_temporal* llegada_ready;
    t_temporal* tiempo_ejecucion;
    double estimado_prox_rafaga;
    int tiempo_io;
} pcb;

typedef struct LineaInstruccion
{
  char *identificador;
  char *parametros[3];
} LineaInstruccion;


typedef struct {
   // int id_seg;
    int base;
    int size;
   // int libre;
}t_segmento;

/**
* @NAME: crear_pcb
* @DESC: Crea un pcb para y se le asigna el contenido de la consola simulando un proceso.
* @PARAMS:
*        t_list* lista_de_instrucciones - La lista de instrucciones de la consoa.
*        int p_id - La variable global del KERNEL que representa el process id.
*        int estimado_rafaga - El estimado de rafaga que llega por config en el kernel
* @RETURN:
*        retorna el pcb creado.
*/
pcb* crear_pcb(t_list*, int, int);

/**
* @NAME: loguear_pcb
* @DESC: Funcion encargada de loguear un pcb.
* @PARAMS:
*        pcb* un_pcb - El pcb que se desea loguear.
*        t_log* logger - El logguer donde sera logueado el pcb.
*/
void loguear_pcb(pcb*, t_log*);

/**
* @NAME: identificar_estado
* @DESC: A partir de un estado de un pcb, devuelve el string que representa dicho estado.
* @PARAMS:
*        estado_pcb un_estado - el estado del pcb.
* @RETURN:
*        retorna char* que representa al estado.
*/
char* identificar_estado(estado_pcb un_estado);

/**
* @NAME: liberar_pcb
* @DESC: Funcion encargada de liberar la memoria que ocupa un pcb.
* @PARAMS:
*        pcb* un_pcb - El pcb que se desea liberar.
*/
void liberar_pcb(pcb*);

/**
* @NAME: loguear_lista_de_instrucciones
* @DESC: Funcion encargada de loguear una lista de instrucciones.
* @PARAMS:
*        t_list* lista_de_instrucciones - La lista de instrucciones que se desea loguear.
*        t_log* logger - El logguer donde sera logueada la lista.
*/
void loguear_lista_de_instrucciones(t_list*, t_log*);

/**
* @NAME: liberar_instruccion
* @DESC: Funcion encargada de liberar la memoria que ocupa una LineaInstruccion.
* @PARAMS:
*        LineaInstruccion* una_instruccion - la instruccion que se desea liberar.
*/
void* liberar_instruccion(LineaInstruccion*);
void dump(Lista*);
pcb* recibir_contexto_ejecucion(t_list*);
void enviar_contexto_ejecucion(pcb*, int, int);
void serializar_contexto_ejecucion(t_paquete*, pcb*);
void actualizar_contexto_ejecucion(pcb*, pcb*);
void liberar_contexto_ejecucion(pcb*);

/**
 * @NAME: valor_del_registro_como_string
 * @DESC: formatea el valor del registro para imprimirse como string
 * @PARAMS:
 *      void* registro - el registro a formatear
 *      size_t tamanio - el tamanio del registro
 * @RETURN:
 *     retorna un string con el valor del registro formateado
 *     (nota: liberar la memoria luego de usarlo)
 */
char* valor_del_registro_como_string(void* registro, size_t tamano);


t_list* deserializar_tabla_segmentos(t_list*);

void serializar_tabla_segmentos(t_paquete*, t_list*);

void liberar_segmento(t_segmento*);

#endif
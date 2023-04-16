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
  MOV_IN,
  MOV_OUT,
  IO,
  F_OPEN,
  F_CLOSE,
  F_SEEK,
  F_READ,
  F_WRITE,
  F_TRUNCATE,
  WAIT,
  SIGNAL,
  //CREATE_SEGMENT,
  //DELETE_SEGMENT,
  YIELD,
  EXIT
} Instruccion;

typedef struct pcb
{
    int pid;
    int program_counter;
    char* AX;
    char* BX;
    char* CX;
    char* DX;
    char* EAX;
    char* EBX;
    char* ECX;
    char* EDX;
    char* RAX;
    char* RBX;
    char* RCX;
    char* RDX;
    
    estado_pcb estado;
    t_list* lista_de_instrucciones;
    t_list* tabla_de_segmentos;
    t_list* tabla_archivos_abiertos;

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

//void enviar_pcb(pcb*, int);
//
//void serializar_pcb(t_paquete*, pcb*);
//
//pcb* recibir_pcb(t_list*);

/**
* @NAME: enviar_contexto_ejecucion
* @DESC: Dado un pcb, guarda su contexto de ejecucion en un paquete, y lo envia por el socket, bajo un determinado codigo.
* @PARAMS:
*        pcb* un_pcb - El pcb/contexto_de_ejecucion que se desea enviar.
*        int socket_a_enviar - El socket por el que se quiere pasar el contexto.
*        int codigo - El codigo de operacion por el cual se envia dicho contexto.
*               Codigos: 
*                       CPU -> KERNEL YIELD, IO, EXIT.
*                       KERNEL -> CPU CONTEXTO_EJECUCION
*/
void enviar_contexto_ejecucion(pcb*, int, int);

/**
* @NAME: serializar_contexto_ejecucion
* @DESC: Dado un paquete y un pcb, guarda los valores del contexto de ejecucion del pcb, en el paquete.
* @PARAMS:
*        t_paquete* paquete_a_enviar - El paquete donde seran guardados los valores.
*        pcb* un_pcb - El pcb que se desea serializar su contexto de ejecucion.
*/
void serializar_contexto_ejecucion(t_paquete*, pcb*);

/**
* @NAME: recibir_contexto_ejecucion
* @DESC: Dada una lista con los valores recibidos a traves de un socket, crea un pcb* y guarda los valores del contexto de ejecucion en dicho pcb*.
* @PARAMS:
*        t_list* valores_contexto_enviado - La lista con los valores recibidos.
* @RETURN:
*        retorna el pcb* con los valores recibidos.
* 
* @ACLARACION: esta funcion deja muchos valores del pcb inicializados en basura. La intencion es crear un pcb, con solo los valores necesarios para
*              que el cpu pueda ejecutar correctamente, y para que el kernel pueda actualizar los pcbs con los contextos recibidos.
*/
pcb* recibir_contexto_ejecucion(t_list*);

/**
* @NAME: actualizar_contexto_ejecucion
* @DESC: Dados un pcb* completo, y un pcb* con solo el contexto de ejecucion cargados, guarda los valores del segundo, en el primero
*        Es decir: Actualiza los valores del contexto del primer pcb, con los del segundo
* @PARAMS:
*        pcb* un_pcb - El pcb a actualizar.
*        pcb* otro_pcb - El contexto de ejecucion de donde se tomaran los valores para actualizar el primer pcb.
* @ACLARACION:
*        Luego de usar el segundo pcb, hay que liberar la memoria.
* 
*/
void actualizar_contexto_ejecucion(pcb*, pcb*);

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



#endif
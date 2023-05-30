#ifndef MEMORIAUTILS_H_
#define MEMORIAUTILS_H_

// Este include ya tiene incluidos muchos archivos de las commons y cosas tipo stdlib, stdio etc, tambien pcb y cosas del estilo.
#include <utils/conexion.h>
#include <ipc.h>

#define FIRST_FIT 500
#define BEST_FIT 502
#define WORST_FIT 503
#define TAM_CABECERA sizeof(t_segmento)
#define TAM_PAQ sizeof(t_paquete)
#define LIBRE true
#define OCUPADO 100
#define M_ERROR -100

extern t_log* mlogger;
extern t_log* klogger;
extern t_log* loggerMemoria;
extern int clientes[4];
extern int conexion;
extern int running_k;
extern int server_m;
extern t_config* memoriaConfig;
extern pthread_t hilo_cpu;
extern pthread_t hilo_kernel;
extern pthread_t hilo_fs;
extern pthread_t hiloConexion;
extern void* memoria;

//extern t_tabla_segmentos* tabla_segmentos; //listado de t_segmentos de todos los procesos
extern t_list* huecos_libres; //listado de t_hueco_libre
extern t_list* tabla_segmentos_gral; //listado de t_segmentos de todos los procesos

typedef struct {
    int inicio;
    int tamanio;
    int estado;
}t_hueco_libre;


typedef struct 
{   
    int index; // id del segmento de la tabla de segmentos
    int pid;
    int direcion_fisica;
    t_segmento* segmento;
}t_tabla_segmentos;


void sigHandler_sigint(int signo);
void startSigHandlers(void);

void terminar_programa(t_log* logger, t_config* configMemoria);
void liberar_memoria();
void liberar_listas();
void liberar_conexion_memoria();
void liberar_t_config();

void inicializar_configuracion();
void inicializar_memoria();
void inicializar_segmentos();
void inicializar_logs();

// Lo del conexion:
void conectar();
void conectar_cpu();
void conectar_kernel();
void conectar_fs();

void ejecutar_kernel();
void ejecutar_cpu();
void ejecutar_fs();

// Lo del config:

typedef struct{
    char* puerto;
    char* ip;
    char* algoritmo;
    int algoritmo_int; 
    int cant_seg; // cantidad de segmentos por proceso
    int retardo;
    int compactacion;
    int tam_memo;
    int tam_seg_0;
    int cant_seg_max;
    int bytes_libres;
    int cpu;    // para guardar la conexion
    int fs;     // para guardar la conexion
    int kernel; // para guardar la conexion

}config_de_memoria;
extern config_de_memoria config_memo;
void obtener_valores_de_configuracion_memoria(t_config*);
void mostrar_valores_de_configuracion_memoria();
void algoritmos();
// ID --> para el id del segmento y para la direc fisica
void loggear(int code, int pid, void* algo, int id, int size, int base);
void respuestas(int cliente, int code,void* algo);

void crear_estructuras();

void* leer_dato(int pid, int direccion, int size);
void escribir_dato(int pid, int direccion, int size, void* valor);

void eliminar_segmento(int pid, int direccion);
void eliminar_segmento_list(t_segmento* segmento, t_list* segmentos);



void compactar();
void actualizar_memoria(int size, int estado);


void modificar_tabla_proceso(int pid, int index, int base, int size);
void modificar_tabla_segmentos(t_tabla_segmentos* tabla,int pid, int dir, int index,int base, int size);
void modificar_segmento(t_segmento* segmento, int base, int size);


void imprimir_tabla(t_list* lista);
void imprimir_tabla_gral();


void ejecutar_kernel_test();
void crear_proceso(int paquete);
void eliminar_proceso(int pid);
void liberar_proceso(int pid);
int buscar_en_tabla_index(int pid);
t_list* crear_tabla_proceso(int pid);
t_list* tabla_proceso(int pid);

void liberar_hueco(int index);
void modificar_hueco(int index, int inicio, int tam, int estado);
int buscar_hueco_libre(int size);
int buscar_hueco_base(int base);
int base_hueco(int index);
t_hueco_libre* crear_hueco_libre(int inicio, int tam,int estado);

int first_fit(int size);
int best_fit(int size);
int worst_fit(int size);

void create_segment(int pid,int tam,int id);
void* buscar_en_tabla_id(int pid, int id);
t_tabla_segmentos* crear_tabla_segmentos(int pid, int index, int base, int tam_seg);

t_segmento* buscar_segmento(int id_seg, t_list* segmentos);
t_segmento* crear_segmento(int pid, int size);

#endif
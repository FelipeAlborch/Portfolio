#include <colas.h>

// LOGGER PARA EL PLANIFICADOR
t_log* logger_planificador_obligatorio;
t_log* logger_planificador_extra;

// Colas de planificacion
t_queue* cola_new;
t_queue* cola_ready_fifo;
t_queue* cola_exec;
t_queue* cola_terminated;

// Lista para procesos en ready por HRRN. tiene que ser una lista asi la puedo filtrar por Response Ratio
t_list* lista_ready_hrrn;

// Semaforos mutex para las colas de los procesos.
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_terminated;

// Semaforos para activar los hilos de planificacion.
sem_t activar_largo_plazo;
sem_t activar_corto_plazo;

// Un semaforo para saber si hay un proceso ejecutando en el cpu o no.
sem_t sem_hay_en_running;

// Semaforo para la mutliprogramacion.
sem_t grado_multiprogramacion;

bool es_fifo = 0;

void agregar_proceso_new(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_new);

    queue_push(cola_new, un_pcb);
    log_trace(logger_planificador_obligatorio, "El proceso < %d > se movio a NEW", un_pcb->pid);

    pthread_mutex_unlock(&mutex_new);
    sem_post(&activar_largo_plazo);
}

void agregar_proceso_ready(pcb* un_pcb)
{
    un_pcb->estado = READY;
    if(es_fifo)
    {
        pthread_mutex_lock(&mutex_ready);

        queue_push(cola_ready_fifo, un_pcb);
        log_trace(logger_planificador_obligatorio, "El proceso < %d > se movio a READY", un_pcb->pid);
        log_trace(logger_planificador_obligatorio, "Cola READY < %s >:", configuracionKernel.ALGORITMO_PLANIFICACION);
        loguear_procesos_en_cola(cola_ready_fifo);

        pthread_mutex_unlock(&mutex_ready);
        sem_post(&activar_corto_plazo);
    }else
    {
        pthread_mutex_lock(&mutex_ready);

        list_add(lista_ready_hrrn, un_pcb);
        un_pcb->llegada_ready = temporal_create();
        log_trace(logger_planificador_obligatorio, "El proceso < %d > se movio a READY", un_pcb->pid);
        log_trace(logger_planificador_obligatorio, "Cola READY < %s >:", configuracionKernel.ALGORITMO_PLANIFICACION);
        loguear_procesos_en_lista(lista_ready_hrrn);

        pthread_mutex_unlock(&mutex_ready);
        sem_post(&activar_corto_plazo);
    }
}

void agregar_proceso_exec(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_exec);

    un_pcb->estado = RUNNING;
    queue_push(cola_exec, un_pcb);
    log_trace(logger_planificador_obligatorio, "El proceso < %d > se movio a EXEC", un_pcb->pid);

    pthread_mutex_unlock(&mutex_exec);
}

void agregar_proceso_terminated(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_terminated);

    un_pcb->estado = TERMINATED;
    queue_push(cola_terminated, un_pcb);
    log_trace(logger_planificador_obligatorio, "El proceso < %d > se movio a TERMINATED", un_pcb->pid);

    pthread_mutex_unlock(&mutex_terminated);
    sem_post(&grado_multiprogramacion);
}

pcb* obtener_proceso_new()
{
    pcb* proceso_new;

    pthread_mutex_lock(&mutex_new);

    proceso_new = queue_pop(cola_new);
    log_trace(logger_planificador_obligatorio, "El proceso: < %d > salio de NEW", proceso_new->pid);

    pthread_mutex_unlock(&mutex_new);

    return proceso_new;
}

pcb* obtener_proceso_ready()
{
    pcb* proceso_ready;

    pthread_mutex_lock(&mutex_ready);

    proceso_ready = queue_pop(cola_ready_fifo);
    log_trace(logger_planificador_obligatorio, "El proceso: < %d > salio de READY", proceso_ready->pid);

    pthread_mutex_unlock(&mutex_ready);

    return proceso_ready;
}


pcb* desalojar_proceso_en_exec()
{
    pthread_mutex_lock(&mutex_exec);

    pcb* proceso_desalojado = queue_pop(cola_exec);
    log_trace(logger_planificador_obligatorio, "El proceso: < %d > salio de EXEC", proceso_desalojado->pid);

    pthread_mutex_unlock(&mutex_exec);
    sem_post(&sem_hay_en_running);

    return proceso_desalojado;
}


void loguear_cola_de_procesos(t_queue* cola_de_procesos)
{
    for(int i = 0; i < queue_size(cola_de_procesos); i++)
    {
        pcb* un_proceso = (pcb*)queue_peek_at(cola_de_procesos,i);
        loguear_pcb(un_proceso, logger_planificador_extra);
    }
}

void* queue_peek_at(t_queue* cola, int i)
{
    return list_get(cola->elements, i);
}

void loguear_procesos_en_cola(t_queue* cola_de_procesos)
{
    for(int i = 0; i < queue_size(cola_de_procesos); i++)
    {
        pcb* un_proceso = (pcb*)queue_peek_at(cola_de_procesos,i);
        log_debug(logger_planificador_obligatorio, "Proceso: < %d >", un_proceso->pid);
    }
}

void loguear_procesos_en_lista(t_list* lista_de_procesos)
{
    for(int i = 0; i < list_size(lista_de_procesos); i++)
    {
        pcb* un_proceso = list_get(lista_de_procesos,i);
        log_debug(logger_planificador_obligatorio, "Proceso: < %d >", un_proceso->pid);
    }
}
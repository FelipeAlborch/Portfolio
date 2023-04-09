#include <planificador.h>

// SOCKET AL CPU
int socket_cpu_planificador;

// LOGGER PARA EL PLANIFICADOR
t_log* logger_planificador_obligatorio;
t_log* logger_planificador_extra;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
t_dictionary* diccionario_de_consolas;

// variable global para el process id
int pid_global = 0;

// Colas de planificacion
t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_exec;
t_queue* cola_terminated;

// Cola blockeo va a haber varias, TO-DO: posiblemente un hilo que haga el IO por proceso.

// Semaforos mutex para las colas de los procesos.
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_terminated;

// Semaforos para activar los hilos de planificacion.
sem_t activar_largo_plazo;
sem_t activar_corto_plazo;

// Semaforo para la mutliprogramacion.
sem_t grado_multiprogramacion;

// Hilos de planificacion.
pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;

void inicializar_planificadores(config_de_kernel config_kernel) {
    logger_planificador_extra = iniciar_logger_modulo(KERNEL_LOGGER_EXTRA);
    logger_planificador_obligatorio = iniciar_logger_modulo(KERNEL_LOGGER);
    
    log_info(logger_planificador_extra, "Iniciando planificadores... ");

    // Inicializo los semaforos.
    sem_init(&activar_largo_plazo, 0, 0);
    sem_init(&activar_corto_plazo, 0, 0);
    sem_init(&grado_multiprogramacion, 0, config_kernel.GRADO_MAX_MULTIPROGRAMACION);

    // Inicializo los mutex.
    pthread_mutex_init(&mutex_new, NULL);
    pthread_mutex_init(&mutex_ready, NULL);
    pthread_mutex_init(&mutex_exec, NULL);
    pthread_mutex_init(&mutex_terminated, NULL);

    // Inicializo las colas de planificacion.
    cola_new = queue_create();
    cola_ready = queue_create();
    cola_exec = queue_create();
    cola_terminated = queue_create();

    // Inicializo el diccionario de consolas.
    diccionario_de_consolas = dictionary_create();

    // Creo los hilos de planificacion.
    pthread_create(&hilo_largo_plazo, NULL, (void*) planificador_largo_plazo, NULL);
    pthread_create(&hilo_corto_plazo, NULL, (void*) planificador_corto_plazo, NULL);
}

void planificador_largo_plazo() {
    while(1) {
        log_info(logger_planificador_extra, "Esperando proceso en cola NEW...");
        sem_wait(&activar_largo_plazo);
        log_info(logger_planificador_extra, "Proceso en cola NEW!");
        // TO-DO: Implementar el planificador largo plazo.
    }
}

void planificador_corto_plazo() {
    while(1) {
        log_info(logger_planificador_extra, "Esperando proceso en cola READY...");
        sem_wait(&activar_corto_plazo);
        log_info(logger_planificador_extra, "Proceso en cola READY!");
        // TO-DO: Implementar el planificador corto plazo.
    }
}


void agregar_proceso_new(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_new);

    queue_push(cola_new, un_pcb);
    log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a NEW", un_pcb->pid);

    pthread_mutex_unlock(&mutex_new);
    sem_post(&activar_largo_plazo);
}

void agregar_proceso_ready(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_ready);

    queue_push(cola_ready, un_pcb);
    log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a READY", un_pcb->pid);

    pthread_mutex_unlock(&mutex_ready);
    sem_post(&activar_corto_plazo);
}

void agregar_proceso_exec(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_exec);

    queue_push(cola_exec, un_pcb);
    log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a EXEC", un_pcb->pid);

    pthread_mutex_unlock(&mutex_exec);
}

void agregar_proceso_terminated(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_terminated);

    queue_push(cola_terminated, un_pcb);
    log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a TERMINATED", un_pcb->pid);

    pthread_mutex_unlock(&mutex_terminated);
    sem_post(&grado_multiprogramacion);
}

void agregar_proceso_block(pcb* un_pcb)
{
    // TO-DO: Implementar el bloqueo de procesos.
}
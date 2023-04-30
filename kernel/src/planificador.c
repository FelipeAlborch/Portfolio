#include <planificador.h>

// Estos sockets ya no los necesitamos porque puse unos globales en un archivo previo que se incluye aca, y se incializan en el main.
//int socket_cpu_planificador;
//int socket_memoria_planificador;
//int socket_fs_planificador;

// LOGGER PARA EL PLANIFICADOR
t_log* logger_planificador_obligatorio;
t_log* logger_planificador_extra;


// variable global para el process id
int pid_global = 0;

// Colas de planificacion
t_queue* cola_new;
t_queue* cola_ready_fifo;
t_queue* cola_exec;
t_queue* cola_terminated;

// Lista para procesos en ready por HRRN. tiene que ser una lista asi la puedo filtrar por Response Ration
t_list* lista_ready_hrrn;

// Cola blockeo va a haber varias, TO-DO: posiblemente un hilo que haga el IO por proceso.

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

// Hilos de planificacion.
pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;

bool es_fifo = 0;
bool resultado_recurso = true;
bool proceso_bloqueado_por_wait = false;


void inicializar_estructuras_planificacion() {
    logger_planificador_extra = iniciar_logger_modulo(KERNEL_LOGGER_EXTRA);
    logger_planificador_obligatorio = iniciar_logger_modulo(KERNEL_LOGGER);

    logger_kernel_util_obligatorio = iniciar_logger_modulo(KERNEL_LOGGER);
    logger_kernel_util_extra = iniciar_logger_modulo(KERNEL_LOGGER_EXTRA);
    
    log_info(logger_planificador_extra, "Iniciando planificadores... ");

    // Inicializo los semaforos.
    sem_init(&activar_largo_plazo, 0, 0);
    sem_init(&activar_corto_plazo, 0, 0);
    sem_init(&grado_multiprogramacion, 0, configuracionKernel.GRADO_MAX_MULTIPROGRAMACION);
    sem_init(&sem_hay_en_running, 0, 1);                                                // Si hay uno en running, deberia estar inicializado en 1

    // Inicializo los mutex.
    pthread_mutex_init(&mutex_new, NULL);
    pthread_mutex_init(&mutex_ready, NULL);
    pthread_mutex_init(&mutex_exec, NULL);
    pthread_mutex_init(&mutex_terminated, NULL);

    // Inicializo las colas de planificacion.
    cola_new = queue_create();
    cola_ready_fifo = queue_create();
    cola_exec = queue_create();
    cola_terminated = queue_create();

    lista_ready_hrrn = list_create();

    // Inicializo el diccionario de consolas.
    diccionario_de_consolas = dictionary_create();
}

void destruir_estructuras_planificacion()
{
    // Destruyo loggers
    log_destroy(logger_planificador_extra);
    log_destroy(logger_planificador_obligatorio);

    log_destroy(logger_kernel_util_extra);
    log_destroy(logger_kernel_util_obligatorio);

    // Destruyo semaforos tipo sem_t
    sem_close(&activar_largo_plazo);
    sem_close(&activar_corto_plazo);
    sem_close(&grado_multiprogramacion);
    sem_close(&sem_hay_en_running);

    // Destruyo semaforos tipo pthread_mutex_t
    pthread_mutex_destroy(&mutex_new);
    pthread_mutex_destroy(&mutex_ready);
    pthread_mutex_destroy(&mutex_exec);
    pthread_mutex_destroy(&mutex_terminated);

    // Destruyo colas.
    // Habria que primero destruir los elementos de las colas.
    queue_destroy(cola_new);
    queue_destroy(cola_ready_fifo);
    queue_destroy(cola_exec);
    queue_destroy(cola_terminated);

    list_destroy_and_destroy_elements(lista_ready_hrrn, (void*)liberar_pcb);

    // Destruyo diccionarios;
    dictionary_destroy(diccionario_de_consolas);
    dictionary_destroy_and_destroy_elements(diccionario_recursos, liberar_recurso);

    // Termino los hilos
    pthread_exit(&hilo_corto_plazo);
    pthread_exit(&hilo_largo_plazo);
}

void iniciar_planificadores()
{
    pthread_create(&hilo_largo_plazo, NULL, planificador_largo_plazo, NULL);
    pthread_detach(hilo_largo_plazo);

    if(strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "FIFO") == 0)
    {
        pthread_create(&hilo_corto_plazo, NULL, planificador_corto_plazo_fifo, NULL);
        pthread_detach(hilo_corto_plazo);
        es_fifo = 1;
    }
    else if(strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "HRRN") == 0)
    {
        pthread_create(&hilo_corto_plazo, NULL, planificador_corto_plazo_hrrn, NULL);
        pthread_detach(hilo_corto_plazo);
    }
}

void* planificador_largo_plazo() {
    
    log_info(logger_planificador_extra, "Inicio de planificador de largo plazo");
    while(1) {
        
        sem_wait(&activar_largo_plazo);
        sem_wait(&grado_multiprogramacion);

        log_info(logger_planificador_extra, "El grado de multiprogramacion permite agregar un proceso a ready");

        pcb* proceso = obtener_proceso_new();
        agregar_proceso_ready(proceso);
    }
}

void* planificador_corto_plazo_fifo()
{
    log_info(logger_planificador_extra, "Inicio de planificador de corto plazo en <FIFO>");
    while(1)
    {
        sem_wait(&activar_corto_plazo);
        sem_wait(&sem_hay_en_running);

        pcb* proceso_a_ejecutar = obtener_proceso_ready();

        agregar_proceso_exec(proceso_a_ejecutar);
        ejecutar(proceso_a_ejecutar);
    }
}

void* planificador_corto_plazo_hrrn()
{
    log_info(logger_planificador_extra, "Inicio de planificador de corto plazo en <HRRN>");
    while(1)
    {
        sem_wait(&activar_corto_plazo);
        sem_wait(&sem_hay_en_running);

        // Antes de mandar el proximo a ejecutar, acomodamos la lista por response ratio
        list_sort(lista_ready_hrrn, comparar_response_ratio);
        // Saco el primer proceso de la lista filtrada
        pcb* proceso_a_ejecutar = list_remove(lista_ready_hrrn,0);

        agregar_proceso_exec(proceso_a_ejecutar);
        
        // Frenamos el tiempo de espera en ready
        temporal_destroy(proceso_a_ejecutar->llegada_ready);
        // Empezamos a contar el tiempo de ejecucion para el futuro calculo del estimado de proxima rafaga
        proceso_a_ejecutar->tiempo_ejecucion = temporal_create();
        
        ejecutar(proceso_a_ejecutar);
    }
}


void ejecutar(pcb* proceso_a_ejecutar)
{
    enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
    log_info(logger_planificador_extra, "Se envia el proceso: < %d > al CPU.", proceso_a_ejecutar->pid);

    pcb* contexto_recibido;
    t_list* lista_recepcion_valores;
    pcb* proceso_en_ejecucion;
    char* recurso;

    recibirOperacion:
    int operacion_de_cpu = recibir_operacion(socketCPU);

    switch(operacion_de_cpu)
    {
        case YIELD:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            contexto_recibido = recibir_contexto_ejecucion(lista_recepcion_valores);
            log_info(logger_planificador_extra, "Contexto del proceso: < %d > recibido por YIELD.", proceso_a_ejecutar->pid);
            proceso_en_ejecucion = desalojar_proceso_en_exec();
            
            if(!es_fifo)
            {
                proceso_en_ejecucion->estimado_prox_rafaga = estimar_proxima_rafaga(proceso_en_ejecucion);
                temporal_destroy(proceso_en_ejecucion->tiempo_ejecucion);
            }
            
            actualizar_contexto_ejecucion(proceso_en_ejecucion, contexto_recibido);
            loguear_pcb(proceso_en_ejecucion, logger_planificador_extra);


            agregar_proceso_ready(proceso_en_ejecucion);
            

            list_destroy(lista_recepcion_valores);
            liberar_contexto_ejecucion(contexto_recibido);
            
        break;

        case IO:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            contexto_recibido = recibir_contexto_ejecucion(lista_recepcion_valores);
            proceso_en_ejecucion = desalojar_proceso_en_exec();
            proceso_en_ejecucion->estado = BLOCKED;
            
            log_info(logger_planificador_obligatorio, "PID: < %d > - Bloqueado por: < IO >" , proceso_en_ejecucion->pid);

            if(!es_fifo)
            {
                proceso_en_ejecucion->estimado_prox_rafaga = estimar_proxima_rafaga(proceso_en_ejecucion);
                temporal_destroy(proceso_en_ejecucion->tiempo_ejecucion);
            }
            
            actualizar_contexto_ejecucion(proceso_en_ejecucion, contexto_recibido);
            loguear_pcb(proceso_en_ejecucion,logger_kernel_util_extra);
            

            pthread_t hilo_io;
            pthread_create(&hilo_io, NULL, esperar_io, (void*) proceso_en_ejecucion);
            pthread_detach(hilo_io);

            list_destroy(lista_recepcion_valores);
            liberar_contexto_ejecucion(contexto_recibido);
        break;
        
        case EXIT:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            contexto_recibido = recibir_contexto_ejecucion(lista_recepcion_valores);
            proceso_en_ejecucion = desalojar_proceso_en_exec();

            actualizar_contexto_ejecucion(proceso_en_ejecucion, contexto_recibido);
            loguear_pcb(proceso_en_ejecucion,logger_planificador_extra);
            log_info(logger_planificador_obligatorio, "Finaliza el proceso < %d > - Motivo: < SUCCESS >", proceso_en_ejecucion->pid);
            
            //agregar_proceso_terminated(proceso_en_ejecucion);
            terminar_proceso(proceso_en_ejecucion);
            //TODO
            //avisar_memoria(FIN_PROCESO);

            list_destroy(lista_recepcion_valores);
            liberar_contexto_ejecucion(contexto_recibido);
        break;

        case WAIT:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Recurso recibido para realizar WAIT: %s", recurso);

            int operacion_wait = recibir_operacion(socketCPU);
            t_list* lista_contexto_wait = _recibir_paquete(socketCPU);
            pcb* contexto_de_wait = recibir_contexto_ejecucion(lista_contexto_wait);

            log_info(logger_planificador_extra, "Contexto recibido por WAIT");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_wait);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            wait_recurso(proceso_a_ejecutar, recurso);


            if(!resultado_recurso)
            {   
                log_warning(logger_planificador_extra, "El recurso NO EXISTE. Terminando proceso");
                proceso_en_ejecucion = desalojar_proceso_en_exec();
                
                log_info(logger_planificador_obligatorio, "Finaliza el proceso < %d > - Motivo: < NO EXISTE RECURSO >", proceso_en_ejecucion->pid);
                
                terminar_proceso(proceso_en_ejecucion);

                resultado_recurso = true;

                liberar_contexto_ejecucion(contexto_de_wait);
                list_destroy(lista_recepcion_valores);
                list_destroy(lista_contexto_wait);
                return;
            }

            liberar_contexto_ejecucion(contexto_de_wait);
            list_destroy(lista_recepcion_valores);
            list_destroy(lista_contexto_wait);
            
            if(proceso_bloqueado_por_wait)
            {
                proceso_bloqueado_por_wait = false;
                return;
            }

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;
        
        break;

        case SIGNAL:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Recurso recibido para realizar SIGNAL: %s", recurso);
            
            int operacion_signal = recibir_operacion(socketCPU);
            t_list* lista_contexto_signal = _recibir_paquete(socketCPU);
            pcb* contexto_de_signal = recibir_contexto_ejecucion(lista_contexto_signal);

            log_info(logger_planificador_extra, "Contexto recibido por SIGNAL");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_signal);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            signal_recurso(proceso_a_ejecutar, recurso);

            if(!resultado_recurso)
            {   
                log_warning(logger_planificador_extra, "El recurso NO EXISTE. Terminando proceso");
                proceso_en_ejecucion = desalojar_proceso_en_exec();
                
                log_info(logger_planificador_obligatorio, "Finaliza el proceso < %d > - Motivo: < NO EXISTE RECURSO >", proceso_en_ejecucion->pid);
                
                terminar_proceso(proceso_en_ejecucion);
                
                resultado_recurso = true;

                liberar_contexto_ejecucion(contexto_de_signal);
                list_destroy(lista_recepcion_valores);
                list_destroy(lista_contexto_signal);
                
                return;
            }

            liberar_contexto_ejecucion(contexto_de_signal);
            list_destroy(lista_recepcion_valores);
            list_destroy(lista_contexto_signal);
            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;

        break;

        case CREATE_SEGMENT:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            int nro_segmento_crear = *(int*) list_get(lista_recepcion_valores, 0);
            int tam_segmento = *(int*) list_get(lista_recepcion_valores, 1);

            int op_create_seg = recibir_operacion(socketCPU);
            t_list* lista_contexto_cs = _recibir_paquete(socketCPU);
            pcb* contexto_cs = recibir_contexto_ejecucion(lista_contexto_cs);

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_cs);
            loguear_pcb(proceso_a_ejecutar, logger_planificador_extra);

            log_info(logger_kernel_util_obligatorio, "PID: < %d > - Crear Segmento - Id: < %d > - Tamaño:< %d >", proceso_a_ejecutar->pid, nro_segmento_crear, tam_segmento);
            
            log_warning(logger_kernel_util_extra, "CREATE_SEGMENT TODAVIA NO IMPLMENETADO");
            
            //crear_segmento:
            //solicitar_creacion_segmento(nro_segmento_crear, tam_segmento, proceso_a_ejecutar->pid);

            //int rta_crecion_memoria;
            //recv(socketMemoria, &rta_crecion_memoria, sizeof(int), MSG_WAITALL);
            /*
            int rta_crecion_memoria = recibir_operacion(socketMemoria);
            switch(rta_crecion_memoria)
            {
                case CREATE_SEGMENT_SUCCESS:
                    log_info(logger_planificador_extra, "CREATE_SEGMENT realizado con exito");
                    
                    t_list* valores_tabla_actualizada = _recibir_paquete(socketMemoria);
                    t_list* tabla_actualizada = armar_tabla_segmentos(valores_tabla_actualizada);
                    list_destroy_and_destroy_elements(proceso_a_ejecutar->tabla_de_segmentos, (void*) destruir_segmento);
                    proceso_a_ejecutar->tabla_archivos_abiertos = list_duplicate(tabla_actualizada);

                    list_destroy(valores_tabla_actualizada);
                break;

                case OUT_OF_MEMORY:
                    log_error(logger_planificador_extra, "CREATE_SEGMENT ERROR OUT_OF_MEMORY");
                    log_info(logger_kernel_util_obligatorio, "Finaliza el proceso < %d > - Motivo: < OUT_OF_MEMORY >", proceso_a_ejecutar->pid);
                    proceso_en_ejecucion = desalojar_proceso_en_exec();
                    terminar_proceso(proceso_en_ejecucion);
                    
                    list_destroy(lista_contexto_cs);
                    liberar_contexto_ejecucion(contexto_cs);
                    return;
                break;  

                case INICIO_COMPACTAR:
                    
                    pthread_mutex_lock(&op_entre_mem_y_fs);
                    
                    enviar_operacion(socketMemoria, INICIO_COMPACTAR);
                    rta_memoria = recibir_operacion(socketMemoria);
                    t_list* valores_de_todos_segmentos = _recibir_paquete();
                    t_dictionary* nuevas_tablas = armar_nuevas_tablas(valores_de_todos_segmentos);
                    actualizar_tablas_segmentos(nuevas_tablas);

                    list_destroy(valores_de_todos_segmentos);

                    goto crear_segmento;

                break;

                default:
                    log_warning(logger_kernel_util_extra, "Operacion de memoria al crear segmento desconocida");
                break;
            }*/

            list_destroy(lista_contexto_cs);
            liberar_contexto_ejecucion(contexto_cs);

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;

        break;

        case DELETE_SEGMENT:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            int nro_segmento_eliminar = *(int*) list_get(lista_recepcion_valores, 0);

            int op_delete_seg = recibir_operacion(socketCPU);
            t_list* lista_contexto_ds = _recibir_paquete(socketCPU);
            pcb* contexto_ds = recibir_contexto_ejecucion(lista_contexto_ds);

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_ds);

            log_info(logger_kernel_util_obligatorio, "PID: < %d > - Eliminar Segmento - Id: < %d >", proceso_a_ejecutar->pid, nro_segmento_eliminar);

            log_warning(logger_kernel_util_extra, "DELETE_SEGMENT TODAVIA NO IMPLMENETADO");

            // soliciar_eliminacion_segmento(nro_segmento_eliminar, proceso_a_ejecutar->pid);
            /*
            int rta_eliminacion_memoria = recibir_operacion(socketMemoria);

            log_info(logger_planificador_extra, "DELETE_SEGMENT realizado con exito");
                    
            t_list* valores_tras_eliminacion = _recibir_paquete(socketMemoria);
            t_list* tabla_tras_eliminacion = armar_tabla_segmentos(valores_tras_eliminacion);
            list_destroy_and_destroy_elements(proceso_a_ejecutar->tabla_de_segmentos, (void*) destruir_segmento);
            proceso_a_ejecutar->tabla_archivos_abiertos = list_duplicate(tabla_tras_eliminacion);
            */

            //list_destroy(valores_tras_eliminacion);
            list_destroy(lista_recepcion_valores);
            liberar_contexto_ejecucion(contexto_ds);

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);

            goto recibirOperacion;

        break;

        case DESCONEXION:
            log_info(logger_planificador_obligatorio, "CPU Desconectado");
        break;
        
        default:
            log_warning(logger_planificador_obligatorio, "Operación desconocida.");
        break;
    }
}

void* esperar_io(pcb* un_pcb)
{
    log_info(logger_planificador_obligatorio, "PID: < %d > Ejecuta IO < %d >.", un_pcb->pid, un_pcb->tiempo_io);
    sleep(un_pcb->tiempo_io);
    log_info(logger_planificador_extra, "Tiempo de io del proceso < %d > esperado correctamente", un_pcb->pid);
    agregar_proceso_ready(un_pcb);
}

void terminar_proceso(pcb* un_pcb)
{
    char* lugar_en_diccionario = string_from_format("%d", un_pcb->pid);
    int socket_a_consola = dictionary_get(diccionario_de_consolas, lugar_en_diccionario);
    t_paquete* paquete_a_consola = crear_paquete_operacion(EXIT);
    enviar_paquete(paquete_a_consola, socket_a_consola);  
    eliminar_paquete(paquete_a_consola); 
    agregar_proceso_terminated(un_pcb);
    liberar_recursos(un_pcb);
    //liberar_pcb(un_pcb);
}


void wait_recurso(pcb* un_pcb, char* un_recurso)
{
    recurso* recurso = dictionary_get(diccionario_recursos, un_recurso);
    if(recurso == NULL)
    {
        //avisar_memoria(FIN_PROCESO);
        resultado_recurso = false;
        return;
    } 

    recurso->instancias--;
    log_info(logger_kernel_util_obligatorio, "PID: < %d > - Wait: < %s > - Instancias < %d >", un_pcb->pid, recurso->nombre, recurso->instancias);
    list_add(un_pcb->recursos_asignados, recurso);

    if(recurso->instancias < 0)
    {
        pcb* proceso_en_ejecucion = desalojar_proceso_en_exec();
        proceso_en_ejecucion->estado = BLOCKED;
        proceso_bloqueado_por_wait = true;

        if(!es_fifo)
        {
            proceso_en_ejecucion->estimado_prox_rafaga = estimar_proxima_rafaga(proceso_en_ejecucion);
            temporal_destroy(proceso_en_ejecucion->tiempo_ejecucion);
        }
        
        pthread_mutex_lock(&(recurso->mutex_cola));
        
        queue_push(recurso->cola_bloqueados, proceso_en_ejecucion);

        pthread_mutex_unlock(&(recurso->mutex_cola));

        log_info(logger_kernel_util_obligatorio, "PID: < %d > - Bloqueado por: < %s >", proceso_en_ejecucion->pid, recurso->nombre);
        
    }

    return;
}

void signal_recurso(pcb* un_pcb, char* un_recurso)
{
    recurso* recurso = dictionary_get(diccionario_recursos, un_recurso);
    if(recurso == NULL)
    {
        //avisar_memoria(FIN_PROCESO);
        resultado_recurso = false;
        return;
    } 

    recurso->instancias++;
    log_info(logger_kernel_util_obligatorio, "PID: < %d > - Signal: < %s > - Instancias < %d >", un_pcb->pid, recurso->nombre, recurso->instancias);
    remover_recurso_si_esta(un_pcb->recursos_asignados, recurso);

    if(!queue_is_empty(recurso->cola_bloqueados))
    {
        pthread_mutex_lock(&(recurso->mutex_cola));

        pcb* proceso_bloqueado = queue_pop(recurso->cola_bloqueados);
        log_info(logger_kernel_util_obligatorio, "PID: < %d > - Desbloqueado por: < %s >", proceso_bloqueado->pid, recurso->nombre);

        pthread_mutex_unlock(&(recurso->mutex_cola));

        agregar_proceso_ready(proceso_bloqueado);
    }

    return;
}

void liberar_recursos(pcb* un_pcb)
{
    while(!list_is_empty(un_pcb->recursos_asignados))
    {
        int i = 0;
        recurso* un_rec = list_remove(un_pcb->recursos_asignados, i);
        un_rec->instancias++;
        log_info(logger_kernel_util_extra, "Se libera instancia del recurso: %s. Instancias: %d", un_rec->nombre, un_rec->instancias);
        
        if(!queue_is_empty(un_rec->cola_bloqueados))
        {
            pthread_mutex_lock(&(un_rec->mutex_cola));
            pcb* pcb_liberado = queue_pop(un_rec->cola_bloqueados);
            log_info(logger_kernel_util_obligatorio, "PID: < %d > - Desbloqueado por: < %s >", pcb_liberado->pid, un_rec->nombre);

            pthread_mutex_unlock(&(un_rec->mutex_cola));
            agregar_proceso_ready(pcb_liberado);
        }
        i++;
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
    un_pcb->estado = READY;
    if(es_fifo)
    {
        pthread_mutex_lock(&mutex_ready);

        queue_push(cola_ready_fifo, un_pcb);
        log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a READY", un_pcb->pid);
        log_info(logger_planificador_obligatorio, "Cola READY < %s >:", configuracionKernel.ALGORITMO_PLANIFICACION);
        loguear_procesos_en_cola(cola_ready_fifo);

        pthread_mutex_unlock(&mutex_ready);
        sem_post(&activar_corto_plazo);
    }else
    {
        pthread_mutex_lock(&mutex_ready);

        list_add(lista_ready_hrrn, un_pcb);
        un_pcb->llegada_ready = temporal_create();
        log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a READY", un_pcb->pid);
        log_info(logger_planificador_obligatorio, "Cola READY < %s >:", configuracionKernel.ALGORITMO_PLANIFICACION);
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
    log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a EXEC", un_pcb->pid);

    pthread_mutex_unlock(&mutex_exec);
}

void agregar_proceso_terminated(pcb* un_pcb)
{
    pthread_mutex_lock(&mutex_terminated);


    un_pcb->estado = TERMINATED;
    queue_push(cola_terminated, un_pcb);
    log_info(logger_planificador_obligatorio, "El proceso < %d > se movio a TERMINATED", un_pcb->pid);

    pthread_mutex_unlock(&mutex_terminated);
    sem_post(&grado_multiprogramacion);
}

void agregar_proceso_block(pcb* un_pcb)
{
    // TO-DO: Implementar el bloqueo de procesos.
}

pcb* obtener_proceso_new()
{
    pcb* proceso_new;

    pthread_mutex_lock(&mutex_new);

    proceso_new = queue_pop(cola_new);
    log_info(logger_planificador_obligatorio, "El proceso: < %d > salio de NEW", proceso_new->pid);

    pthread_mutex_unlock(&mutex_new);

    return proceso_new;
}

pcb* obtener_proceso_ready()
{
    pcb* proceso_ready;

    pthread_mutex_lock(&mutex_ready);

    proceso_ready = queue_pop(cola_ready_fifo);
    log_info(logger_planificador_obligatorio, "El proceso: < %d > salio de READY", proceso_ready->pid);

    pthread_mutex_unlock(&mutex_ready);

    return proceso_ready;
}


pcb* obtener_proceso_block()
{
    // TO-DO: Implementar el bloqueo de procesos.
}

pcb* desalojar_proceso_en_exec()
{
    pthread_mutex_lock(&mutex_exec);

    pcb* proceso_desalojado = queue_pop(cola_exec);
    log_info(logger_planificador_obligatorio, "El proceso: < %d > salio de EXEC", proceso_desalojado->pid);

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
        log_info(logger_planificador_obligatorio, "Proceso: < %d >", un_proceso->pid);
    }
}

void loguear_procesos_en_lista(t_list* lista_de_procesos)
{
    for(int i = 0; i < list_size(lista_de_procesos); i++)
    {
        pcb* un_proceso = list_get(lista_de_procesos,i);
        log_info(logger_planificador_obligatorio, "Proceso: < %d >", un_proceso->pid);
    }
}

void startSigHandlers(void) {
	signal(SIGINT, sigHandler_sigint);
}

void sigHandler_sigint(int signo) {
	log_warning(logger_kernel_util_extra, "Tiraste un CTRL+C, macho, abortaste el proceso.");
	
	destruir_estructuras_planificacion();
    close(socketCPU);
    //close(socketFS);
    //close(socketMemoria);
	printf("-------------------FINAL POR CTRL+C-------------------");

	exit(-1);
}
#include <planificador.h>

// variable global para el process id
int pid_global = 0;

// Hilos de planificacion.
pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;


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
    dictionary_destroy(tabla_de_procesos);

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

        log_info(logger_planificador_extra, "Esperando tabla de segmentos para el nuevo proceso");
        esperar_tabla_segmentos(proceso);

        leer_segmentos(proceso);
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
        loguear_rr_de_procesos(logger_planificador_extra);
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
    char* nombre_recurso;
    int operacion_de_cpu;
    int tamanio, direccion_fisica;

    recibirOperacion:
    
    operacion_de_cpu = recibir_operacion(socketCPU);
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
            
            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            //list_destroy(lista_recepcion_valores);
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

            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            //list_destroy(lista_recepcion_valores);
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

            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            //list_destroy(lista_recepcion_valores);
            liberar_contexto_ejecucion(contexto_recibido);
        break;

        case WAIT:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            nombre_recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Recurso recibido para realizar WAIT: %s", nombre_recurso);

            int operacion_wait = recibir_operacion(socketCPU);
            t_list* lista_contexto_wait = _recibir_paquete(socketCPU);
            pcb* contexto_de_wait = recibir_contexto_ejecucion(lista_contexto_wait);

            log_info(logger_planificador_extra, "Contexto recibido por WAIT");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_wait);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            wait_recurso(proceso_a_ejecutar, nombre_recurso);


            if(!resultado_recurso)
            {   
                log_warning(logger_planificador_extra, "El recurso NO EXISTE. Terminando proceso");
                proceso_en_ejecucion = desalojar_proceso_en_exec();
                
                log_info(logger_planificador_obligatorio, "Finaliza el proceso < %d > - Motivo: < NO EXISTE RECURSO >", proceso_en_ejecucion->pid);
                
                terminar_proceso(proceso_en_ejecucion);

                resultado_recurso = true;

                liberar_contexto_ejecucion(contexto_de_wait);
                list_destroy_and_destroy_elements(lista_recepcion_valores,free);
                list_destroy_and_destroy_elements(lista_contexto_wait,free);
                //list_destroy(lista_recepcion_valores);
                //list_destroy(lista_contexto_wait);
                return;
            }

            liberar_contexto_ejecucion(contexto_de_wait);
            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            list_destroy_and_destroy_elements(lista_contexto_wait,free);
            //list_destroy(lista_recepcion_valores);
            //list_destroy(lista_contexto_wait);
            
            if(proceso_bloqueado_por_recurso)
            {
                proceso_bloqueado_por_recurso = false;
                return;
            }

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;
        
        break;

        case SIGNAL:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            nombre_recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Recurso recibido para realizar SIGNAL: %s", nombre_recurso);
            
            int operacion_signal = recibir_operacion(socketCPU);
            t_list* lista_contexto_signal = _recibir_paquete(socketCPU);
            pcb* contexto_de_signal = recibir_contexto_ejecucion(lista_contexto_signal);

            log_info(logger_planificador_extra, "Contexto recibido por SIGNAL");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_signal);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            signal_recurso(proceso_a_ejecutar, nombre_recurso);

            if(!resultado_recurso)
            {   
                log_warning(logger_planificador_extra, "El recurso NO EXISTE. Terminando proceso");
                proceso_en_ejecucion = desalojar_proceso_en_exec();
                
                log_info(logger_planificador_obligatorio, "Finaliza el proceso < %d > - Motivo: < NO EXISTE RECURSO >", proceso_en_ejecucion->pid);
                
                terminar_proceso(proceso_en_ejecucion);
                
                resultado_recurso = true;

                liberar_contexto_ejecucion(contexto_de_signal);
                list_destroy_and_destroy_elements(lista_recepcion_valores,free);
                list_destroy_and_destroy_elements(lista_contexto_signal,free);
                //list_destroy(lista_recepcion_valores);
                //list_destroy(lista_contexto_signal);
                
                return;
            }

            liberar_contexto_ejecucion(contexto_de_signal);
            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            list_destroy_and_destroy_elements(lista_contexto_signal,free);
            //list_destroy(lista_recepcion_valores);
            //list_destroy(lista_contexto_signal);
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

            log_trace(logger_kernel_util_obligatorio, "PID: < %d > - Crear Segmento - Id: < %d > - Tamaño:< %d >", proceso_a_ejecutar->pid, nro_segmento_crear, tam_segmento);
            
            //log_warning(logger_kernel_util_extra, "CREATE_SEGMENT TODAVIA NO IMPLMENETADO");
            
            crear_segmento:
            solicitar_creacion_segmento(nro_segmento_crear, tam_segmento, proceso_a_ejecutar->pid);

            //int rta_crecion_memoria;
            //recv(socketMemoria, &rta_crecion_memoria, sizeof(int), MSG_WAITALL);
            
            int rta_crecion_memoria = recibir_operacion(socketMemoria);
            log_error(logger_kernel_util_extra, "lo recibido feu: %d", rta_crecion_memoria);
            switch(rta_crecion_memoria)
            {
                case CREATE_SEGMENT:
                    log_info(logger_planificador_extra, "CREATE_SEGMENT realizado con exito");
                    
                    t_list* valor_de_base = _recibir_paquete(socketMemoria);
                    int base = *(int*) list_get(valor_de_base, 0);
                    t_segmento* segmento_a_actualizar = list_get(proceso_a_ejecutar->tabla_de_segmentos, nro_segmento_crear);
                    segmento_a_actualizar->base = base;
                    segmento_a_actualizar->size = tam_segmento;

                    printf("El segmento: %d quedo con la base en: %d\n", nro_segmento_crear, segmento_a_actualizar->base);

                    leer_segmentos(proceso_a_ejecutar);

                    list_destroy_and_destroy_elements(valor_de_base, free);
                break;

                case OUT_OF_MEMORY:
                    log_error(logger_planificador_extra, "CREATE_SEGMENT ERROR OUT_OF_MEMORY");
                    log_info(logger_kernel_util_obligatorio, "Finaliza el proceso < %d > - Motivo: < OUT_OF_MEMORY >", proceso_a_ejecutar->pid);
                    proceso_en_ejecucion = desalojar_proceso_en_exec();
                    terminar_proceso(proceso_en_ejecucion);
                    
                    list_destroy_and_destroy_elements(lista_contexto_cs,free);
                    liberar_contexto_ejecucion(contexto_cs);
                    return;
                break;  

                case INICIO_COMPACTAR:
                    
                    enviar_operacion(socketMemoria, INICIO_COMPACTAR);
                    int rta_memoria = recibir_operacion(socketMemoria);
                    t_list* valores_de_todos_segmentos = _recibir_paquete(socketMemoria);
                    actualizar_tablas_segmentos(valores_de_todos_segmentos);

                    list_destroy(valores_de_todos_segmentos);

                    goto crear_segmento;

                break;

                default:
                    log_warning(logger_kernel_util_extra, "Operacion de memoria al crear segmento desconocida");
                break;
            }

            list_destroy_and_destroy_elements(lista_contexto_cs, free);
            //list_destroy(lista_contexto_cs);
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

            //log_warning(logger_kernel_util_extra, "DELETE_SEGMENT TODAVIA NO IMPLMENETADO");

            solicitar_eliminacion_segmento(nro_segmento_eliminar, proceso_a_ejecutar->pid);
            
            log_info(logger_kernel_util_extra, "ESPERANDO TABLA ACTUALIZADA");

            int rta_eliminacion_memoria = recibir_operacion(socketMemoria);

            log_info(logger_planificador_extra, "DELETE_SEGMENT realizado con exito");
                    
            t_list* valores_tras_eliminacion = _recibir_paquete(socketMemoria);
            t_list* tabla_tras_eliminacion = deserializar_tabla_segmentos(valores_tras_eliminacion);
            list_destroy_and_destroy_elements(proceso_a_ejecutar->tabla_de_segmentos,free);
            proceso_a_ejecutar->tabla_de_segmentos = list_duplicate(tabla_tras_eliminacion);

            leer_segmentos(proceso_a_ejecutar);
            

            list_destroy_and_destroy_elements(valores_tras_eliminacion,free);
            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            //list_destroy(lista_recepcion_valores);
            liberar_contexto_ejecucion(contexto_ds);

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);

            goto recibirOperacion;

        break;

        case F_OPEN: 
            // RECIBIR PARAMETRO(S) DE CPU
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            nombre_recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Nombre de archivo para realizar F_OPEN: %s", nombre_recurso);

            int operacion_fopen = recibir_operacion(socketCPU);
            t_list* lista_contexto_fopen = _recibir_paquete(socketCPU);
            pcb* contexto_de_fopen = recibir_contexto_ejecucion(lista_contexto_fopen);

            log_info(logger_planificador_extra, "Contexto recibido por F_OPEN");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_fopen);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            // BUSCAR EN TABLA DE ARCHIVOS ABIERTOS
            if (!dictionary_has_key(tabla_global_archivos_abiertos, nombre_recurso))
            {
                // PEDIR AL FS
                // archivo = obtener_archivo_de_fs();
                t_paquete* paquete_a_fs = crear_paquete_operacion(CREAR_ARCHIVO);
                agregar_a_paquete(paquete_a_fs, nombre_recurso, strlen(nombre_recurso)+1);
                enviar_paquete(paquete_a_fs, socketFS);
                
                int rta1;
                int rta2;
                recv(socketFS, &rta1, sizeof(int), MSG_WAITALL );
                recv(socketFS, &rta2, sizeof(int), MSG_WAITALL );
                

                // SI NO EXISTE, CREARLO
                t_recurso* archivo = crear_recurso(nombre_recurso, 1);
                archivo->posicion = 0;

                dictionary_put(tabla_global_archivos_abiertos, nombre_recurso, archivo);
            }

            fopen_recurso(proceso_a_ejecutar, nombre_recurso);

            liberar_contexto_ejecucion(contexto_de_fopen);
            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            list_destroy_and_destroy_elements(lista_contexto_fopen,free);
            //list_destroy(lista_recepcion_valores);
            //list_destroy(lista_contexto_fopen);
            
            if(proceso_bloqueado_por_recurso)
            {
                proceso_bloqueado_por_recurso = false;
                return;
            }

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;

        break;

        case F_CLOSE: 
            // RECIBIR PARAMETRO(S) DE CPU
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            nombre_recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Nombre de archivo para realizar F_CLOSE: %s", nombre_recurso);

            int operacion_fclose = recibir_operacion(socketCPU);
            t_list* lista_contexto_fclose = _recibir_paquete(socketCPU);
            pcb* contexto_de_fclose = recibir_contexto_ejecucion(lista_contexto_fclose);

            log_info(logger_planificador_extra, "Contexto recibido por F_CLOSE");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_fclose);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            fclose_recurso(proceso_a_ejecutar, nombre_recurso);

            
            if(!archivo_esta_abierto(nombre_recurso))
            {
                // REMOVER DE LA TABLA DE ARCHIVOS ABIERTOS
                t_recurso* archivo = dictionary_remove(tabla_global_archivos_abiertos, nombre_recurso);
                free(archivo);
            }

            liberar_contexto_ejecucion(contexto_de_fclose);
            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            list_destroy_and_destroy_elements(lista_contexto_fclose,free);
            //list_destroy(lista_recepcion_valores);
            //list_destroy(lista_contexto_fclose);
            
            if(proceso_bloqueado_por_recurso)
            {
                proceso_bloqueado_por_recurso = false;
                return;
            }

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;

        break;

        case F_SEEK: 
            // RECIBIR PARAMETRO(S) DE CPU
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            nombre_recurso = list_get(lista_recepcion_valores, 0);
            int posicion = *(int*) list_get(lista_recepcion_valores, 1);
            log_info(logger_planificador_extra,"Nombre de archivo para realizar F_SEEK: %s, posicion: %d", nombre_recurso, posicion);

            int operacion_fseek = recibir_operacion(socketCPU);
            t_list* lista_contexto_fseek = _recibir_paquete(socketCPU);
            pcb* contexto_de_fseek = recibir_contexto_ejecucion(lista_contexto_fseek);

            log_info(logger_planificador_extra, "Contexto recibido por F_SEEK");

            actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_fseek);

            loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

            fseek_archivo(proceso_a_ejecutar, nombre_recurso, posicion);

            liberar_contexto_ejecucion(contexto_de_fseek);
            list_destroy_and_destroy_elements(lista_recepcion_valores, free);
            list_destroy_and_destroy_elements(lista_contexto_fseek, free);
            //list_destroy(lista_recepcion_valores);
            //list_destroy(lista_contexto_fseek);

            enviar_contexto_ejecucion(proceso_a_ejecutar, socketCPU, CONTEXTO_EJECUCION);
            
            goto recibirOperacion;

        break;

            case F_READ:
                lista_recepcion_valores = _recibir_paquete(socketCPU);
                nombre_recurso = list_get(lista_recepcion_valores, 0);
                direccion_fisica = *(int*) list_get(lista_recepcion_valores, 1);
                tamanio = *(int*) list_get(lista_recepcion_valores, 2);
                log_info(logger_planificador_extra,"Nombre de archivo para realizar F_READ: %s, dir: %d, tamanio: %d", nombre_recurso, direccion_fisica, tamanio);

                int operacion_fread = recibir_operacion(socketCPU);
                t_list* lista_contexto_fread = _recibir_paquete(socketCPU);
                pcb* contexto_de_fread = recibir_contexto_ejecucion(lista_contexto_fread);

                log_info(logger_planificador_extra, "Contexto recibido por F_READ");

                actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_fread);

                loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

                t_paquete* paquete_fread = crear_paquete_operacion(LEER_ARCHIVO);
                agregar_a_paquete(paquete_fread, nombre_recurso, strlen(nombre_recurso)+1);
                agregar_a_paquete(paquete_fread, &direccion_fisica, sizeof(int));
                agregar_a_paquete(paquete_fread, &tamanio, sizeof(int));
                
                pthread_mutex_lock(&mutex_fs);  // Se bloquea al hilo antes de enviar el paquete (realizar la solicitud)

                enviar_paquete(paquete_fread, socketFS);

                pthread_t* hilo_fread;
                pthread_create(&hilo_fread, NULL, esperar_listo_de_fs, (void*) nombre_recurso);
                pthread_detach(hilo_fread);

                return;

            break;

            case F_WRITE:
                lista_recepcion_valores = _recibir_paquete(socketCPU);
                nombre_recurso = list_get(lista_recepcion_valores, 0);
                direccion_fisica = *(int*) list_get(lista_recepcion_valores, 1);
                tamanio = *(int*) list_get(lista_recepcion_valores, 2);
                log_info(logger_planificador_extra,"Nombre de archivo para realizar F_WRITE: %s, dir: %d, tamanio: %d", nombre_recurso, direccion_fisica, tamanio);

                int operacion_fwrite = recibir_operacion(socketCPU);
                t_list* lista_contexto_fwrite = _recibir_paquete(socketCPU);
                pcb* contexto_de_fwrite = recibir_contexto_ejecucion(lista_contexto_fwrite);

                log_info(logger_planificador_extra, "Contexto recibido por F_READ");

                actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_fwrite);

                loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

                t_paquete* paquete_fwrite = crear_paquete_operacion(ESCRIBIR_ARCHIVO);
                agregar_a_paquete(paquete_fwrite, nombre_recurso, strlen(nombre_recurso)+1);
                agregar_a_paquete(paquete_fwrite, &direccion_fisica, sizeof(int));
                agregar_a_paquete(paquete_fwrite, &tamanio, sizeof(int));
                
                pthread_mutex_lock(&mutex_fs);  // Se bloquea al hilo antes de enviar el paquete (realizar la solicitud)
                
                enviar_paquete(paquete_fwrite, socketFS);

                pthread_t* hilo_fwrite;
                pthread_create(&hilo_fwrite, NULL, esperar_listo_de_fs, (void*) nombre_recurso);
                pthread_detach(hilo_fwrite);

                return;

            break;

            case F_TRUNCATE:
                lista_recepcion_valores = _recibir_paquete(socketCPU);
                nombre_recurso = list_get(lista_recepcion_valores, 0);
                tamanio = *(int*) list_get(lista_recepcion_valores, 1);
                log_info(logger_planificador_extra,"Nombre de archivo para realizar F_TRUNCATE: %s, tamanio: %d", nombre_recurso, tamanio);

                int operacion_ftruncate = recibir_operacion(socketCPU);
                t_list* lista_contexto_truncate = _recibir_paquete(socketCPU);
                pcb* contexto_de_ftruncate = recibir_contexto_ejecucion(lista_contexto_truncate);

                log_info(logger_planificador_extra, "Contexto recibido por F_TRUNCATE");

                actualizar_contexto_ejecucion(proceso_a_ejecutar, contexto_de_ftruncate);

                loguear_pcb(proceso_a_ejecutar, logger_kernel_util_extra);

                t_paquete* paquete_ftruncate = crear_paquete_operacion(TRUNCAR_ARCHIVO);
                agregar_a_paquete(paquete_ftruncate, nombre_recurso, strlen(nombre_recurso)+1);
                agregar_a_paquete(paquete_ftruncate, &tamanio, sizeof(int));
                
                pthread_mutex_lock(&mutex_fs);  // Se bloquea al hilo antes de enviar el paquete (realizar la solicitud)
                
                enviar_paquete(paquete_ftruncate, socketFS);

                pthread_t* hilo_ftruncate;
                pthread_create(&hilo_ftruncate, NULL, esperar_listo_de_fs, (void*) nombre_recurso);
                pthread_detach(hilo_ftruncate);

                return;

            case SEG_FAULT:
                lista_recepcion_valores = _recibir_paquete(socketCPU);
                contexto_recibido = recibir_contexto_ejecucion(lista_recepcion_valores);
                log_warning(logger_planificador_extra, "Contexto del proceso: < %d > recibido por SEG_FAULT.", proceso_a_ejecutar->pid);
                proceso_en_ejecucion = desalojar_proceso_en_exec();

                log_warning(logger_planificador_extra, "Terminando proceso: < %d > por  SEG_FAULT.", proceso_a_ejecutar->pid);

                actualizar_contexto_ejecucion(proceso_en_ejecucion, contexto_recibido);
                loguear_pcb(proceso_en_ejecucion, logger_planificador_extra);

                terminar_proceso(proceso_en_ejecucion);

                list_destroy_and_destroy_elements(lista_recepcion_valores,free);
                liberar_contexto_ejecucion(contexto_recibido);
            
            return;

            case DESCONEXION:
                log_info(logger_planificador_obligatorio, "CPU Desconectado");
            break;
        
        default:
            log_warning(logger_planificador_obligatorio, "Operación desconocida.");
        break;
    }
}






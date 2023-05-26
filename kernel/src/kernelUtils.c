#include "kernelUtils.h"
#include "kernelArchivos.h"

extern t_dictionary* tabla_global_archivos_abiertos;

t_log* logger_kernel_util_obligatorio;
t_log* logger_kernel_util_extra;

// DICCIONARIO DE CONSOLAS: UNA MANERA DE GUARDAR SOCKETS DE LAS CONSOLAS, PARA LOS PIDS CREADOS.
t_dictionary* diccionario_de_consolas;

// Los sockets que usaremos en todo el modulo kernel, para las diferentes conexiones salvo las consolas.
int socketMemoria;
int socketCPU;
int socketFS;

bool resultado_recurso = true;
bool proceso_bloqueado_por_recurso = false;


/*****************************************************************************
 *              FUNCIONES UTILES PARA ADMINISTRATIVAS
******************************************************************************/

void agregar_socket_a_diccionario(int pcb_pid, int socket_consola)
{
    //int *p = malloc(sizeof(int));
	//*p=socket_consola;
    char* key = string_from_format("%d", pcb_pid);
	dictionary_put(diccionario_de_consolas, key, socket_consola);
    free(key);
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

void remover_recurso_si_esta(t_list* lista,t_recurso* un_recurso)
{
    for(int i = 0; i < list_size(lista); i++)
    {
        t_recurso* un_rec = list_get(lista, i);
        if(un_recurso->nombre == un_rec->nombre)
        {
            list_remove(lista, i);
        }
    }
}

void liberar_recursos(pcb* un_pcb)
{
    while(!list_is_empty(un_pcb->recursos_asignados))
    {
        int i = 0;
        t_recurso* un_rec = list_remove(un_pcb->recursos_asignados, i);
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


/*****************************************************************************
 *              FUNCIONES UTILES PARA HRRN
******************************************************************************/

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

/*****************************************************************************
 *              FUNCIONES UTILES PARA COMUNICACION
******************************************************************************/

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

void actualizar_tablas_segmentos(t_list* lista_de_valores)
{
    int cantidad_de_procesos = *(int*) list_get(lista_de_valores, 0);

    int base=  0;
    for(int i = 0; i < cantidad_de_procesos; i++)
    {
        int cantidad_de_segmentos = *(int*) list_get(lista_de_valores, base + 1);
        
        char* key = string_from_format("%d", i+1);
        pcb* proceso = dictionary_get(tabla_de_procesos, key);
        free(key);
        t_list* tabla_a_actualizar = proceso->tabla_de_segmentos;

        for(int j = 0; j < cantidad_de_segmentos; j++)
        {
            int base = *(int*) list_get(lista_de_valores, base + 2);
            int tamanio = *(int*) list_get(lista_de_valores, base + 3);

            t_segmento* segmento_a_actualizar = list_get(tabla_a_actualizar, j);
            segmento_a_actualizar->base = base;
            segmento_a_actualizar->size = tamanio;
            
            base += 2; 
        }
        base++;
    }
}

void* esperar_tabla_segmentos(pcb* un_pcb)
{
	t_paquete* paquete_incializacion = crear_paquete_operacion(INICIO_PROCESO);
	int p_id = un_pcb->pid;
	agregar_a_paquete(paquete_incializacion, &p_id, sizeof(int));
	enviar_paquete(paquete_incializacion, socketMemoria);
	eliminar_paquete(paquete_incializacion);

	t_list* segmentos_recibidos;

	switch(recibir_operacion(socketMemoria))
	{
		case INICIO_PROCESO:
			segmentos_recibidos = _recibir_paquete(socketMemoria);
			t_list* tabla_de_segmentos = deserializar_tabla_segmentos(segmentos_recibidos);
			un_pcb->tabla_de_segmentos = list_duplicate(tabla_de_segmentos);
			list_destroy_and_destroy_elements(segmentos_recibidos, free);
		break;

		default:
			log_warning(logger_kernel_util_extra, "Operacion de memoria esperando tabla de segmentos desconocida");
			return;
		break;
	}
}

//void serializar_tablas_de_segmentos(t_paquete* paquete,t_list* lista_general_de_segmentos)
//{
//    int cantidad_de_procesos = list_size(lista_general_de_segmentos);
//    agregar_a_paquete(paquete, &cantidad_de_procesos, sizeof(int));
//
//    for(int i = 0; i < cantidad_de_procesos; i++)
//    {
//        t_list* tabla_del_proceso = list_get(lista_general_de_segmentos, i);
//        int cantidad_de_segmentos = list_size(tabla_del_proceso);
//        agregar_a_paquete(paquete, &cantidad_de_segmentos, sizeof(int));
//        
//        for(int j = 0; j < cantidad_de_procesos; j++)
//        {
//            t_segmento* segmento = list_Get(tabla_del_proceso, j);
//            agregar_a_paquete(paquete, &(segmento->base), sizeof(int));
//            agregar_a_paquete(paquete, &(segmento->size), sizeof(int));
//        }
//    }
//}

/*****************************************************************************
 *              FUNCIONES PARA EJECUCION
******************************************************************************/



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
    free(lugar_en_diccionario);
    t_paquete* paquete_a_consola = crear_paquete_operacion(EXIT);
    enviar_paquete(paquete_a_consola, socket_a_consola);  
    
//    t_paquete* paquete_a_memoria = crear_paquete_operacion(FIN_PROCESO);
//    int p_id = un_pcb->pid;
//    agregar_a_paquete(paquete_a_memoria, &p_id, sizeof(int));
//    enviar_paquete(paquete_a_memoria, socketMemoria);
    
//    eliminar_paquete(paquete_a_memoria);
    eliminar_paquete(paquete_a_consola); 
    agregar_proceso_terminated(un_pcb);
    liberar_recursos(un_pcb);
    //liberar_pcb(un_pcb);
}

void wait_recurso(pcb* un_pcb, char* un_recurso) {
    wait_recurso_generico(un_pcb, un_recurso, diccionario_recursos, "WAIT");
}

void fopen_recurso(pcb* un_pcb, char* un_recurso) {
    wait_recurso_generico(un_pcb, un_recurso, tabla_global_archivos_abiertos, "F_OPEN");
}

void wait_recurso_generico(pcb* un_pcb, char* un_recurso, t_dictionary* dictionary, char* operacion){
   t_recurso* recurso = dictionary_get(dictionary, un_recurso);
    if(recurso == NULL)
    {
        //avisar_memoria(FIN_PROCESO);
        resultado_recurso = false;
        return;
    } 

    recurso->instancias--;
    log_info(logger_kernel_util_obligatorio, "PID: < %d > - %s: < %s > - Instancias < %d >", un_pcb->pid, operacion, recurso->nombre, recurso->instancias);
    list_add(un_pcb->recursos_asignados, recurso);
    
    if(recurso->instancias < 0)
    {
        pcb* proceso_en_ejecucion = desalojar_proceso_en_exec();
        proceso_en_ejecucion->estado = BLOCKED;
        proceso_bloqueado_por_recurso = true;

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


void signal_recurso(pcb* un_pcb, char* un_recurso) {
    signal_recurso_generico(un_pcb, un_recurso, diccionario_recursos, "SIGNAL");
}

void fclose_recurso(pcb* un_pcb, char* un_recurso) {
    signal_recurso_generico(un_pcb, un_recurso, tabla_global_archivos_abiertos, "F_CLOSE");
}

void signal_recurso_generico(pcb* un_pcb, char* un_recurso, t_dictionary* dictionary, char* operacion){

    t_recurso* recurso = dictionary_get(dictionary, un_recurso);
    if(recurso == NULL)
    {
        //avisar_memoria(FIN_PROCESO);
        resultado_recurso = false;
        return;
    } 

    recurso->instancias++;
    log_info(logger_kernel_util_obligatorio, "PID: < %d > - %s: < %s > - Instancias < %d >", un_pcb->pid, operacion, recurso->nombre, recurso->instancias);
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


bool archivo_esta_abierto(char* archivo)
{
    // contador de procesos que tienen el archivo abierto
    int contador = 0;

    // devuelve true si el nombre del recurso matchea con el del archivo
    void _recurso_es_el_archivo(t_recurso* recurso) {
        return string_equals_ignore_case(recurso->nombre, archivo);
    }

    // suma 1 a contador si el pcb tiene el archivo abierto
    void _pcb_tiene_el_archivo(char* ___, pcb* iteration) {
        int recursos_asignados = list_size(iteration->recursos_asignados);

        bool found = list_any_satisfy(iteration->recursos_asignados, (void*) _recurso_es_el_archivo);
        if(found)
            contador++;   
    }

    dictionary_iterator(tabla_de_procesos, (void*) _pcb_tiene_el_archivo);

    return contador != 0;
}



void fseek_archivo(pcb* un_pcb, char* un_recurso, int posicion) {
     t_recurso* archivo = dictionary_get(tabla_global_archivos_abiertos, un_recurso);
    if(archivo == NULL)
    {
        resultado_recurso = false;
        return;
    }

    archivo->posicion = posicion;
}

void esperar_listo_de_fs()
{
    pcb* proceso_desalojado = desalojar_proceso_en_exec();
    if(!es_fifo)
    {
        proceso_desalojado->estimado_prox_rafaga = estimar_proxima_rafaga(proceso_desalojado);
        temporal_destroy(proceso_desalojado->tiempo_ejecucion);
    }

    int rtafs;
    
    /*
    *   ESTO PUEDE GENERAR QUILOMBO
    */
    recv(socketFS, &rtafs, sizeof(int), MSG_WAITALL);
    
    agregar_proceso_ready(proceso_desalojado);
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
    t_paquete* paquete_ds = crear_paquete_operacion(DELETE_SEGMENT);
    agregar_a_paquete(paquete_ds, &nro_segmento, sizeof(int));
    agregar_a_paquete(paquete_ds, &pid_proceso, sizeof(int));
    enviar_paquete(paquete_ds, socketMemoria);
    eliminar_paquete(paquete_ds);
}


void leer_segmentos(pcb* un_pcb)
{
    for(int i = 0; i < list_size(un_pcb->tabla_de_segmentos); i++)
    {
        t_segmento* segmento = list_get(un_pcb->tabla_de_segmentos, i);
        printf("El segmento < %d > tiene un tamaño de: < %d > y su base es < %d >\n", i, segmento->size, segmento->base);
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
#include <main.h>

int main(int argc, char *argv[]) {

    //startSigHandlers();

    Config* kernelConfig = config_create(argv[1]);
    configuracionKernel = obtener_valores_de_configuracion_kernel(kernelConfig);
    mostrar_valores_de_configuracion_kernel(configuracionKernel);
    leer_diccionario_recursos();

    socketMemoria = conectar_con_memoria(configuracionKernel);
    socketCPU = conectar_con_cpu(configuracionKernel);
    socketFS = conectar_con_filesystem(configuracionKernel);

    iniciar_tabla_de_procesos();
    inicializar_estructuras_planificacion();
    iniciar_planificadores();
    inicializar_archivos_kernel();

    int socketServicioConsolas = iniciar_servidor_para_consolas(configuracionKernel);

    /*
    LineaInstruccion* linea = malloc(sizeof(LineaInstruccion));
    linea->identificador = "SET";
    linea->parametros[0] = "AX";
    linea->parametros[1] = "AX";
    linea->parametros[2] = "AX";
    printf("Linea: %s %s %s %s", linea->identificador, linea->parametros[0], linea->parametros[1], linea->parametros[2]);
    t_list* lista_instrucciones = list_create();
    list_add(lista_instrucciones, linea);
    pcb* un_pcb = crear_pcb(lista_instrucciones, pid_global, configuracionKernel.ESTIMACION_INICIAL/1000);
    loguear_pcb(un_pcb, loggerKernel);
    list_destroy_and_destroy_elements(lista_instrucciones, (void*)liberar_instruccion);    
    liberar_pcb(un_pcb);
    */
    // PRUEBAS DEL RESPONSE RATIO
    /*
    pcb* p1;
    pcb* p2; 
    pcb* p3;
    p1 = malloc(sizeof(pcb*));
    p2 = malloc(sizeof(pcb*));
    p3 = malloc(sizeof(pcb*));
    p1->pid = 1;
    p2->pid = 2;
    p3->pid = 3;
    p1->llegada_ready = temporal_create();
    sleep(2);
    p2->llegada_ready = temporal_create();
    sleep(1);
    p3->llegada_ready = temporal_create();
    sleep(3);
    p1->estimado_prox_rafaga = 4;
    p2->estimado_prox_rafaga = 7;
    p3->estimado_prox_rafaga = 9;
    printf("El proceso 1 espero: %ld\n", temporal_gettime(p1->llegada_ready)/1000);
    printf("El proceso 2 espero: %ld\n", temporal_gettime(p2->llegada_ready)/1000);
    printf("El proceso 3 espero: %ld\n", temporal_gettime(p3->llegada_ready)/1000);
    printf("El RR de 1 es: %f\n", calcular_response_ratio(p1));
    printf("El RR de 2 es: %f\n", calcular_response_ratio(p2));
    printf("El RR de 3 es: %f\n", calcular_response_ratio(p3));
    printf("el resultado de dividir 3 por 7 es: %f\n", 3.0/7.0);
    double num1 = 3.2;
    double num2 = 5.92;
    double division = num1 / num2;
    printf ("la division da: %f\n", division);

    t_list* lista_procesos = list_create();
    list_add(lista_procesos, p3);
    list_add(lista_procesos, p1);
    list_add(lista_procesos, p2);
    
    for(int i = 0; i < list_size(lista_procesos);i++)
    {
        pcb* proc = list_get(lista_procesos, i);
        printf("En la posicion %d, esta le proceso: %d\n", i, proc->pid);
    }

    list_sort(lista_procesos, comparar_response_ratio);
    printf("Luego de filtrar la lista, los procesos quedaron: \n");
    
    for(int i = 0; i < list_size(lista_procesos);i++)
    {
        pcb* proc = list_get(lista_procesos,i);
        printf("En la posicion %d, esta le proceso: %d\n", i, proc->pid);
    }

    pcb* proceso_con_mayor_rr = list_get(lista_procesos,0);
    printf("El proceso con mayor rr es: %d\n", proceso_con_mayor_rr->pid);
    */

    /*
    LineaInstruccion* instr = malloc(sizeof(LineaInstruccion));
    instr->identificador = "WAIT";
    instr->parametros[0] = "DISCO";
    instr->parametros[1] = "-1";
    instr->parametros[2] = "-1";
    t_list* lista = list_create();
    list_add(lista, instr);
    t_list* lista_procesos = list_create();
    pcb* p1;
    pcb* p2;
    pcb* p3;

    double estimado_inicial = configuracionKernel.ESTIMACION_INICIAL/1000;
    printf("El estimado inicial es: %f" ,estimado_inicial);
    pcb* pA = crear_pcb(lista, 1, estimado_inicial);
    pcb* pB = crear_pcb(lista, 2, estimado_inicial);
    pcb* pC = crear_pcb(lista, 3, estimado_inicial);
    list_add(lista_procesos, pA);
    list_add(lista_procesos, pB);
    list_add(lista_procesos, pC);
    printf("El proceso: %d tiene un estimado inicial de %f\n", pA->pid, pA->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado inicial de %f\n", pB->pid, pB->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado inicial de %f\n", pC->pid, pC->estimado_prox_rafaga);
    printf("\n");
    printf("El proceso: %d esta en ready\n", pA->pid);    
    printf("El proceso: %d esta en ready\n", pB->pid);
    printf("El proceso: %d esta en ready\n", pC->pid);
    pA->llegada_ready = temporal_create();
    pB->llegada_ready = temporal_create();
    pC->llegada_ready = temporal_create();
    printf("\n");

    printf("El proceso 1 se pone a ejecutar 12 segundos");
    temporal_destroy(pA->llegada_ready);
    pA->tiempo_ejecucion = temporal_create();
    sleep(12);
    printf("\n");
    printf("El proceso 1 salio de ejecucion. Su tiempo de ejecucion fue: %ld" ,temporal_gettime(pA->tiempo_ejecucion)/1000);
    double nueva_rafagaA = estimar_proxima_rafaga(pA);
    printf("\n");
    printf("EL nuevo estimado de 1 es: %f", nueva_rafagaA);
    pA->estimado_prox_rafaga = nueva_rafagaA;
    temporal_destroy(pA->tiempo_ejecucion);
    pA->llegada_ready = temporal_create();
    printf("\n");
    printf("El proceso 1 volvio a ready");
    printf("\n");
    printf("El proceso: %d espero: %ld\n", pA->pid, temporal_gettime(pA->llegada_ready)/1000);    
    printf("El proceso: %d espero: %ld\n", pB->pid, temporal_gettime(pB->llegada_ready)/1000);
    printf("El proceso: %d espero: %ld\n", pC->pid, temporal_gettime(pC->llegada_ready)/1000);
    printf("\n");
    printf("El RR de 1 es: %f\n", calcular_response_ratio(pA));
    printf("El RR de 2 es: %f\n", calcular_response_ratio(pB));
    printf("El RR de 3 es: %f\n", calcular_response_ratio(pC));
    printf("\n");
    printf("El proceso: %d tiene un estimado proximo de %f\n", pA->pid, pA->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pB->pid, pB->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pC->pid, pC->estimado_prox_rafaga);
    printf("\n");
    list_sort(lista_procesos, comparar_response_ratio);
    p1 = list_get(lista_procesos,0);
    p2 = list_get(lista_procesos,1);
    p3 = list_get(lista_procesos,2);
    printf("Orden 1 : %d\n", p1->pid);
    printf("Orden 2 : %d\n", p2->pid);
    printf("Orden 3 : %d\n", p3->pid);



    temporal_destroy(pB->llegada_ready);
    pB->tiempo_ejecucion = temporal_create();
    sleep(2);
    printf("\n");
    printf("El proceso 2 salio de ejecucion. Su tiempo de ejecucion fue: %ld" ,temporal_gettime(pB->tiempo_ejecucion)/1000);
    double nueva_rafagaB = estimar_proxima_rafaga(pB);
    printf("\n");
    printf("EL nuevo estimado de 2 es: %f", nueva_rafagaB);
    pB->estimado_prox_rafaga = nueva_rafagaB;
    temporal_destroy(pB->tiempo_ejecucion);
    pB->llegada_ready = temporal_create();
    printf("\n");
    printf("El proceso B volvio a ready");
    printf("\n");
    printf("El proceso: %d espero: %ld\n", pA->pid, temporal_gettime(pA->llegada_ready)/1000);    
    printf("El proceso: %d espero: %ld\n", pB->pid, temporal_gettime(pB->llegada_ready)/1000);
    printf("El proceso: %d espero: %ld\n", pC->pid, temporal_gettime(pC->llegada_ready)/1000);
    printf("\n");
    printf("El RR de 1 es: %f\n", calcular_response_ratio(pA));
    printf("El RR de 2 es: %f\n", calcular_response_ratio(pB));
    printf("El RR de 3 es: %f\n", calcular_response_ratio(pC));
    printf("\n");
    printf("El proceso: %d tiene un estimado proximo de %f\n", pA->pid, pA->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pB->pid, pB->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pC->pid, pC->estimado_prox_rafaga);
    printf("\n");
    list_sort(lista_procesos, comparar_response_ratio);
    p1 = list_get(lista_procesos,0);
    p2 = list_get(lista_procesos,1);
    p3 = list_get(lista_procesos,2);
    printf("Orden 1 : %d\n", p1->pid);
    printf("Orden 2 : %d\n", p2->pid);
    printf("Orden 3 : %d\n", p3->pid);

    printf("El proceso 3 se pone a ejecutar 2 segundos");
    temporal_destroy(pC->llegada_ready);
    pC->tiempo_ejecucion = temporal_create();
    sleep(2);
    printf("\n");
    printf("El proceso 3 salio de ejecucion. Su tiempo de ejecucion fue: %ld" ,temporal_gettime(pC->tiempo_ejecucion)/1000);
    double nueva_rafagaC = estimar_proxima_rafaga(pC);
    printf("\n");
    printf("EL nuevo estimado de 3 es: %f", nueva_rafagaC);
    pC->estimado_prox_rafaga = nueva_rafagaC;
    temporal_destroy(pC->tiempo_ejecucion);
    pC->llegada_ready = temporal_create();
    printf("\n");
    printf("El proceso C volvio a ready");
    printf("\n");
    printf("El proceso: %d espero: %ld\n", pA->pid, temporal_gettime(pA->llegada_ready)/1000);    
    printf("El proceso: %d espero: %ld\n", pB->pid, temporal_gettime(pB->llegada_ready)/1000);
    printf("El proceso: %d espero: %ld\n", pC->pid, temporal_gettime(pC->llegada_ready)/1000);
    printf("\n");
    printf("El RR de 1 es: %f\n", calcular_response_ratio(pA));
    printf("El RR de 2 es: %f\n", calcular_response_ratio(pB));
    printf("El RR de 3 es: %f\n", calcular_response_ratio(pC));
    printf("\n");
    printf("El proceso: %d tiene un estimado proximo de %f\n", pA->pid, pA->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pB->pid, pB->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pC->pid, pC->estimado_prox_rafaga);
    printf("\n");
    list_sort(lista_procesos, comparar_response_ratio);
    p1 = list_get(lista_procesos,0);
    p2 = list_get(lista_procesos,1);
    p3 = list_get(lista_procesos,2);
    printf("Orden 1 : %d\n", p1->pid);
    printf("Orden 2 : %d\n", p2->pid);
    printf("Orden 3 : %d\n", p3->pid);

    printf("El proceso 2 se pone a ejecutar 2 segundos");
    temporal_destroy(pB->llegada_ready);
    pB->tiempo_ejecucion = temporal_create();
    sleep(2);
    printf("\n");
    printf("El proceso 2 salio de ejecucion. Su tiempo de ejecucion fue: %ld" ,temporal_gettime(pC->tiempo_ejecucion)/1000);
    nueva_rafagaB = estimar_proxima_rafaga(pB);
    printf("\n");
    printf("EL nuevo estimado de 2 es: %f", nueva_rafagaB);
    pB->estimado_prox_rafaga = nueva_rafagaB;
    temporal_destroy(pB->tiempo_ejecucion);
    pB->llegada_ready = temporal_create();
    printf("\n");
    printf("El proceso B volvio a ready");
    printf("\n");
    printf("El proceso: %d espero: %ld\n", pA->pid, temporal_gettime(pA->llegada_ready)/1000);    
    printf("El proceso: %d espero: %ld\n", pB->pid, temporal_gettime(pB->llegada_ready)/1000);
    printf("El proceso: %d espero: %ld\n", pC->pid, temporal_gettime(pC->llegada_ready)/1000);
    printf("\n");
    printf("El RR de 1 es: %f\n", calcular_response_ratio(pA));
    printf("El RR de 2 es: %f\n", calcular_response_ratio(pB));
    printf("El RR de 3 es: %f\n", calcular_response_ratio(pC));
    printf("\n");
    printf("El proceso: %d tiene un estimado proximo de %f\n", pA->pid, pA->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pB->pid, pB->estimado_prox_rafaga);
    printf("El proceso: %d tiene un estimado proximo de %f\n", pC->pid, pC->estimado_prox_rafaga);
    printf("\n");
    list_sort(lista_procesos, comparar_response_ratio);
    p1 = list_get(lista_procesos,0);
    p2 = list_get(lista_procesos,1);
    p3 = list_get(lista_procesos,2);
    printf("Orden 1 : %d\n", p1->pid);
    printf("Orden 2 : %d\n", p2->pid);
    printf("Orden 3 : %d\n", p3->pid);
    */
/*
    LineaInstruccion* instr1 = malloc(sizeof(LineaInstruccion));
    instr1->identificador = "WAIT";
    instr1->parametros[0] = "DISCO";
    instr1->parametros[1] = "-1";
    instr1->parametros[2] = "-1";
    LineaInstruccion* instr2 = malloc(sizeof(LineaInstruccion));
    instr2->identificador = "SIGNAL";
    instr2->parametros[0] = "DISCO";
    instr2->parametros[1] = "-1";
    instr2->parametros[2] = "-1";
    LineaInstruccion* instr3 = malloc(sizeof(LineaInstruccion));
    instr3->identificador = "WAIT";
    instr3->parametros[0] = "DISCO";
    instr3->parametros[1] = "-1";
    instr3->parametros[2] = "-1";
    t_list* lista_ej = list_create();
    list_add(lista_ej, instr1);
    list_add(lista_ej, instr2);
    list_add(lista_ej, instr3);
    
    pcb* pcb_ej = crear_pcb(lista_ej, 1, 10);
    loguear_pcb(pcb_ej, logger_kernel_util_extra);

    enviar_contexto_ejecucion(pcb_ej, socketCPU, CONTEXTO_EJECUCION);

    switch(recibir_operacion(socketCPU))
    {
        t_list* lista_recepcion_valores;
        pcb* contexto_recibido;

        case WAIT:
            lista_recepcion_valores = _recibir_paquete(socketCPU);
            char* nombre_recurso = list_get(lista_recepcion_valores, 0);
            log_info(logger_planificador_extra,"Nombre de archivo para realizar F_OPEN: %s", nombre_recurso);

            int operacion_fopen = recibir_operacion(socketCPU);
            t_list* lista_contexto_fopen = _recibir_paquete(socketCPU);
            pcb* contexto_de_fopen = recibir_contexto_ejecucion(lista_contexto_fopen);

            list_destroy_and_destroy_elements(lista_recepcion_valores,free);
            list_destroy_and_destroy_elements(lista_contexto_fopen, free);
            liberar_contexto_ejecucion(contexto_de_fopen);
        break;
    
    }
*/

    pthread_t hiloConsolas; 
    pthread_create(&hiloConsolas, NULL, (void*)esperar_consolas, (void*)socketServicioConsolas);
    pthread_join(hiloConsolas, NULL);

    config_destroy(kernelConfig);
    destruir_estructuras_planificacion();

    return 0;
}
#include <main.h>


int main(int argc, char *argv[]) {

    //Logger* loggerKernel = iniciar_logger_modulo(KERNEL_LOGGER);

    Config* kernelConfig = config_create(argv[1]);
    configuracionKernel = obtener_valores_de_configuracion_kernel(kernelConfig);
    mostrar_valores_de_configuracion_kernel(configuracionKernel);
    leer_diccionario_recursos();
    
    socketMemoria = conectar_con_memoria(configuracionKernel);
    socketCPU = conectar_con_cpu(configuracionKernel);
    socketFS = conectar_con_filesystem(configuracionKernel);

    inicializar_estructuras_planificacion();
    iniciar_planificadores();
    

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


    pthread_t hiloConsolas; 
    pthread_create(&hiloConsolas, NULL, (void*)esperar_consolas, (void*)socketServicioConsolas);
    pthread_join(hiloConsolas, NULL);

    //log_destroy(loggerKernel);
    config_destroy(kernelConfig);
    destruir_estructuras_planificacion();

    return 0;
}
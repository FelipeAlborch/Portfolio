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

    pthread_t hiloConsolas; 
    pthread_create(&hiloConsolas, NULL, (void*)esperar_consolas, (void*)socketServicioConsolas);
    pthread_join(hiloConsolas, NULL);

    //log_destroy(loggerKernel);
    config_destroy(kernelConfig);
    destruir_estructuras_planificacion();

    return 0;
}
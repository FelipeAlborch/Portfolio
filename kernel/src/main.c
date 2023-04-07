#include <main.h>


int main(int argc, char *argv[]) {

    Logger* loggerKernel = iniciar_logger_modulo(KERNEL_LOGGER);

    log_info(loggerKernel, "Esto es el kernel :D");

    Config* kernelConfig = config_create(argv[1]);
    configuracionKernel = obtener_valores_de_configuracion_kernel(kernelConfig);
    mostrar_valores_de_configuracion_kernel(configuracionKernel);
    
    int socketMemoria = conectar_con_memoria(configuracionKernel);
    int socketCPU = conectar_con_cpu(configuracionKernel);
    //int socketFS = conectar_con_filesystem(configuracionKernel);

    inicializar_planificadores(configuracionKernel);

    socket_cpu_planificador = socketCPU;

    int socketServicioConsolas = iniciar_servidor_para_consolas(configuracionKernel);

    //pcb* unPcb = malloc(sizeof(pcb));
    //memcpy(unPcb->AX,"HOLA",4);
    //printf("en ax se guardo: %c", unPcb->AX[0]);
    //memcpy(unPcb->EAX,"HOLACHAU",8);
    //log_info(loggerKernel,"En EAX se guardo: %c", unPcb->EAX[7]);

    pthread_t hiloConsolas; 
    pthread_create(&hiloConsolas, NULL, (void*)esperar_consolas, (void*)socketServicioConsolas);
    pthread_join(hiloConsolas, NULL);


    // void *memcpy(void *dest, const void *src, size_t n);

    log_destroy(loggerKernel);
    config_destroy(kernelConfig);

    return 0;
}
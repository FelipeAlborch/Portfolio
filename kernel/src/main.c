#include <main.h>


int main(int argc, char *argv[]) {

    Logger* loggerKernel = iniciar_logger_modulo(KERNEL_LOGGER);

    log_info(loggerKernel, "Esto es el kernel :D");

    Config* kernelConfig = config_create(argv[1]);
    configuracionKernel = obtener_valores_de_configuracion_kernel(kernelConfig);
    mostrar_valores_de_configuracion_kernel(configuracionKernel);
    
    int socketMemoria = conectar_con_memoria(configuracionKernel);
    int socketCPU = conectar_con_cpu(configuracionKernel);
    // conectar_con_cpu
    // conectar_con_filesystem

    // conectar_con_consola(s)
    // conexion_con_consolas(configuracion_kernel);

    log_destroy(loggerKernel);
    config_destroy(kernelConfig);

    return 0;
}
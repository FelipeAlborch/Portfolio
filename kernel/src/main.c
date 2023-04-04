#include <main.h>


int main(int argc, char *argv[]) {

    t_log* logger_kernel = iniciar_logger_modulo(KERNEL_LOGGER);

    log_info(logger_kernel, "Esto es el kernel :D");

    t_config* kernel_config = config_create(argv[1]);
    configuracion_kernel = obtener_valores_de_configuracion_kernel(kernel_config);
    mostrar_valores_de_configuracion_kernel(configuracion_kernel);
    
    int socket_memoria = conectar_con_memoria(configuracion_kernel);
    // conectar_con_cpu
    // conectar_con_filesystem

    // conectar_con_consola(s)
    // conexion_con_consolas(configuracion_kernel);

    log_destroy(logger_kernel);
    config_destroy(kernel_config);

    return 0;
}
#include <main.h>

int socket_kernel;

int main(int argc, char *argv[]) {

    t_log* logger_consola = iniciar_logger_modulo(CONSOLA_LOGGER);
    log_info(logger_consola,"El logger abre bien! :D");

    t_config* consola_config = config_create(argv[1]);
    configuracion_consola = obtener_valores_de_configuracion_consola(consola_config);
    mostrar_valores_de_configuracion_consola(configuracion_consola); 
    
    if(conectar_con_kernel() == SUCCESS)
    {
        
    }

    log_destroy(logger_consola);
    config_destroy(consola_config);

    return 0;
}
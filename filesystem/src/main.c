#include <main.h>


int main(int argc, char *argv[]) {
    
    t_log* logger_filesystem = iniciar_logger_modulo(FILESYSTEM_LOGGER);

    log_info(logger_filesystem, "Este es el filesystem :D");

    t_config* filesystem_config = config_create(argv[1]);
    configuracion_filesystem = obtener_valores_de_configuracion_filesystem(filesystem_config);
    mostrar_valores_de_configuracion_filesystem(configuracion_filesystem);




    log_destroy(logger_filesystem);
    config_destroy(filesystem_config);
        
    return 0;
}
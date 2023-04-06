#include <utils.h>
#include <config.h>

int main(int argc, char *argv[]) {
    
    // Logger* logger = iniciar_logger_modulo(FILESYSTEM_LOGGER);

    fs_config *fs_config = fs_config_create(argv[1]);

    fs_config_print(fs_config);

    // int fs_socket = iniciar_servidor_en("127.0.0.1", config..)

    // int mem_socket = conectar_con_memoria("127.0.0.1", config..);

    // log_destroy(logger);

    fs_config_destroy(fs_config);
        
    return 0;
}
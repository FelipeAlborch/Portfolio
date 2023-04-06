#include <main.h>


int main(int argc, char *argv[]) {
    
    Logger* loggerFilesystem = iniciar_logger_modulo(FILESYSTEM_LOGGER);
    Config* filesystemConfig = config_create(argv[1]);

    configuracionFileSystem = obtener_valores_de_configuracion_filesystem(filesystemConfig);
    mostrar_valores_de_configuracion_filesystem(configuracionFileSystem);

    //int socketFileSystem = iniciar_servidor_en("127.0.0.1", configuracionFilesystem.)
    int socketMemoria = conectar_con_memoria(configuracionFileSystem);

    log_destroy(loggerFilesystem);
    config_destroy(filesystemConfig);
        
    return 0;
}
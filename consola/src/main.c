#include <main.h>

#define ARCHIVO_CODIGO "pseudocodigo.txt"

int socket_kernel;

int main(int argc, char *argv[]) {

    Logger* logger_consola = iniciar_logger_modulo(CONSOLA_LOGGER);

    Config* consola_config = config_create(argv[1]);
    configuracion_consola = obtener_valores_de_configuracion_consola(consola_config);
    mostrar_valores_de_configuracion_consola(configuracion_consola); 
    
    //char *archivo_codigo = (argv[2] != NULL) ? argv[1] : ARCHIVO_CODIGO;
    char* archivo_codigo = argv[2];
    //printf("\n\nArchivo codigo: %s\n\n", archivo_codigo);
    int socketKernel = conectar_con_kernel(configuracion_consola);

    log_info(logger_consola, "Me conecte al kernel!");
    enviar_instrucciones_a_kernel(archivo_codigo);

    log_destroy(logger_consola);
    config_destroy(consola_config);

    return 0;
}
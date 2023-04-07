#include <main.h>

#define ARCHIVO_CODIGO "pseudocodigo.txt"

int socket_kernel;

int main(int argc, char *argv[]) {

    Logger* logger_consola = iniciar_logger_modulo(CONSOLA_LOGGER);
    log_info(logger_consola,"El logger abre bien! :D");

    char *archivo_codigo = (argv[1] != NULL) ? argv[1] : ARCHIVO_CODIGO;
    printf("\n\nArchivo codigo: %s\n\n", archivo_codigo);
    
    Config* consola_config = config_create(argv[2]);
    configuracion_consola = obtener_valores_de_configuracion_consola(consola_config);
    mostrar_valores_de_configuracion_consola(configuracion_consola); 
    
    int socketKernel = conectar_con_kernel(configuracion_consola);

    log_info(logger_consola, "Me conecte al kernel!");
    enviar_instrucciones_a_kernel(archivo_codigo);


    //sleep(10);

    log_destroy(logger_consola);
    config_destroy(consola_config);

    return 0;
}
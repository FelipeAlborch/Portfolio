#include <main.h>

#define ARCHIVO_CODIGO "pseudocodigo.txt"


int main(int argc, char *argv[]) {

    Logger* logger_consola = iniciar_logger_modulo(CONSOLA_LOGGER);

    Config* consola_config = config_create(argv[1]);
    configuracion_consola = obtener_valores_de_configuracion_consola(consola_config);
    mostrar_valores_de_configuracion_consola(configuracion_consola); 
    
    
    //char *archivo_codigo = (argv[2] != NULL) ? argv[1] : ARCHIVO_CODIGO;
    
    char* archivo_codigo = argv[2];
    FILE* archivo_pseudocodigo;
   
    if ((archivo_pseudocodigo = fopen(archivo_codigo, "r")) == NULL)
    {
        log_error(logger_consola, "[Consola] Abriendo archivo %s [FAIL]", archivo_codigo);
    }
    
    int socketKernel = conectar_con_kernel(configuracion_consola);
    socket_kernel = socketKernel;

    //leer_archivo(archivo_pseudocodigo, logger_consola);

    t_list* lista_instrucciones = enlistar_instrucciones(archivo_pseudocodigo);
    leer_lista_de_instrucciones(lista_instrucciones);

    t_paquete* paquete_instrucciones = crear_paquete_operacion(LISTA_INSTRUCCIONES);
    empaquetar_instrucciones(paquete_instrucciones, lista_instrucciones);

    enviar_paquete(paquete_instrucciones, socketKernel);

    //list_destroy_and_destroy_elements(lista_instrucciones, (void*) liberar_instruccion);
    //for(int i = 0; i < list_size(lista_instrucciones); i++)
    //{
    //    LineaInstruccion* instr = list_get(lista_instrucciones, i);
    //    liberar_instruccion(instr);
    //}
   //list_destroy(lista_instrucciones);        //Este list_destroy tira error: corrupted size vs. prev_size Aborted (core dumped)
    eliminar_paquete(paquete_instrucciones);
    
    log_info(logger_consola, "Instrucciones enviadas correctamente");
    
    
    switch(recibir_operacion(socketKernel))
    {
        case EXIT:
            log_info(logger_consola, "Voy a terminar! :D");
            fclose(archivo_pseudocodigo);
            log_destroy(logger_consola);
            config_destroy(consola_config);
            close(socketKernel);

            exit(1);
        break;

        default:
            log_warning(logger_consola, "Operacion desconocida, no fue exit");
        break;
    }

    return 0;
}
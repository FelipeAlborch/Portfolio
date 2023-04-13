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
    
    //printf("\n\nArchivo codigo: %s\n\n", archivo_codigo);
    
    int socketKernel = conectar_con_kernel(configuracion_consola);
    socket_kernel = socketKernel;

    enviar_instrucciones_a_kernel(archivo_pseudocodigo);
    
    //FILE* el_archivo = fopen(archivo_codigo, "r");
    //char* linea = leer_linea(el_archivo);
    //printf("linea: %s\n", linea);
    //char** tokens = obtener_tokens(linea);
    //printf("Token 1: %s, 2: %s 3: %s, 4: %s\n", tokens[0], tokens[1], tokens[2], tokens[3]);
    //LineaInstruccion *linea_instruccion = malloc(sizeof(LineaInstruccion));
    //asignar_tokens_a_linea_instruccion(linea_instruccion, tokens);
    //printf("instr: %s, %s, %s, %s",linea_instruccion->identificador, linea_instruccion->parametros[0], linea_instruccion->parametros[1], linea_instruccion->parametros[2], linea_instruccion->parametros[3]);
    
    //t_list* lista = list_create();
    //list_add(lista, linea_instruccion);

    //free(linea_instruccion->identificador);
    //free(linea_instruccion->parametros[0]);
    //free(linea_instruccion->parametros[1]);
    //free(linea_instruccion->parametros[2]);
    //free(linea_instruccion->parametros[3]);
    //free(linea_instruccion);

    //list_destroy(lista);
    //free(tokens[0]);
    //free(tokens[1]);
    //free(tokens[2]);
    //free(tokens[3]);
    //free(tokens);
//
    //free(linea);
    



    fclose(archivo_pseudocodigo);
    log_destroy(logger_consola);
    config_destroy(consola_config);

    return 0;
}
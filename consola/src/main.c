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
    log_info(logger_consola, "Instrucciones enviadas correctamente");
    
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

    /*
    LineaInstruccion* instr1 = leer_linea_de_instruccion(archivo_pseudocodigo);
    log_info(logger_consola, "%s %s %s %s", instr1->identificador, instr1->parametros[0], instr1->parametros[1], instr1->parametros[2]);
    LineaInstruccion* instr2 = leer_linea_de_instruccion(archivo_pseudocodigo);
    log_info(logger_consola, "%s %s %s %s", instr2->identificador, instr2->parametros[0], instr2->parametros[1], instr2->parametros[2]);
    
    LineaInstruccion* instr3 = leer_linea_de_instruccion(archivo_pseudocodigo);
    log_info(logger_consola, "%s %s %s %s", instr3->identificador, instr3->parametros[0], instr3->parametros[1], instr3->parametros[2]);
    
    LineaInstruccion* instr4 = leer_linea_de_instruccion(archivo_pseudocodigo);
    log_info(logger_consola, "%s %s %s %s", instr4->identificador, instr4->parametros[0], instr4->parametros[1], instr4->parametros[2]);
    
    LineaInstruccion* instr5 = leer_linea_de_instruccion(archivo_pseudocodigo);
    log_info(logger_consola, "%s %s %s %s", instr5->identificador, instr5->parametros[0], instr5->parametros[1], instr5->parametros[2]);
    
    LineaInstruccion* instr6 = leer_linea_de_instruccion(archivo_pseudocodigo);
    log_info(logger_consola, "%s %s %s %s", instr6->identificador, instr6->parametros[0], instr6->parametros[1], instr6->parametros[2]);
    */

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
#include <consolaUtils.h>

void enviar_instrucciones_a_kernel(char *nombre_archivo)
{
  Logger *logger = iniciar_logger_modulo(CONSOLA_LOGGER);
  Lista *instrucciones;
  FILE *archivo;

  if ((archivo = fopen(nombre_archivo, "r")) == NULL)
  {
    log_error(logger, "[Consola] Abriendo archivo %s [FAIL]", nombre_archivo);
  }

  instrucciones = list_create();

  rellenar_lista_de_instrucciones(instrucciones, archivo);

  fclose(archivo);

  // Esta parte hay que rehacerla
  //t_paquete *paquete = crear_paquete(LINEAS_CODIGO);
  /*serializar_segmentos_instrucciones(paquete, instrucciones, CONSOLA_CONFIG.SEGMENTOS);

  log_info(logger, "[Consola] Enviando instrucciones al Servidor Kernel [...]");

  enviar_paquete_a_servidor(paquete, socket_kernel_fd);
  log_info(logger, "[Consola] Enviando instrucciones al Servidor Kernel [OK]");
  */
  
  dump(instrucciones);

  for(int i = 0; i < list_size(instrucciones); i++)
  {
      LineaInstruccion* una_instr = list_get(instrucciones, i);
      free(una_instr->identificador);
      free(una_instr->parametros[0]);
      free(una_instr->parametros[1]);
      free(una_instr->parametros[2]);
      free(una_instr);
  }

  list_destroy(instrucciones);
  log_destroy(logger);
  //eliminar_paquete(paquete);
}

void rellenar_lista_de_instrucciones(Lista *lista, FILE *archivo)
{
  while (!feof(archivo))
  {
    LineaInstruccion *linea_instruccion = leer_linea_de_instruccion(archivo);

    agregar_linea_de_instruccion_a_lista(linea_instruccion, lista);
  }
}

LineaInstruccion *leer_linea_de_instruccion(FILE *archivo)
{
  char *linea = leer_linea(archivo);

  char **tokens = obtener_tokens(linea);

  LineaInstruccion *linea_instruccion = malloc(sizeof(LineaInstruccion));

  asignar_tokens_a_linea_instruccion(linea_instruccion, tokens);

  free(linea);

  return linea_instruccion;
}

char *leer_linea(FILE *archivo)
{
  char *linea = string_new();
  
  int tamanio_buffer = 0;

  if (getline(&linea, (size_t *restrict)&tamanio_buffer, archivo)) {};

  eliminar_salto_linea(linea);

  return linea;
}

void eliminar_salto_linea(char *linea)
{
  if (linea[strlen(linea) - 1] == '\n')
  {
    linea[strlen(linea) - 1] = '\0';
  }
}

char **obtener_tokens(char *string)
{
  short cantidad_tokens = cantidad_de_tokens(string);

  char **tokens = string_n_split(string, cantidad_tokens, " ");

  rellenar_parametros_de_instruccion(tokens, cantidad_tokens);

  return tokens;
}

int cantidad_de_tokens(char *string)
{
  return veces(string, ' ') + 1;
}

int veces(char *string, char caracter)
{
  int veces = 0;

  for (int i = 0; i < string_length(string); i++)
  {
    if (string[i] == caracter) veces++;
  }

  return veces;
}

void rellenar_parametros_de_instruccion(char **tokens, int cantidad_tokens)
{
  if (cantidad_tokens < 2) tokens[1] = strdup("-1");

  if (cantidad_tokens < 3) tokens[2] = strdup("-1");

  if (cantidad_tokens < 4) tokens[3] = strdup("-1");
}

void asignar_tokens_a_linea_instruccion(LineaInstruccion *linea_instruccion, char **tokens)
{
  linea_instruccion->identificador = strdup(tokens[0]);
  
  linea_instruccion->parametros[0] = strdup(tokens[1]);

  linea_instruccion->parametros[1] = strdup(tokens[2]);

  linea_instruccion->parametros[2] = strdup(tokens[3]);

  //free(*tokens);
}

void agregar_linea_de_instruccion_a_lista(LineaInstruccion *linea_instruccion, Lista *lista)
{
  
  list_add(lista, linea_instruccion);
  
  /*if (!strcmp(linea_instruccion->identificador, "NO_OP"))
  {
    for (int i = 0; i < linea_instruccion->parametros[0]; i++)
    {
      list_add(lista, linea_instruccion);
    }
    linea_instruccion->parametros[0] = -1;
  }
  else
  {
    list_add(lista, linea_instruccion);
  }*/
}

void dump(Lista *instrucciones)
{
  for (int i = 0; i < list_size(instrucciones); i++)
  {
    LineaInstruccion *lineaInstruccion = list_get(instrucciones, i);

    printf(
      "Instruccion: %s %s %s %s\n", 
      lineaInstruccion->identificador, 
      lineaInstruccion->parametros[0], 
      lineaInstruccion->parametros[1],
	  lineaInstruccion->parametros[2]
    );
  }
}
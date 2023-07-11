#include <consolaUtils.h>

int socket_kernel;
/*
void enviar_instrucciones_a_kernel(FILE* archivo_pseudocodigo)
{
  Logger *logger = iniciar_logger_modulo(CONSOLA_LOGGER);
  Lista *instrucciones = list_create();;

  rellenar_lista_de_instrucciones(instrucciones, archivo_pseudocodigo);

  t_paquete* paquete_kernel = crear_paquete_operacion(LISTA_INSTRUCCIONES);
  empaquetar_instrucciones(paquete_kernel, instrucciones);
  enviar_paquete(paquete_kernel,socket_kernel);
  
  dump(instrucciones);
  
  // La funcion liberar_instruccion esta en el archivo PCB.c, la puse ahi porque ahi pusiste el struct LineaInstruccion. De todas maneras va a venir bien para liberar las instrucciones del pcb
  //list_destroy_and_destroy_elements(instrucciones, (void*)liberar_instruccion);
  for(int i = 0; i < list_size(instrucciones); i++)
  {
    LineaInstruccion* instru = list_get(instrucciones, i);
    liberar_instruccion(instru);
  }
  //list_destroy(instrucciones);
  log_destroy(logger);
  eliminar_paquete(paquete_kernel);
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
  if (cantidad_tokens < 2) tokens[1] = string_duplicate("-1");

  if (cantidad_tokens < 3) tokens[2] = string_duplicate("-1");

  if (cantidad_tokens < 4) tokens[3] = string_duplicate("-1");
}

void asignar_tokens_a_linea_instruccion(LineaInstruccion *linea_instruccion, char **tokens)
{
  linea_instruccion->identificador = string_duplicate(tokens[0]);
  
  linea_instruccion->parametros[0] = string_duplicate(tokens[1]);

  linea_instruccion->parametros[1] = string_duplicate(tokens[2]);

  linea_instruccion->parametros[2] = string_duplicate(tokens[3]);

  //liberar_tokens(tokens);
}

void agregar_linea_de_instruccion_a_lista(LineaInstruccion *linea_instruccion, Lista *lista)
{
  list_add(lista, linea_instruccion);
}

void* liberar_tokens(char** tokens)
{
  // Hay que eliminar cada posicion en la que podriamos haber escrito algo, y despues el tokens en si.
  free(tokens[0]);
  free(tokens[1]);
  free(tokens[2]);
  free(tokens[3]);
  free(tokens);
}
*/
void empaquetar_instrucciones(t_paquete* paquete_a_kernel, t_list* lista_de_instrucciones)
{
  LineaInstruccion* una_instruccion;
  int i;
  int cantidad_de_instrucciones = list_size(lista_de_instrucciones);

  agregar_a_paquete(paquete_a_kernel, &cantidad_de_instrucciones, sizeof(int));
  for(i = 0; i < cantidad_de_instrucciones; i++)
  {
    una_instruccion = list_get(lista_de_instrucciones, i);
    agregar_a_paquete(paquete_a_kernel, una_instruccion->identificador, strlen(una_instruccion->identificador) + 1);
    agregar_a_paquete(paquete_a_kernel, una_instruccion->parametros[0], strlen(una_instruccion->parametros[0]) + 1);
    agregar_a_paquete(paquete_a_kernel, una_instruccion->parametros[1], strlen(una_instruccion->parametros[1]) + 1);
    agregar_a_paquete(paquete_a_kernel, una_instruccion->parametros[2], strlen(una_instruccion->parametros[2]) + 1);
  }
}





char* leer_linea(FILE* archivo)
{
  char* linea_leida; //= string_new();
  size_t tam_linea = 0;

  getline(&linea_leida, &tam_linea, archivo);

  while(linea_leida[string_length(linea_leida) -1] == '\n' || linea_leida[string_length(linea_leida) -1] == '\r')
  {
    linea_leida[string_length(linea_leida) -1] = '\0';
  }

  //if(linea_leida[string_length(linea_leida) -1] == '\n' || linea_leida[string_length(linea_leida) -1] == '\r')
  //{
  //  linea_leida[string_length(linea_leida) -1] = '\0';
  //}
  //if(linea_leida[string_length(linea_leida) -1] == '\n' || linea_leida[string_length(linea_leida) -1] == '\r')
  //{
  //  linea_leida[string_length(linea_leida) -1] = '\0';
  //}
    
  return linea_leida;
}

int cantidad_de_valores(char* linea)
{
  int cantidad_de_valores = 0;

  for(int i = 0; i < string_length(linea); i++)
  {
    if(linea[i] == ' ')
    {
      cantidad_de_valores++;
    }
  }
  return cantidad_de_valores+1;
}

void inicializar_parametros(char** parametros, int cantidad_valores)
{
  if(cantidad_valores < 2)
  {
    parametros[1] = "-1";
    parametros[2] = "-1";
    parametros[3] = "-1";
  }
  else if(cantidad_valores < 3)
  {
    parametros[2] = "-1";
    parametros[3] = "-1";
  }
  else if(cantidad_valores < 4)
  {
    parametros[3] = "-1";
  }
}

char** identificar_parametros(char* linea)
{
  int cantidad_valores = cantidad_de_valores(linea); 
  char** parametros = string_n_split(linea, cantidad_valores, " ");
  inicializar_parametros(parametros, cantidad_valores);

  return parametros;
}

LineaInstruccion* leer_instruccion(FILE* archivo)
{
  char* linea = leer_linea(archivo);
  char** parametros = identificar_parametros(linea);
  
  LineaInstruccion* instruccion_leida = malloc(sizeof(LineaInstruccion));
  
  crear_instruccion(instruccion_leida, parametros);
  
  //int cantidad_valores = cantidad_de_valores(linea);
  //for (int i = 0; i < cantidad_valores; i++) {
  //  free(parametros[i]);
  //}
  //free(parametros);
  
  free(linea);

  return instruccion_leida;
}

void crear_instruccion(LineaInstruccion* instruccion_leida, char** parametros)
{
  instruccion_leida->identificador = string_duplicate(parametros[0]);
  instruccion_leida->parametros[0] = string_duplicate(parametros[1]);
  instruccion_leida->parametros[1] = string_duplicate(parametros[2]);
  instruccion_leida->parametros[2] = string_duplicate(parametros[3]);
}

void leer_archivo(FILE* archivo, t_log* logger)
{
  LineaInstruccion* una_instruccion;

  while(!feof(archivo))
  {
    una_instruccion = leer_instruccion(archivo);
    log_info(logger, "Se leyo: %s con los parametros %s %s %s", una_instruccion->identificador, una_instruccion->parametros[0], una_instruccion->parametros[1], una_instruccion->parametros[2]);
    liberar_instruccion(una_instruccion);
  }
}

t_list* enlistar_instrucciones(FILE* archivo)
{
  t_list* lista_instrucciones = list_create();
  while(!feof(archivo))
  {
    LineaInstruccion* una_instruccion = leer_instruccion(archivo);
    list_add(lista_instrucciones, una_instruccion);
  }
  return lista_instrucciones;
}

void leer_lista_de_instrucciones(t_list* lista_instrucciones)
{
  int i;
  for(i = 0; i < list_size(lista_instrucciones); i++)
  {
    LineaInstruccion* una_instruccion = list_get(lista_instrucciones, i);
    imprimir_instruccion(una_instruccion, i+1);
  }
}

void imprimir_instruccion(LineaInstruccion* una_instruccion, int nro_instruccion)
{
  printf("INSTRUCCION [%d]: %s %s %s %s\n", nro_instruccion, una_instruccion->identificador, una_instruccion->parametros[0], una_instruccion->parametros[1], una_instruccion->parametros[2]);
}
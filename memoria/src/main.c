#include <main.h>


int main(int argc, char *argv[]) {

  t_log* logger_memoria = iniciar_logger_modulo(MEMORIA_LOGGER);

  log_info(logger_memoria, "Esto es la memoria D:");  

  t_config* memoria_config = config_create(argv[1]);
  configuracion_memoria = obtener_valores_de_configuracion_memoria(memoria_config);
  mostrar_valores_de_configuracion_memoria(configuracion_memoria);

  int socket_servicio_memoria = iniciar_servicio_memoria(configuracion_memoria);
  int socket_de_un_cliente = esperar_cliente(socket_servicio_memoria);


  printf("EL socket de servicio es: %d", socket_servicio_memoria);
  printf("EL socket del cliente es: %d", socket_de_un_cliente);



  log_destroy(logger_memoria);
  config_destroy(memoria_config);
    
  return 0;
}
#include <main.h>


int main(int argc, char *argv[]) {

  Logger* loggerMemoria = iniciar_logger_modulo(MEMORIA_LOGGER);

  log_info(loggerMemoria, "Esto es la memoria D:");  

  Config* memoriaConfig = config_create(argv[1]);
  configuracionMemoria = obtener_valores_de_configuracion_memoria(memoriaConfig);
  mostrar_valores_de_configuracion_memoria(configuracionMemoria);

  int socketServicioMemoria = iniciar_servicio_memoria(configuracionMemoria);
  //int socket_de_un_cliente = esperar_cliente(socket_servicio_memoria);
  //printf("EL socket de servicio es: %d", socket_servicio_memoria);
  //printf("EL socket del cliente es: %d", socket_de_un_cliente);

  Hilo hiloCliente1, hiloCliente2, hiloCliente3;
  int socketCliente1 = esperar_cliente(socketServicioMemoria);
  pthread_create(&hiloCliente1, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente1);

  int socketCliente2 = esperar_cliente(socketServicioMemoria);
  pthread_create(&hiloCliente2, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente2);

  int socketCliente3 = esperar_cliente(socketServicioMemoria);
  pthread_create(&hiloCliente3, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente3);

  pthread_join(hiloCliente1, NULL);
  pthread_join(hiloCliente2, NULL);
  pthread_join(hiloCliente3, NULL);

  log_destroy(loggerMemoria);
  config_destroy(memoriaConfig);
    
  return 0;
}
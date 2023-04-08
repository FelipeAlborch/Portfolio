#include <main.h>

int main(int argc, char *argv[]) {

	startSigHandlers();


  

  memoriaConfig = config_create(argv[1]);
  
  inicializar_memoria();
  log_info(loggerMemoria, "Esto es la memoria D:");  

  //conectar();
  pthread_create(&hiloConexion, NULL, (void *)conectar, NULL);
  pthread_create(&hilo_kernel, NULL, (void *)conectar, NULL);
  pthread_join(hiloConexion, NULL);
  pthread_join(hilo_kernel,NULL);
  /* int socketServicioMemoria = iniciar_servicio_memoria(config_memo);

  pthread_t hiloCliente1, hiloCliente2, hiloCliente3;
  int socketCliente1 = esperar_cliente(socketServicioMemoria);
  pthread_create(&hiloCliente1, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente1);

  int socketCliente2 = esperar_cliente(socketServicioMemoria);
  pthread_create(&hiloCliente2, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente2);

  int socketCliente3 = esperar_cliente(socketServicioMemoria);
  pthread_create(&hiloCliente3, NULL, (void *)manejar_paquetes_clientes, (void *)socketCliente3);

  pthread_join(hiloCliente1, NULL);
  pthread_join(hiloCliente2, NULL);
  pthread_join(hiloCliente3, NULL);
 */
  
  //log_info(loggerMemoria,"antes de terminar programa");
	terminar_programa(loggerMemoria, memoriaConfig);  
  return 0;
}
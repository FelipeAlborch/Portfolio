#include <main.h>

int main(int argc, char *argv[]) {

	startSigHandlers();


  

  memoriaConfig = config_create(argv[1]);
  
  inicializar_memoria();
  log_info(loggerMemoria, "Esto es la memoria D:");  

  pthread_create(&hilo_cpu, NULL, (void *)conectar_cpu, NULL);
  pthread_join(hilo_cpu,NULL);
  pthread_create(&hilo_kernel, NULL, (void *)conectar_kernel, NULL);
  pthread_join(hilo_kernel,NULL);
  
  terminar_programa(loggerMemoria, memoriaConfig);  
  return 0;
}
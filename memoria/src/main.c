#include <main.h>

int main(int argc, char *argv[]) {

	startSigHandlers();

  memoriaConfig = config_create(argv[1]);
  
  inicializar_memoria();
  log_info(loggerMemoria, "Esto es la memoria D:");  
  //ejecutar_kernel_test();
  
  pthread_create(&hilo_fs, NULL, (void *)conectar_fs, NULL);
  pthread_detach(hilo_fs);
  
  sem_wait(&sem_fs_conectado);
  pthread_create(&hilo_cpu, NULL, (void *)conectar_cpu, NULL);
  pthread_detach(hilo_cpu);
  
  sem_wait(&sem_cpu_conectado);
  pthread_create(&hilo_kernel, NULL, (void *)conectar_kernel, NULL);
  //pthread_detach(hilo_kernel);

  pthread_join(hilo_cpu,NULL);
  pthread_join(hilo_fs,NULL);
  pthread_join(hilo_kernel,NULL);

  terminar_programa(loggerMemoria);   
  return 0;
}


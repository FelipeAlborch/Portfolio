#include <main.h>

int main(int argc, char *argv[]) {

  Logger* logger_cpu = iniciar_logger_modulo(CPU_LOGGER);
  log_info(logger_cpu, "Este es el cpu :D");

  Config* cpuConfig = config_create(argv[1]);
  configuracionCPU = obtener_valores_de_configuracion_cpu(cpuConfig);
  mostrar_valores_de_configuracion_cpu(configuracionCPU);

  log_info(logger_cpu, "Iniciando Servidor CPU...");
  int socketCpuDispatch = iniciar_servidor_en("127.0.0.1", configuracionCPU.PUERTO_ESCUCHA);

  if (socketCpuDispatch < 0)
  {
      log_error(logger_cpu, "Error intentando iniciar Servidor CPU.");
      log_destroy(logger_cpu);
      return 1;
  }

  log_info(logger_cpu, "Servidor de CPU iniciado correctamente!");

  int socketMemoria = conectar_con_memoria(configuracionCPU);
  socketMemoriaUtil = socketMemoria;
 
  Hilo hiloKernelDispatch;
  pthread_create(&hiloKernelDispatch, NULL, (void *)esperar_kernel_dispatch, (void *)socketCpuDispatch);
  pthread_join(hiloKernelDispatch, NULL);


  log_destroy(logger_cpu);
  config_destroy(cpuConfig);

  return 0;
}
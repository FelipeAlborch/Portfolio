#include <main.h>

int main(int argc, char *argv[]) {

  t_log* logger_cpu = iniciar_logger_modulo(CPU_LOGGER);
  log_info(logger_cpu, "Este es el cpu :D");

  t_config* cpu_config = config_create(argv[1]);
  configuracion_cpu = obtener_valores_de_configuracion_cpu(cpu_config);
  mostrar_valores_de_configuracion_cpu(configuracion_cpu);




  log_destroy(logger_cpu);
  config_destroy(cpu_config);

  return 0;
}
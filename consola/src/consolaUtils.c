#include <consolaUtils.h>

int conectar_con_kernel()
{
  log_info(logger, "[Consola] Conectando Kernel [...]");

  socket_kernel = crear_conexion(CONSOLA_CONFIG.IP_KERNEL, CONSOLA_CONFIG.PUERTO_KERNEL);

  if (socket_kernel < 0)
  {
    log_info(logger, "[Consola] Conectando Kernel [FAIL]");

    log_error(logger, "[Consola] Kernel no está disponible. socket_kernel_fd = %d", socket_kernel);

    return FAILURE;
  }

  log_info(logger, "[Consola] Conectando Kernel [OK]");

  return SUCCESS;
}
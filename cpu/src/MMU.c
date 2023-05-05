#include <MMU.h>

int obtener_direccion_fisica(int direccionLogica)
{
    Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

    int numeroSegmento = obtener_numero_de_segmento(direccionLogica);
    log_info(logger, "Numero de segmento calculado: [%d]", numeroSegmento);

    int desplazamientoSegmento = obtener_desplazamiento_del_segmento(direccionLogica);
    log_info(logger, "Desplazamiento del segmento [%d]: [%d]", numeroSegmento, desplazamientoSegmento);

    log_destroy(logger);
}

int obtener_num_segmento(int direccionLogica)
{
  int numeroSegmento = floor(direccionLogica / configuracionCPU.TAM_MAX_SEGMENTO);

  return numeroSegmento;
}

int obtener_desplazamiento_del_segmento(int direccionLogica)
{
  int desplazamientoSegmento = direccionLogica % configuracionCPU.TAM_MAX_SEGMENTO;

  return desplazamientoSegmento
}
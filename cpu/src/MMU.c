#include <MMU.h>

int obtener_direccion_fisica(char* direccionLogica, pcb *pcb, int cantBytes)
{
    Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

    int DL = atoi(direccionLogica);
    int numeroSegmento = obtener_num_segmento(DL);
    log_info(logger, "Numero de segmento calculado: [%d]", numeroSegmento);

    t_segmento* segmento = list_get(pcb->tabla_de_segmentos, numeroSegmento);
    log_info(logger, "La base del segmento [%d] es de: %d", numeroSegmento, segmento->base);

    int desplazamientoSegmento = obtener_desplazamiento_del_segmento(DL);
    log_info(logger, "Desplazamiento del segmento [%d]: [%d]", numeroSegmento, desplazamientoSegmento);

    log_info(logger, "La cantidad de bytes es de: %d", cantBytes);

    if(segmento->size < desplazamientoSegmento + cantBytes)
      return -1;

    int DF = desplazamientoSegmento + segmento->base;
    log_destroy(logger);
    return DF;
}

int obtener_num_segmento(int direccionLogica)
{
  int numeroSegmento = floor(direccionLogica / configuracionCPU.TAM_MAX_SEGMENTO);

  return numeroSegmento;
}

int obtener_desplazamiento_del_segmento(int direccionLogica)
{
  int desplazamientoSegmento = direccionLogica % configuracionCPU.TAM_MAX_SEGMENTO;

  return desplazamientoSegmento;
}
#include <MMU.h>

int obtener_direccion_fisica(char* direccionLogica, pcb *pcb, int cantBytes)
{
    Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

    int DL = atoi(direccionLogica);
    int numeroSegmento = obtener_num_segmento(DL);
    //log_debug(logger, "Numero de segmento calculado: [%d]", numeroSegmento);

    t_segmento* segmento = list_get(pcb->tabla_de_segmentos, numeroSegmento);
    //log_debug(logger, "La base del segmento [%d] es de: %d", numeroSegmento, segmento->base);

    int desplazamientoSegmento = obtener_desplazamiento_del_segmento(DL);
    //log_debug(logger, "Desplazamiento del segmento [%d]: [%d]", numeroSegmento, desplazamientoSegmento);
    log_debug(logger, "Segmento N°: [%d]. Base: [%d]. Tamanio: [%d]. Offset: [%d] ",
     numeroSegmento, segmento->base, segmento->size, desplazamientoSegmento);
    //log_debug(logger, "La cantidad de bytes es de: %d", cantBytes);
    //log_debug(logger, "El tamanio del segmento es de: %d", segmento->size);
    
    if(segmento->size <= desplazamientoSegmento + cantBytes){
      log_error(logger, "PID: < %d > - Error SEG_FAULT - Segmento: < %d > - Offset: < %d > - Tamaño: < %d >", pcb->pid, numeroSegmento, desplazamientoSegmento, cantBytes);
      log_destroy(logger);
      return -1;
    }
    //int DF = desplazamientoSegmento + segmento->base;
    //Sería así como lo necesito desde memoria:
    int DF = segmento->size + segmento->base;
    log_debug(logger, "La Direccion Fisica es: [%d]", (segmento->base + desplazamientoSegmento));
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
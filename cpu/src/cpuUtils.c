#include <cpuUtils.h>

void ejecutar_lista_instrucciones_del_pcb(pcb *pcb, int socketKernel, int socketMemoria)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  log_info(logger, "Ejecutando instrucciones del PCB Nº %d", pcb->pid);

  for (int i = pcb->program_counter; i < list_size(pcb->lista_de_instrucciones); i++)
  {
    log_info(logger, "Contador de Programa: %d\n", pcb->program_counter);

    LineaInstruccion *lineaInstruccion = list_get(pcb->lista_de_instrucciones, pcb->program_counter);
    Instruccion instruccion = obtener_tipo_instruccion(lineaInstruccion->identificador);
    log_info(logger, "Instruccion: %s\n", lineaInstruccion->identificador);
    log_info(logger, "Parametro 0: %s\n", lineaInstruccion->parametros[0]);
    log_info(logger, "Parametro 1: %s\n", lineaInstruccion->parametros[1]);
    log_info(logger, "Parametro 1: %s\n", lineaInstruccion->parametros[2]);

    log_info(logger, "INSTRUCCION EJECUTADA");
    log_info(logger, "< PID: %d | Nombre instruccion: %s | Parametros: %s %s %s >",
        pcb->pid, lineaInstruccion->identificador, lineaInstruccion->parametros[0]
        , lineaInstruccion->parametros[1], lineaInstruccion->parametros[2]);

    /*if (seNecesitaAtenderInterrupcion)
    {
      log_info(logger, "Se necesita atender una interrupción");

      atender_interrupcion(pcb, socketKernel);

      log_info(logger, "Se termino de atender una interrupción (valor : %d)", seNecesitaAtenderInterrupcion);
      log_destroy(logger);
      return;
    }*/
    pcb->program_counter++;

    switch (instruccion)
    {
      case SET:
        //ejecutar_set(pcb, lineaInstruccion);
        sleep(configuracionCPU.RETARDO_INSTRUCCION);
        //log_info(logger, "Estado de los registros: AX = %d, BX = %d, CX = %d, DX = %d", pcb->AX, pcb->BX, pcb->CX, pcb->DX);
        break;

      case EXIT: 
        //ejecutar_exit(pcb, socketKernel);
        log_info(logger, "Se envio el pcb %d a Kernel", pcb->pid);
        break;

      default:
        break;
    }

    /*if (instruccion == IO || instruccion == EXIT || haySegmentationFault || hayPageFault)
    {
      haySegmentationFault = false;
      hayPageFault = false;
      log_destroy(logger);
      return;
    }*/
  }
}

Instruccion obtener_tipo_instruccion(char *instruccion)
{
  if (!strcmp(instruccion, "SET")){
    printf("Lei la instruccion SET\n");
    return SET;
  }
  else if (!strcmp(instruccion, "I/O")){
    printf("Lei la instruccion I/0\n");
    return IO;
  }
  else if (!strcmp(instruccion, "MOV_IN")){
    printf("Lei la instruccion MOV_IN\n");
    return MOV_IN;
  }
  else if (!strcmp(instruccion, "MOV_OUT")){
    printf("Lei la instruccion MOV_OUT\n");
    return MOV_OUT;
  }
  else if (!strcmp(instruccion, "F_OPEN")){
    printf("Lei la instruccion ADD\n");
    return F_OPEN;
  }
  else if (!strcmp(instruccion, "F_CLOSE")){
    printf("Lei la instruccion ADD\n");
    return F_CLOSE;
  }
  else if (!strcmp(instruccion, "F_SEEK")){
    printf("Lei la instruccion ADD\n");
    return F_SEEK;
  }
  else if (!strcmp(instruccion, "F_READ")){
    printf("Lei la instruccion ADD\n");
    return F_READ;
  }
  else if (!strcmp(instruccion, "F_WRITE")){
    printf("Lei la instruccion ADD\n");
    return F_WRITE;
  }
  else if (!strcmp(instruccion, "F_TRUNCATE")){
    printf("Lei la instruccion ADD\n");
    return F_TRUNCATE;
  }
  else if (!strcmp(instruccion, "WAIT")){
    printf("Lei la instruccion ADD\n");
    return WAIT;
  }
  else if (!strcmp(instruccion, "SIGNAL")){
    printf("Lei la instruccion ADD\n");
    return SIGNAL;
  }
  else if (!strcmp(instruccion, "CREATE_SEGMENT")){
    printf("Lei la instruccion ADD\n");
    return CREATE_SEGMENT_INSTRUCTION;
  }
  else if (!strcmp(instruccion, "DELETE_SEGMENT")){
    printf("Lei la instruccion ADD\n");
    return DELETE_SEGMENT_INSTRUCTION;
  }
  else if (!strcmp(instruccion, "YIELD")){
    printf("Lei la instruccion ADD\n");
    return YIELD;
  }
  else if (!strcmp(instruccion, "EXIT")){
    printf("Lei la instruccion EXIT\n");
    return EXIT;
  }
  else{
    printf("Lei una instruccion desconocida\n");
    return DESCONOCIDA;
  }
}
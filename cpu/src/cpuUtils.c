#include <cpuUtils.h>

bool recursoDisponible;

void ejecutar_lista_instrucciones_del_pcb(pcb *pcb, int socketKernel, int socketMemoria)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  log_info(logger, "Ejecutando instrucciones del PCB Nº %d", pcb->pid);

  for (int i = pcb->program_counter; i < list_size(pcb->lista_de_instrucciones); i++)
  {
    log_info(logger, "Contador de Programa: %d\n", pcb->program_counter);

    LineaInstruccion *lineaInstruccion = list_get(pcb->lista_de_instrucciones, pcb->program_counter);
    Instruccion instruccion = obtener_tipo_instruccion(lineaInstruccion->identificador);
    //log_info(logger, "Instruccion: %s\n", lineaInstruccion->identificador);
    //log_info(logger, "Parametro 0: %s\n", lineaInstruccion->parametros[0]);
    //log_info(logger, "Parametro 1: %s\n", lineaInstruccion->parametros[1]);
    //log_info(logger, "Parametro 1: %s\n", lineaInstruccion->parametros[2]);

    logear_instruccion(pcb->pid, lineaInstruccion);

    pcb->program_counter++;

    switch (instruccion)
    {
      case SET:
        ejecutar_set(pcb, lineaInstruccion);
        estado_de_los_registros(pcb);
        //sleep(configuracionCPU.RETARDO_INSTRUCCION);
        break;

      case IO:
        ejecutar_io(pcb, lineaInstruccion, socketKernel);
        break;
      
      /*case WAIT:
        ejecutar_wait();
        break; */ 

      case YIELD:
        ejecutar_yield(pcb, socketKernel);
        break;

      case EXIT: 
        ejecutar_exit(pcb, socketKernel);
        break;

      default:
        break;
    }

    if (instruccion == YIELD || instruccion == EXIT || instruccion == IO)
    {
      log_destroy(logger);
      return;
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
    //printf("Lei la instruccion SET\n");
    return SET;
  }
  else if (!strcmp(instruccion, "IO")){
    //printf("Lei la instruccion I/0\n");
    return IO;
  }
  else if (!strcmp(instruccion, "MOV_IN")){
    //printf("Lei la instruccion MOV_IN\n");
    return MOV_IN;
  }
  else if (!strcmp(instruccion, "MOV_OUT")){
    //printf("Lei la instruccion MOV_OUT\n");
    return MOV_OUT;
  }
  else if (!strcmp(instruccion, "F_OPEN")){
    //printf("Lei la instruccion F_OPEN\n");
    return F_OPEN;
  }
  else if (!strcmp(instruccion, "F_CLOSE")){
    //printf("Lei la instruccion F_CLOSE\n");
    return F_CLOSE;
  }
  else if (!strcmp(instruccion, "F_SEEK")){
    //printf("Lei la instruccion F_SEEK\n");
    return F_SEEK;
  }
  else if (!strcmp(instruccion, "F_READ")){
    //printf("Lei la instruccion F_READ\n");
    return F_READ;
  }
  else if (!strcmp(instruccion, "F_WRITE")){
    //printf("Lei la instruccion F_WRITE\n");
    return F_WRITE;
  }
  else if (!strcmp(instruccion, "F_TRUNCATE")){
    //printf("Lei la instruccion F_TRUNCATE\n");
    return F_TRUNCATE;
  }
  else if (!strcmp(instruccion, "WAIT")){
    //printf("Lei la instruccion WAIT\n");
    return WAIT;
  }
  else if (!strcmp(instruccion, "SIGNAL")){
    //printf("Lei la instruccion SIGNAL\n");
    return SIGNAL;
  }
  else if (!strcmp(instruccion, "CREATE_SEGMENT")){
    //printf("Lei la instruccion CREATE_SEGMENT\n");
    return CREATE_SEGMENT_INSTRUCTION;
  }
  else if (!strcmp(instruccion, "DELETE_SEGMENT")){
    //printf("Lei la instruccion DELETE_SEGMENT\n");
    return DELETE_SEGMENT_INSTRUCTION;
  }
  else if (!strcmp(instruccion, "YIELD")){
    //printf("Lei la instruccion YIELD\n");
    return YIELD;
  }
  else if (!strcmp(instruccion, "EXIT")){
    //printf("Lei la instruccion EXIT\n");
    return EXIT;
  }
  else{
    //printf("Lei una instruccion desconocida\n");
    return DESCONOCIDA;
  }
}

void ejecutar_set(pcb *pcb, LineaInstruccion *instruccion)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  if(!strcmp(instruccion->parametros[0], "AX"))
    strcpy(pcb->AX, instruccion->parametros[1]);
  else if(!strcmp(instruccion->parametros[0], "BX"))
    strcpy(pcb->BX, instruccion->parametros[1]);  
  else if(!strcmp(instruccion->parametros[0], "CX"))
    strcpy(pcb->CX, instruccion->parametros[1]);
  else if(!strcmp(instruccion->parametros[0], "DX"))
    strcpy(pcb->DX, instruccion->parametros[1]);
  if(!strcmp(instruccion->parametros[0], "EAX"))
    strcpy(pcb->EAX, instruccion->parametros[1]);
  else if(!strcmp(instruccion->parametros[0], "EBX"))
    strcpy(pcb->EBX, instruccion->parametros[1]);  
  else if(!strcmp(instruccion->parametros[0], "ECX"))
    strcpy(pcb->ECX, instruccion->parametros[1]);
  else if(!strcmp(instruccion->parametros[0], "EDX"))
    strcpy(pcb->EDX, instruccion->parametros[1]);
  if(!strcmp(instruccion->parametros[0], "RAX"))
    strcpy(pcb->RAX, instruccion->parametros[1]);
  else if(!strcmp(instruccion->parametros[0], "RBX"))
    strcpy(pcb->RBX, instruccion->parametros[1]);  
  else if(!strcmp(instruccion->parametros[0], "RCX"))
    strcpy(pcb->RCX, instruccion->parametros[1]);
  else if(!strcmp(instruccion->parametros[0], "RDX"))
    strcpy(pcb->RDX, instruccion->parametros[1]);

  log_info(logger, "Se copio al registro %s el valor %s", instruccion->parametros[0], instruccion->parametros[1]);
  log_destroy(logger);
}

void ejecutar_io(pcb *pcb, LineaInstruccion *instruccion, int socketKernel)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  pcb->tiempo_io = atoi(instruccion->parametros[0]);

  log_info(logger, "Enviando el contexto de ejecucion del proceso [%d] a Kernel...", pcb->pid);
  enviar_contexto_ejecucion(pcb, socketKernel, IO); //Consultar con Facu
  log_info(logger, "Contexto de ejecucion enviado!");
  log_info(logger, "Motivo del envio: IO");

  log_destroy(logger);
}

/*void verificar_existencia_recurso(LineaInstruccion *instruccion, int socketKernel)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  log_info(logger, "Solicitando a Kernel por la disponibilidad del recurso [%s]", instruccion->parametros[0]);
  t_paquete *paquete = crear_paquete();
  agregar_a_paquete(paquete, instruccion->parametros[0], strlen(instruccion->parametros[0]) + 1);
  enviar_paquete(paquete, socketKernel);
  log_info(logger, "Peticion del recurso [%s] enviada a Kernel!", instruccion->parametros[0]);

  log_info(logger, "Esperando respuesta de Kernel...");

  log_destroy(logger);
}*/

void ejecutar_yield(pcb *pcb, int socketKernel)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  log_info(logger, "Enviando el contexto de ejecucion del proceso [%d] a Kernel...", pcb->pid);
  enviar_contexto_ejecucion(pcb, socketKernel, YIELD);
  log_info(logger, "Contexto de ejecucion enviado!");
  log_info(logger, "Motivo del envio: YIELD");

  log_destroy(logger);
}

void ejecutar_exit(pcb *pcb, int socketKernel)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  log_info(logger, "Enviando el contexto de ejecucion del proceso [%d] a Kernel...", pcb->pid);
  enviar_contexto_ejecucion(pcb, socketKernel, EXIT); // Consultar con Facu
  log_info(logger, "Contexto de ejecucion enviado!");
  log_info(logger, "Motivo del envio: EXIT");

  log_destroy(logger);
}

void estado_de_los_registros(pcb *unPcb)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);

  log_info(logger, "ESTADO DE LOS REGISTROS");

  char* AX = valor_del_registro_como_string(unPcb->AX,4);   // valor_del_registro_como_string hace malloc, asi que hay que hacer un free despues de usar los valores
  char* BX = valor_del_registro_como_string(unPcb->BX,4);
  char* CX = valor_del_registro_como_string(unPcb->CX,4);
  char* DX = valor_del_registro_como_string(unPcb->DX,4);
  char* EAX = valor_del_registro_como_string(unPcb->EAX,8);
  char* EBX = valor_del_registro_como_string(unPcb->EBX,8);
  char* ECX = valor_del_registro_como_string(unPcb->ECX,8);
  char* EDX = valor_del_registro_como_string(unPcb->EDX,8);
  char* RAX = valor_del_registro_como_string(unPcb->RAX,16);
  char* RBX = valor_del_registro_como_string(unPcb->RBX,16);
  char* RCX = valor_del_registro_como_string(unPcb->RCX,16);
  char* RDX = valor_del_registro_como_string(unPcb->RDX,16);

  log_info(logger, "AX: %s | BX: %s | CX: %s | DX: %s", AX, BX, CX, DX);

  log_info(logger, "EAX: %s | EBX: %s | ECX: %s | EDX: %s", EAX, EBX, ECX, EDX);

  log_info(logger, "RAX: %s | RBX: %s | RCX: %s | RDX: %s", RAX, RBX, RCX, RDX);

  free(AX);
  free(BX);
  free(CX);
  free(DX);

  free(EAX);
  free(EBX);
  free(ECX);
  free(EDX);

  free(RAX);
  free(RBX);
  free(RCX);
  free(RDX);

  log_destroy(logger);
}

void logear_instruccion(int pid, LineaInstruccion *instruccion)
{
  Logger *logger = iniciar_logger_modulo(CPU_LOGGER);
  
  if(!strcmp(instruccion->parametros[0], "-1"))
  {
    log_info(logger, "< PID: %d | Nombre instruccion: %s >", pid, instruccion->identificador);
  }
  else if(!strcmp(instruccion->parametros[1], "-1"))
  {
    log_info(logger, "< PID: %d | Nombre instruccion: %s | Parametros: %s >", pid, instruccion->identificador, instruccion->parametros[0]);
  }
  else if(!strcmp(instruccion->parametros[2], "-1"))
  {
    log_info(logger, "< PID: %d | Nombre instruccion: %s | Parametros: %s, %s >",
    pid, instruccion->identificador, instruccion->parametros[0], instruccion->parametros[1]);
  }
  else
  {
    log_info(logger, "< PID: %d | Nombre instruccion: %s | Parametros: %s, %s, %s >",
     pid, instruccion->identificador, instruccion->parametros[0], instruccion->parametros[1], instruccion->parametros[2]);
  }
  
  log_destroy(logger);
}

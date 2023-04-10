#include <utils/PCB.h>


pcb* crear_pcb(t_list* lista_de_instrucciones, int p_id, int estimado_rafaga)
{
    pcb* proceso = malloc(sizeof(pcb));

    proceso->pid = p_id;
    proceso->estado = NEW;
    proceso->program_counter = 0;
    proceso->AX = "0000";             
    proceso->BX = "0000";
    proceso->CX = "0000";
    proceso->DX = "0000";
    proceso->EAX = "00000000";
    proceso->EBX = "00000000";
    proceso->ECX = "00000000";
    proceso->EDX = "00000000";
    proceso->RAX = "0000000000000000";
    proceso->RBX = "0000000000000000";
    proceso->RCX = "0000000000000000";
    proceso->RDX = "0000000000000000";
    
    proceso->estimado_prox_rafaga = estimado_rafaga;
    proceso->lista_de_instrucciones = list_duplicate(lista_de_instrucciones);
    proceso->tabla_de_segmentos = list_create();

    return proceso;
}

void loguear_pcb(pcb* un_pcb, t_log* logger)
{
    log_info(logger, "Lectura de proceso: ");
    log_info(logger, "PID: <%d> ", un_pcb->pid);
    log_info(logger, "Estado: <%s>",identificar_estado(un_pcb->estado));
    log_info(logger, "Program Counter: <%d> ", un_pcb->program_counter);
    log_info(logger, "Registros: ");
    log_info(logger, "AX = %s ", un_pcb->AX);
    log_info(logger, "BX = %s ", un_pcb->BX);
    log_info(logger, "CX = %s ", un_pcb->CX);
    log_info(logger, "DX = %s ", un_pcb->DX);
    log_info(logger, "EAX = %s ", un_pcb->EAX);
    log_info(logger, "EBX = %s ", un_pcb->EBX);
    log_info(logger, "ECX = %s ", un_pcb->ECX);
    log_info(logger, "EDX = %s ", un_pcb->EDX);
    log_info(logger, "RAX = %s ", un_pcb->RAX);
    log_info(logger, "RBX = %s ", un_pcb->RBX);
    log_info(logger, "RCX = %s ", un_pcb->RCX);
    log_info(logger, "RDX = %s ", un_pcb->RDX);
    log_info(logger, "Estimacion inicial: %d", un_pcb->estimado_prox_rafaga);
    
    loguear_lista_de_instrucciones(un_pcb->lista_de_instrucciones, logger);
    //loguear_tabla_de_segmentos(un_pcb->tabla_de_segmentos, logger);
}

char* identificar_estado(estado_pcb un_estado)
{
    switch(un_estado)
    {
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case RUNNING:
            return "RUNNING";
        case BLOCKED:
            return "BLOCKED";
        case TERMINATED:
            return "TERMINATED";
        default:
            return "NO SE RECONOCE EL ESTADO";
    }
}

void liberar_pcb(pcb* un_pcb)
{
    free(un_pcb->AX);
    free(un_pcb->BX);
    free(un_pcb->CX);
    free(un_pcb->DX);
    free(un_pcb->EAX);
    free(un_pcb->EBX);
    free(un_pcb->ECX);
    free(un_pcb->EDX);
    free(un_pcb->RAX);
    free(un_pcb->RBX);
    free(un_pcb->RCX);
    free(un_pcb->RDX);
    list_destroy_and_destroy_elements(un_pcb->lista_de_instrucciones, (void*)liberar_instruccion);
    //list_destroy_and_destroy_elements(un_pcb->tabla_de_segmentos, (void*)liberar_segmento);
    free(un_pcb);
    return NULL;
}

void loguear_lista_de_instrucciones(t_list* lista_de_instrucciones, t_log* logger)
{
    for(int i = 0; i < list_size(lista_de_instrucciones); i++)
    {
        LineaInstruccion* una_instruccion = list_get(lista_de_instrucciones, i);
        log_info(logger, "Instruccion N°<%d>: %s %s %s ", i+1, una_instruccion->identificador, una_instruccion->parametros[0], una_instruccion->parametros[1]);
    }
}

void* liberar_instruccion(LineaInstruccion* una_instruccion)
{
    free(una_instruccion->identificador);
    free(una_instruccion->parametros[0]);
    free(una_instruccion->parametros[1]);
    free(una_instruccion->parametros[2]);
    free(una_instruccion->parametros[3]);
    free(una_instruccion);
    return NULL;
}
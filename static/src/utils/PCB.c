#include <utils/PCB.h>


pcb* crear_pcb(t_list* lista_de_instrucciones, int p_id, int estimado_rafaga)
{
    pcb* proceso = malloc(sizeof(pcb));

    proceso->pid = p_id;
    proceso->estado = NEW;
    proceso->program_counter = 0;

    // inicializar registros
    memset(proceso->AX, 0 , 4);
    memset(proceso->BX, 1, 4);
    memset(proceso->CX, 2, 4);
    memset(proceso->DX, 3, 4);
    memset(proceso->EAX, 4, 8);
    memset(proceso->EBX, 5, 8);
    memset(proceso->ECX, 6, 8);
    memset(proceso->EDX, 7, 8);
    memset(proceso->RAX, 8, 16);
    memset(proceso->RBX, 9, 16);
    memset(proceso->RCX, 10, 16);
    memset(proceso->RDX, 11, 16);

    proceso->estimado_prox_rafaga = estimado_rafaga;
    proceso->tiempo_io = 0;
    proceso->lista_de_instrucciones = list_duplicate(lista_de_instrucciones);
    proceso->tabla_de_segmentos = list_create();
    proceso->tabla_archivos_abiertos = list_create();

    // Los t_temporal se crean cuando llegan a ready o a exec.

    return proceso;
}

char* valor_del_registro_como_string(void* registro, size_t tamanio) {
    char* valor_str = malloc(tamanio + 1);
    strncpy(valor_str, registro, tamanio);
    valor_str[tamanio] = '\0';
    return valor_str;
}

void loguear_pcb(pcb* un_pcb, t_log* logger)
{
    log_info(logger, "Lectura de proceso: ");
    log_info(logger, "PID: <%d> ", un_pcb->pid);
    log_info(logger, "Estado: <%s>",identificar_estado(un_pcb->estado));
    log_info(logger, "Program Counter: <%d> ", un_pcb->program_counter);
    log_info(logger, "Registros: ");

    // log de valores en hexadecimal
    log_info(logger, "AX = %08x ", *un_pcb->AX);
    log_info(logger, "BX = %08x ", *un_pcb->BX);
    log_info(logger, "CX = %08x ", *un_pcb->CX);
    log_info(logger, "DX = %08x ", *un_pcb->DX);
    log_info(logger, "EAX = %016llx ", *un_pcb->EAX);
    log_info(logger, "EBX = %016llx ", *un_pcb->EBX);
    log_info(logger, "ECX = %016llx ", *un_pcb->ECX);
    log_info(logger, "EDX = %016llx ", *un_pcb->EDX);
    log_info(logger, "RAX = %016llx ", *un_pcb->RAX);
    log_info(logger, "RBX = %016llx ", *un_pcb->RBX);
    log_info(logger, "RCX = %016llx ", *un_pcb->RCX); // debe imprimir 0x000000000000000a
    log_info(logger, "RDX = %016llx ", *un_pcb->RDX); // debe imprimir 0x000000000000000b

    // log de valores como string
    memcpy(un_pcb->AX, "ABCD", 4);
    memcpy(un_pcb->BX, "EFGH", 4);
    memcpy(un_pcb->CX, "IJKL", 4);
    memcpy(un_pcb->DX, "MNOP", 4);
    memcpy(un_pcb->EAX, "12345678", 8);
    memcpy(un_pcb->EBX, "abcdefgh", 8);
    memcpy(un_pcb->ECX, "ijklmnop", 8);
    memcpy(un_pcb->EDX, "qrstuvwx", 8);
    memcpy(un_pcb->RAX, "1234567890123456", 16);
    memcpy(un_pcb->RBX, "abcdefghijklmnop", 16);
    memcpy(un_pcb->RCX, "qrstuvwxyz123456", 16);
    memcpy(un_pcb->RDX, "7890123456789012", 16);
    
    char* ax_str = valor_del_registro_como_string(un_pcb->AX, 4);
    char* bx_str = valor_del_registro_como_string(un_pcb->BX, 4);
    char* cx_str = valor_del_registro_como_string(un_pcb->CX, 4);
    char* dx_str = valor_del_registro_como_string(un_pcb->DX, 4);
    char* eax_str = valor_del_registro_como_string(un_pcb->EAX, 8);
    char* ebx_str = valor_del_registro_como_string(un_pcb->EBX, 8);
    char* ecx_str = valor_del_registro_como_string(un_pcb->ECX, 8);
    char* edx_str = valor_del_registro_como_string(un_pcb->EDX, 8);
    char* rax_str = valor_del_registro_como_string(un_pcb->RAX, 16);
    char* rbx_str = valor_del_registro_como_string(un_pcb->RBX, 16);
    char* rcx_str = valor_del_registro_como_string(un_pcb->RCX, 16);
    char* rdx_str = valor_del_registro_como_string(un_pcb->RDX, 16);


    log_info(logger, "AX = %s ", ax_str);
    log_info(logger, "BX = %s ", bx_str);
    log_info(logger, "CX = %s ", cx_str);
    log_info(logger, "DX = %s ", dx_str);
    log_info(logger, "EAX = %s ", eax_str);
    log_info(logger, "EBX = %s ", ebx_str);
    log_info(logger, "ECX = %s ", ecx_str);
    log_info(logger, "EDX = %s ", edx_str);
    log_info(logger, "RAX = %s ", rax_str);
    log_info(logger, "RBX = %s ", rbx_str);
    log_info(logger, "RCX = %s ", rcx_str);
    log_info(logger, "RDX = %s ", rdx_str);

    free(ax_str);
    free(bx_str);
    free(cx_str);
    free(dx_str);
    free(eax_str);
    free(ebx_str);
    free(ecx_str);
    free(edx_str);
    free(rax_str);
    free(rbx_str);
    free(rcx_str);
    free(rdx_str);

    log_info(logger, "Estimacion proxima rafaga: %f", un_pcb->estimado_prox_rafaga);
    log_info(logger, "Tiempo IO: %d ", un_pcb->tiempo_io);

    loguear_lista_de_instrucciones(un_pcb->lista_de_instrucciones, logger);
    //loguear_tabla_de_segmentos(un_pcb->tabla_de_segmentos, logger);
    //loguear_tabla_archivos_abiertos(un_pcb->tabla_archivos_abiertos, logger);

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
    //list_destroy_and_destroy_elements(un_pcb->tabla_archivos_abiertos, (void*)liberar_archivo);

    //temporal_destroy(un_pcb->llegada_ready);      Estos temporals despues se destruyen
    //temporal_destroy(un_pcb->tiempo_ejecucion);

    free(un_pcb);
}
/*
void enviar_pcb(pcb* un_pcb, int socket_a_enviar)
{
    t_paquete* paquete_a_enviar = crear_paquete_operacion(PCB);

    serializar_pcb(paquete_a_enviar, un_pcb);

    enviar_paquete(paquete_a_enviar, socket_a_enviar);

    eliminar_paquete(paquete_a_enviar);
}

void serializar_pcb(t_paquete* paquete, pcb* un_pcb)
{
    int cantidad_de_instrucciones = list_size(un_pcb->lista_de_instrucciones);
    int cantidad_en_tabla_segmentos = list_size(un_pcb->tabla_de_segmentos);

    agregar_a_paquete(paquete, &cantidad_de_instrucciones, sizeof(int));
    agregar_a_paquete(paquete, &cantidad_en_tabla_segmentos, sizeof(int));

    int i = 0;

    for(i; i < cantidad_de_instrucciones; i++)
    {
        LineaInstruccion* una_instruccion = list_get(un_pcb->lista_de_instrucciones, i);
        agregar_a_paquete(paquete, una_instruccion->identificador, strlen(una_instruccion->identificador)+1);
        agregar_a_paquete(paquete, una_instruccion->parametros[0], strlen(una_instruccion->parametros[0])+1);
        agregar_a_paquete(paquete, una_instruccion->parametros[1], strlen(una_instruccion->parametros[1])+1);
        agregar_a_paquete(paquete, una_instruccion->parametros[2], strlen(una_instruccion->parametros[2])+1);
    }
    i = 0;

    //for(i; i < cantidad_en_tabla_segmentos; i++)
    //{
    //    segmento* un_segmento = list_get(un_pcb->tabla_de_segmentos,i);
    //    agregar_a_paquete(paquete, &un_segmento->tamanio_segmento, sizeof(int));
    //    agregar_a_paquete(paquete, un_segmento->id_tabla_pagina, strlen(un_segmento->id_tabla_pagina)+1);
    //}

    agregar_a_paquete(paquete, &(un_pcb->pid), sizeof(int));
    agregar_a_paquete(paquete, &(un_pcb->program_counter), sizeof(int));

    agregar_a_paquete(paquete, un_pcb->AX, strlen(un_pcb->AX));
    agregar_a_paquete(paquete, un_pcb->BX, strlen(un_pcb->BX));
    agregar_a_paquete(paquete, un_pcb->CX, strlen(un_pcb->CX));
    agregar_a_paquete(paquete, un_pcb->DX, strlen(un_pcb->DX));

    agregar_a_paquete(paquete, un_pcb->EAX, strlen(un_pcb->EAX));
    agregar_a_paquete(paquete, un_pcb->EBX, strlen(un_pcb->EBX));
    agregar_a_paquete(paquete, un_pcb->ECX, strlen(un_pcb->ECX));
    agregar_a_paquete(paquete, un_pcb->EDX, strlen(un_pcb->EDX));

    agregar_a_paquete(paquete, un_pcb->RAX, strlen(un_pcb->RAX));
    agregar_a_paquete(paquete, un_pcb->RBX, strlen(un_pcb->RBX));
    agregar_a_paquete(paquete, un_pcb->RCX, strlen(un_pcb->RCX));
    agregar_a_paquete(paquete, un_pcb->RDX, strlen(un_pcb->RDX));

    agregar_a_paquete(paquete, &(un_pcb->estado), sizeof(estado_pcb));
    agregar_a_paquete(paquete, &(un_pcb->estimado_prox_rafaga), sizeof(int));

}

pcb* recibir_pcb(t_list* valores_pcb_enviado)
{
    int cantidad_de_instrucciones = *(int *)list_get(valores_pcb_enviado, 0);
    int cantidad_en_tabla_segmentos = *(int *)list_get(valores_pcb_enviado, 1);

    pcb* pcb_recibido = //(pcb*) malloc(sizeof(pcb));
    t_list* lista_de_instrucciones = list_create();
    t_list* tabla_de_segmentos = list_create();
    int i;
    int base;   // Representa el desplazamiento a traves de la lista para ubicar los valores empaquetados en el orden predefinido

    for(i = 0; i < cantidad_de_instrucciones; i++)
    {
        base = 4 * i;
        LineaInstruccion* una_instruccion = malloc(sizeof(LineaInstruccion));

        una_instruccion->identificador = string_duplicate(list_get(valores_pcb_enviado, base + 2));
        una_instruccion->parametros[0] = string_duplicate(list_get(valores_pcb_enviado, base + 3));
        una_instruccion->parametros[1] = string_duplicate(list_get(valores_pcb_enviado, base + 4));
        una_instruccion->parametros[2] = string_duplicate(list_get(valores_pcb_enviado, base + 5));

        list_add(lista_de_instrucciones, una_instruccion);
    }

    base = (cantidad_de_instrucciones * 4) + 2;

    //for(i = 0; i < cantidad_en_tabla_segmentos; i++)
    //{
    //    segmento* un_segmento = malloc(sizeof(segmento));

    //    un_segmento->tamanio_segmento = *(int*)list_get(valores_pcb_enviado, base);
    //    base++;
    //    un_segmento->id_tabla_pagina = string_duplicate(list_get(valores_pcb_enviado, base));
    //    base++;

    //    list_add(tabla_de_segmentos, un_segmento);
    //}

    int valores_restantes = base;

    pcb_recibido->pid = *(int*)list_get(valores_pcb_enviado, valores_restantes);
    pcb_recibido->program_counter = *(int*)list_get(valores_pcb_enviado, valores_restantes + 1);

    pcb_recibido->AX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 2));
    pcb_recibido->BX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 3));
    pcb_recibido->CX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 4));
    pcb_recibido->DX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 5));

    pcb_recibido->EAX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 6));
    pcb_recibido->EBX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 7));
    pcb_recibido->ECX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 8));
    pcb_recibido->EDX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 9));

    pcb_recibido->RAX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 10));
    pcb_recibido->RBX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 11));
    pcb_recibido->RCX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 12));
    pcb_recibido->RDX = string_duplicate(list_get(valores_pcb_enviado, valores_restantes + 13));

    pcb_recibido->estado = *(estado_pcb*)list_get(valores_pcb_enviado, valores_restantes + 14);
    pcb_recibido->estimado_prox_rafaga = *(int*)list_get(valores_pcb_enviado, valores_restantes + 15);

    pcb_recibido->lista_de_instrucciones = list_duplicate(lista_de_instrucciones);

    //pcb_recibido->tabla_de_segmentos = list_duplicate(tabla_de_segmentos);

    list_destroy(lista_de_instrucciones);
    //list_destroy(tabla_de_segmentos);

    //list_destroy(valores_pcb_enviado);

    return pcb_recibido;
}*/

// ACLARACION: El codigo es basicamente la intencion con la que se envia el contexto de ejecucion.
// Es decir, si yo pongo: CONTEXTO_EJECUCION, es kernel -> cpu para que pueda ejecutar
// El resto de codigos seran las instrucciones que lea el cpu y que deba devolver el contexto de ejecucion.
// Por ejemplo: IO, EXIT, YIELD. En todas estas, se devuelve el contexto de ejecucion al kernel, pero por diferentes motivos.
void enviar_contexto_ejecucion(pcb* un_pcb, int socket_a_enviar, int codigo)
{
    t_paquete* paquete_a_enviar = crear_paquete_operacion(codigo);

    serializar_contexto_ejecucion(paquete_a_enviar, un_pcb);

    enviar_paquete(paquete_a_enviar, socket_a_enviar);

    eliminar_paquete(paquete_a_enviar);
}

void serializar_contexto_ejecucion(t_paquete* paquete, pcb* un_pcb)
{
    int cantidad_de_instrucciones = list_size(un_pcb->lista_de_instrucciones);
    agregar_a_paquete(paquete, &cantidad_de_instrucciones, sizeof(int));

    int i = 0;
    for(i; i < cantidad_de_instrucciones; i++)
    {
        LineaInstruccion* una_instruccion = list_get(un_pcb->lista_de_instrucciones, i);
        agregar_a_paquete(paquete, una_instruccion->identificador, strlen(una_instruccion->identificador)+1);
        agregar_a_paquete(paquete, una_instruccion->parametros[0], strlen(una_instruccion->parametros[0])+1);
        agregar_a_paquete(paquete, una_instruccion->parametros[1], strlen(una_instruccion->parametros[1])+1);
        agregar_a_paquete(paquete, una_instruccion->parametros[2], strlen(una_instruccion->parametros[2])+1);
    }

    agregar_a_paquete(paquete, &(un_pcb->pid), sizeof(int));
    agregar_a_paquete(paquete, &(un_pcb->program_counter), sizeof(int));

    agregar_a_paquete(paquete, un_pcb->AX, strlen(un_pcb->AX));
    agregar_a_paquete(paquete, un_pcb->BX, strlen(un_pcb->BX));
    agregar_a_paquete(paquete, un_pcb->CX, strlen(un_pcb->CX));
    agregar_a_paquete(paquete, un_pcb->DX, strlen(un_pcb->DX));

    agregar_a_paquete(paquete, un_pcb->EAX, strlen(un_pcb->EAX));
    agregar_a_paquete(paquete, un_pcb->EBX, strlen(un_pcb->EBX));
    agregar_a_paquete(paquete, un_pcb->ECX, strlen(un_pcb->ECX));
    agregar_a_paquete(paquete, un_pcb->EDX, strlen(un_pcb->EDX));

    agregar_a_paquete(paquete, un_pcb->RAX, strlen(un_pcb->RAX));
    agregar_a_paquete(paquete, un_pcb->RBX, strlen(un_pcb->RBX));
    agregar_a_paquete(paquete, un_pcb->RCX, strlen(un_pcb->RCX));
    agregar_a_paquete(paquete, un_pcb->RDX, strlen(un_pcb->RDX));

    agregar_a_paquete(paquete, &(un_pcb->tiempo_io), sizeof(int));
}

pcb* recibir_contexto_ejecucion(t_list* valores_contexto_enviado)
{
    int cantidad_de_instrucciones = *(int *)list_get(valores_contexto_enviado, 0);

    pcb* contexto_recibido = /*(pcb*) */malloc(sizeof(pcb));
    t_list* lista_de_instrucciones = list_create();
    int i;
    int base;   // Representa el desplazamiento a traves de la lista para ubicar los valores empaquetados en el orden predefinido

    for(i = 0; i < cantidad_de_instrucciones; i++)
    {
        base = 4 * i;
        LineaInstruccion* una_instruccion = malloc(sizeof(LineaInstruccion));

        una_instruccion->identificador = string_duplicate(list_get(valores_contexto_enviado, base + 1));
        una_instruccion->parametros[0] = string_duplicate(list_get(valores_contexto_enviado, base + 2));
        una_instruccion->parametros[1] = string_duplicate(list_get(valores_contexto_enviado, base + 3));
        una_instruccion->parametros[2] = string_duplicate(list_get(valores_contexto_enviado, base + 4));

        list_add(lista_de_instrucciones, una_instruccion);
    }

    base = (cantidad_de_instrucciones * 4) + 1;
    int valores_restantes = base;

    contexto_recibido->pid = *(int*)list_get(valores_contexto_enviado, valores_restantes);
    contexto_recibido->program_counter = *(int*)list_get(valores_contexto_enviado, valores_restantes + 1);

    memcpy(contexto_recibido->AX, list_get(valores_contexto_enviado, valores_restantes + 2), 4);
    memcpy(contexto_recibido->BX, list_get(valores_contexto_enviado, valores_restantes + 3), 4);
    memcpy(contexto_recibido->CX, list_get(valores_contexto_enviado, valores_restantes + 4), 4);
    memcpy(contexto_recibido->DX, list_get(valores_contexto_enviado, valores_restantes + 5), 4);

    memcpy(contexto_recibido->EAX, list_get(valores_contexto_enviado, valores_restantes + 6), 8);
    memcpy(contexto_recibido->EBX, list_get(valores_contexto_enviado, valores_restantes + 7), 8);
    memcpy(contexto_recibido->ECX, list_get(valores_contexto_enviado, valores_restantes + 8), 8);
    memcpy(contexto_recibido->EDX, list_get(valores_contexto_enviado, valores_restantes + 9), 8);

    memcpy(contexto_recibido->RAX, list_get(valores_contexto_enviado, valores_restantes + 10), 16);
    memcpy(contexto_recibido->RBX, list_get(valores_contexto_enviado, valores_restantes + 11), 16);
    memcpy(contexto_recibido->RCX, list_get(valores_contexto_enviado, valores_restantes + 12), 16);
    memcpy(contexto_recibido->RDX, list_get(valores_contexto_enviado, valores_restantes + 13), 16);

    contexto_recibido->tiempo_io = *(int*)list_get(valores_contexto_enviado, valores_restantes + 14);

    contexto_recibido->lista_de_instrucciones = list_duplicate(lista_de_instrucciones);

    list_destroy(lista_de_instrucciones);

    return contexto_recibido;
}

void actualizar_contexto_ejecucion(pcb* un_pcb, pcb* otro_pcb)
{
    un_pcb->program_counter = otro_pcb->program_counter;

    memcpy(un_pcb->AX, otro_pcb->AX, 4);
    memcpy(un_pcb->BX, otro_pcb->BX, 4);
    memcpy(un_pcb->CX, otro_pcb->CX, 4);
    memcpy(un_pcb->DX, otro_pcb->DX, 4);

    memcpy(un_pcb->EAX, otro_pcb->EAX, 8);
    memcpy(un_pcb->EBX, otro_pcb->EBX, 8);
    memcpy(un_pcb->ECX, otro_pcb->ECX, 8);
    memcpy(un_pcb->EDX, otro_pcb->EDX, 8);

    memcpy(un_pcb->RAX, otro_pcb->RAX, 16);
    memcpy(un_pcb->RBX, otro_pcb->RBX, 16);
    memcpy(un_pcb->RCX, otro_pcb->RCX, 16);
    memcpy(un_pcb->RDX, otro_pcb->RDX, 16);

    un_pcb->tiempo_io = otro_pcb->tiempo_io;
}

void loguear_lista_de_instrucciones(t_list* lista_de_instrucciones, t_log* logger)
{
    for(int i = 0; i < list_size(lista_de_instrucciones); i++)
    {
        LineaInstruccion* una_instruccion = list_get(lista_de_instrucciones, i);
        log_info(logger, "Instruccion N°<%d>: %s %s %s %s ", i+1, una_instruccion->identificador, una_instruccion->parametros[0], una_instruccion->parametros[1], una_instruccion->parametros[2]);
    }
}

void* liberar_instruccion(LineaInstruccion* una_instruccion)
{
    free(una_instruccion->identificador);
    free(una_instruccion->parametros[0]);
    free(una_instruccion->parametros[1]);
    free(una_instruccion->parametros[2]);
    free(una_instruccion);
    return NULL;
}

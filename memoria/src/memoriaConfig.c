#include <memoriaConfig.h>

config_de_memoria configuracionMemoria;

config_de_memoria obtener_valores_de_configuracion_memoria(Config* memoriaConfig){
    configuracionMemoria.PUERTO_ESCUCHA = config_get_string_value(memoriaConfig, "PUERTO_ESCUCHA");
    configuracionMemoria.TAM_MEMORIA = config_get_int_value(memoriaConfig, "TAM_MEMORIA");
    configuracionMemoria.TAM_SEGMENTO_0 = config_get_int_value(memoriaConfig,"TAM_SEGMENTO_0");
    configuracionMemoria.CANT_SEGMENTOS = config_get_int_value(memoriaConfig,"CANT_SEGMENTOS");
    configuracionMemoria.RETARDO_MEMORIA = config_get_int_value(memoriaConfig,"RETARDO_MEMORIA");
    configuracionMemoria.RETARDO_COMPACTACION = config_get_int_value(memoriaConfig,"RETARDO_COMPACTACION");
    configuracionMemoria.ALGORITMO_ASIGNACION = config_get_string_value(memoriaConfig,"ALGORITMO_ASIGNACION");
    return configuracionMemoria;
}

void mostrar_valores_de_configuracion_memoria (config_de_memoria configuracionMemoria){
    printf("PUERTO_ESCUCHA = %s\n", configuracionMemoria.PUERTO_ESCUCHA);
    printf("TAM_MEMORIA = %d\n" , configuracionMemoria.TAM_MEMORIA);
    printf("TAM_SEGMENTO_0 = %d\n" , configuracionMemoria.TAM_SEGMENTO_0);
    printf("CANT_SEGMENTOS = %d\n" , configuracionMemoria.CANT_SEGMENTOS);
    printf("RETARDO_MEMORIA = %d\n" , configuracionMemoria.RETARDO_MEMORIA);
    printf("RETARDO_COMPACTACION = %d\n", configuracionMemoria.RETARDO_COMPACTACION);
    printf("ALGORITMO_ASIGNACION = %s\n" , configuracionMemoria.ALGORITMO_ASIGNACION);
}
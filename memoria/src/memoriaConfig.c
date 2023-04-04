#include <memoriaConfig.h>

config_de_memoria configuracion_memoria;

config_de_memoria obtener_valores_de_configuracion_memoria(t_config* memoria_config){
    configuracion_memoria.PUERTO_ESCUCHA = config_get_string_value(memoria_config, "PUERTO_ESCUCHA");
    configuracion_memoria.TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
    configuracion_memoria.TAM_SEGMENTO_0 = config_get_int_value(memoria_config,"TAM_SEGMENTO_0");
    configuracion_memoria.CANT_SEGMENTOS = config_get_int_value(memoria_config,"CANT_SEGMENTOS");
    configuracion_memoria.RETARDO_MEMORIA = config_get_int_value(memoria_config,"RETARDO_MEMORIA");
    configuracion_memoria.RETARDO_COMPACTACION = config_get_int_value(memoria_config,"RETARDO_COMPACTACION");
    configuracion_memoria.ALGORITMO_ASIGNACION = config_get_string_value(memoria_config,"ALGORITMO_ASIGNACION");
    return configuracion_memoria;
}

void mostrar_valores_de_configuracion_memoria (config_de_memoria configuracion_memoria){
    printf("PUERTO_ESCUCHA = %s\n", configuracion_memoria.PUERTO_ESCUCHA);
    printf("TAM_MEMORIA = %d\n" , configuracion_memoria.TAM_MEMORIA);
    printf("TAM_SEGMENTO_0 = %d\n" , configuracion_memoria.TAM_SEGMENTO_0);
    printf("CANT_SEGMENTOS = %d\n" , configuracion_memoria.CANT_SEGMENTOS);
    printf("RETARDO_MEMORIA = %d\n" , configuracion_memoria.RETARDO_MEMORIA);
    printf("RETARDO_COMPACTACION = %d\n", configuracion_memoria.RETARDO_COMPACTACION);
    printf("ALGORITMO_ASIGNACION = %s\n" , configuracion_memoria.ALGORITMO_ASIGNACION);
}
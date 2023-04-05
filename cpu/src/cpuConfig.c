#include "cpuConfig.h"

config_de_cpu configuracion_cpu;

config_de_cpu obtener_valores_de_configuracion_cpu(t_config* cpu_config){
    configuracion_cpu.RETARDO_INSTRUCCION= config_get_int_value(cpu_config, "RETARDO_INSTRUCCION");
    configuracion_cpu.IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
    configuracion_cpu.PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
    configuracion_cpu.PUERTO_ESCUCHA = config_get_string_value(cpu_config, "PUERTO_ESCUCHA");
    configuracion_cpu.TAM_MAX_SEGMENTO = config_get_int_value(cpu_config, "TAM_MAX_SEGMENTO");
    return configuracion_cpu;
}

void mostrar_valores_de_configuracion_cpu(config_de_cpu configuracion_cpu){
    printf("RETARDO_INSTRUCCION = %d\n", configuracion_cpu.RETARDO_INSTRUCCION);
    printf("IP_MEMORIA = %s\n", configuracion_cpu.IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", configuracion_cpu.PUERTO_MEMORIA);
    printf("PUERTO_ESCUCHA = %s\n", configuracion_cpu.PUERTO_ESCUCHA);
    printf("TAM_MAX_SEGMENTO = %d\n", configuracion_cpu.TAM_MAX_SEGMENTO);
}
#include "cpuConfig.h"

config_de_cpu configuracionCPU;

config_de_cpu obtener_valores_de_configuracion_cpu(t_config* cpuConfig){
    configuracionCPU.RETARDO_INSTRUCCION= config_get_int_value(cpuConfig, "RETARDO_INSTRUCCION");
    configuracionCPU.IP_MEMORIA = config_get_string_value(cpuConfig, "IP_MEMORIA");
    configuracionCPU.PUERTO_MEMORIA = config_get_string_value(cpuConfig, "PUERTO_MEMORIA");
    configuracionCPU.PUERTO_ESCUCHA = config_get_string_value(cpuConfig, "PUERTO_ESCUCHA");
    configuracionCPU.TAM_MAX_SEGMENTO = config_get_int_value(cpuConfig, "TAM_MAX_SEGMENTO");
    configuracionCPU.IP_SERVER = string_duplicate("127.0.0.1");
    return configuracionCPU;
}

void mostrar_valores_de_configuracion_cpu(config_de_cpu configuracionCPU){
    printf("IP_SERVER = %s\n", configuracionCPU.IP_SERVER);
    printf("RETARDO_INSTRUCCION = %d\n", configuracionCPU.RETARDO_INSTRUCCION);
    printf("IP_MEMORIA = %s\n", configuracionCPU.IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", configuracionCPU.PUERTO_MEMORIA);
    printf("PUERTO_ESCUCHA = %s\n", configuracionCPU.PUERTO_ESCUCHA);
    printf("TAM_MAX_SEGMENTO = %d\n", configuracionCPU.TAM_MAX_SEGMENTO);
}
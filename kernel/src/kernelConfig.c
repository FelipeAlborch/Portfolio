#include "kernelConfig.h"

config_de_kernel configuracionKernel;

config_de_kernel obtener_valores_de_configuracion_kernel(Config* kernelConfig){
    configuracionKernel.IP_MEMORIA = config_get_string_value(kernelConfig,"IP_MEMORIA");
    configuracionKernel.PUERTO_MEMORIA = config_get_string_value(kernelConfig,"PUERTO_MEMORIA");
    configuracionKernel.IP_FILESYSTEM = config_get_string_value(kernelConfig, "IP_FILESYSTEM");
    configuracionKernel.PUERTO_FILESYSTEM = config_get_string_value(kernelConfig, "PUERTO_FILESYSTEM");
    configuracionKernel.IP_CPU = config_get_string_value(kernelConfig,"IP_CPU");
    configuracionKernel.PUERTO_CPU = config_get_string_value(kernelConfig,"PUERTO_CPU");
    configuracionKernel.PUERTO_ESCUCHA = config_get_string_value(kernelConfig,"PUERTO_ESCUCHA");
    configuracionKernel.ALGORITMO_PLANIFICACION = config_get_string_value(kernelConfig,"ALGORITMO_PLANIFICACION");
    configuracionKernel.ESTIMACION_INICIAL = config_get_int_value(kernelConfig,"ESTIMACION_INICIAL");
    configuracionKernel.HRRN_ALFA = config_get_double_value(kernelConfig,"HRRN_ALFA");
    configuracionKernel.GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(kernelConfig,"GRADO_MAX_MULTIPROGRAMACION");
    configuracionKernel.RECURSOS = config_get_array_value(kernelConfig,"RECURSOS");
    configuracionKernel.INSTANCIAS_RECURSOS = config_get_array_value(kernelConfig,"INSTANCIAS_RECURSOS");
    return configuracionKernel;
}
void mostrar_valores_de_configuracion_kernel(config_de_kernel configuracion_kernel){
    printf("IP_MEMORIA = %s\n", configuracionKernel.IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", configuracionKernel.PUERTO_MEMORIA);
    printf("IP_FILESYSTEM = %s\n", configuracionKernel.IP_FILESYSTEM);
    printf("PUERTO_FILESYSTEM = %s\n", configuracionKernel.PUERTO_FILESYSTEM);
    printf("IP_CPU = %s\n", configuracionKernel.IP_CPU);
    printf("PUERTO_CPU = %s\n", configuracionKernel.PUERTO_CPU);
    printf("PUERTO_ESCUCHA = %s\n", configuracionKernel.PUERTO_ESCUCHA);
    printf("ALGORITMO_PLANIFICACION = %s\n", configuracionKernel.ALGORITMO_PLANIFICACION);
    printf("ESTIMACION_INICIAL = %d\n", configuracionKernel.ESTIMACION_INICIAL);
    printf("HRRN_ALFA = %f\n", configuracionKernel.HRRN_ALFA);
    printf("GRADO_MAX_MULTIPROGRAMACION = %d\n", configuracionKernel.GRADO_MAX_MULTIPROGRAMACION);
    printf("RECURSOS = %s\n", configuracionKernel.RECURSOS[0]);                                    // SOLO IMPRIMO UNO PARA VER SI SE TOMO BIEN
    printf("INSTANCIAS_RECURSOS = %s\n", configuracionKernel.INSTANCIAS_RECURSOS[0]);              // SOLO IMPRIMO UNO PARA VER SI SE TOMO BIEN
}
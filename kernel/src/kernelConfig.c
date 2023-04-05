#include "kernelConfig.h"

config_de_kernel configuracion_kernel;

config_de_kernel obtener_valores_de_configuracion_kernel(t_config* kernelConfig){
    configuracion_kernel.IP_MEMORIA = config_get_string_value(kernelConfig,"IP_MEMORIA");
    configuracion_kernel.PUERTO_MEMORIA = config_get_string_value(kernelConfig,"PUERTO_MEMORIA");
    configuracion_kernel.IP_FILESYSTEM = config_get_string_value(kernelConfig, "IP_FILESYSTEM");
    configuracion_kernel.PUERTO_FILESYSTEM = config_get_string_value(kernelConfig, "PUERTO_FILESYSTEM");
    configuracion_kernel.IP_CPU = config_get_string_value(kernelConfig,"IP_CPU");
    configuracion_kernel.PUERTO_CPU = config_get_string_value(kernelConfig,"PUERTO_CPU");
    configuracion_kernel.PUERTO_ESCUCHA = config_get_string_value(kernelConfig,"PUERTO_ESCUCHA");
    configuracion_kernel.ALGORITMO_PLANIFICACION = config_get_string_value(kernelConfig,"ALGORITMO_PLANIFICACION");
    configuracion_kernel.ESTIMACION_INICIAL = config_get_int_value(kernelConfig,"ESTIMACION_INICIAL");
    configuracion_kernel.HRRN_ALFA = config_get_double_value(kernelConfig,"HRRN_ALFA");
    configuracion_kernel.GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(kernelConfig,"GRADO_MAX_MULTIPROGRAMACION");
    configuracion_kernel.RECURSOS = config_get_array_value(kernelConfig,"RECURSOS");
    configuracion_kernel.INSTANCIAS_RECURSOS = config_get_array_value(kernelConfig,"INSTANCIAS_RECURSOS");
    return configuracion_kernel;
}
void mostrar_valores_de_configuracion_kernel(config_de_kernel configuracion_kernel){
    printf("IP_MEMORIA = %s\n", configuracion_kernel.IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", configuracion_kernel.PUERTO_MEMORIA);
    printf("IP_FILESYSTEM = %s\n", configuracion_kernel.IP_FILESYSTEM);
    printf("PUERTO_FILESYSTEM = %s\n", configuracion_kernel.PUERTO_FILESYSTEM);
    printf("IP_CPU = %s\n", configuracion_kernel.IP_CPU);
    printf("PUERTO_CPU = %s\n", configuracion_kernel.PUERTO_CPU);
    printf("PUERTO_ESCUCHA = %s\n", configuracion_kernel.PUERTO_ESCUCHA);
    printf("ALGORITMO_PLANIFICACION = %s\n", configuracion_kernel.ALGORITMO_PLANIFICACION);
    printf("ESTIMACION_INICIAL = %d\n", configuracion_kernel.ESTIMACION_INICIAL);
    printf("HRRN_ALFA = %f\n", configuracion_kernel.HRRN_ALFA);
    printf("GRADO_MAX_MULTIPROGRAMACION = %d\n", configuracion_kernel.GRADO_MAX_MULTIPROGRAMACION);
    printf("RECURSOS = %s\n", configuracion_kernel.RECURSOS[0]);                                    // SOLO IMPRIMO UNO PARA VER SI SE TOMO BIEN
    printf("INSTANCIAS_RECURSOS = %s\n", configuracion_kernel.INSTANCIAS_RECURSOS[0]);              // SOLO IMPRIMO UNO PARA VER SI SE TOMO BIEN
}
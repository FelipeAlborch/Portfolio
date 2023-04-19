#include "kernelConfig.h"

config_de_kernel configuracionKernel;
t_dictionary* diccionario_recursos;

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
    crear_diccionario_recursos();
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
    //printf("RECURSOS = %s\n", configuracionKernel.RECURSOS[0]);                                    // SOLO IMPRIMO UNO PARA VER SI SE TOMO BIEN
    //printf("INSTANCIAS_RECURSOS = %s\n", configuracionKernel.INSTANCIAS_RECURSOS[0]);              // SOLO IMPRIMO UNO PARA VER SI SE TOMO BIEN
    //for(int i = 0; i < string_array_size(configuracionKernel.RECURSOS); i++)
    //{
    //    printf("RECURSO = %s, INSTANCIAS = %s \n", configuracionKernel.RECURSOS[i], configuracion_kernel.INSTANCIAS_RECURSOS[i]);
    //}
}

void crear_diccionario_recursos()
{
    diccionario_recursos = dictionary_create();

    for(int i = 0; i < string_array_size(configuracionKernel.RECURSOS); i++)
    {
        char* key = configuracionKernel.RECURSOS[i];
        int instancias = atoi(configuracionKernel.INSTANCIAS_RECURSOS[i]);
        recurso* nuevo_recurso = crear_recurso(key, instancias);
        dictionary_put(diccionario_recursos,key, nuevo_recurso);
    }
}

void leer_diccionario_recursos()
{
    for(int i = 0; i < dictionary_size(diccionario_recursos); i++)
    {   
        char* key = configuracionKernel.RECURSOS[i];
        recurso* un_recurso = dictionary_get(diccionario_recursos, (void*) key);
        
        printf("Recurso: %s, ",un_recurso->nombre);
        printf("Instancias: %d \n",un_recurso->instancias);
        printf("Cant procesos bloqueados por el recurso: %d \n",queue_size(un_recurso->cola_bloqueados));

    }
}

recurso* crear_recurso(char* nombre, int instancias)
{
    recurso* nuevo_recurso = malloc(sizeof(recurso));
    
    nuevo_recurso->nombre = (char*) malloc(strlen(nombre) + 1);
    strcpy(nuevo_recurso->nombre, nombre);
    
    nuevo_recurso->instancias = instancias;
    
    nuevo_recurso->cola_bloqueados = queue_create();
    
    pthread_mutex_init(&(nuevo_recurso->mutex_cola), NULL);

    return nuevo_recurso;
}


void liberar_recurso(recurso* un_recurso)
{
    free(un_recurso->nombre);
    queue_destroy_and_destroy_elements(un_recurso->cola_bloqueados, (void*)liberar_pcb);
    pthread_mutex_destroy(&(un_recurso->mutex_cola));
    free(un_recurso);
}
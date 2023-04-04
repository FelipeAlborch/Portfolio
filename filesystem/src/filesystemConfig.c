#include <filesystemConfig.h>

config_de_filesystem configuracion_filesystem;

config_de_filesystem obtener_valores_de_configuracion_filesystem(t_config* filesystem_config){
    configuracion_filesystem.IP_MEMORIA = config_get_string_value(filesystem_config, "IP_MEMORIA");
    configuracion_filesystem.PUERTO_MEMORIA = config_get_string_value(filesystem_config, "PUERTO_MEMORIA");
    configuracion_filesystem.PUERTO_ESCUCHA = config_get_string_value(filesystem_config, "PUERTO_ESCUCHA");
    configuracion_filesystem.PATH_SUPERBLOQUE = config_get_string_value(filesystem_config, "PATH_SUPERBLOQUE");
    configuracion_filesystem.PATH_BITMAP = config_get_string_value(filesystem_config,"PATH_BITMAP");
    configuracion_filesystem.PATH_BLOQUES = config_get_string_value(filesystem_config,"PATH_BLOQUES");
    configuracion_filesystem.PATH_FCB = config_get_string_value(filesystem_config,"PATH_FCB");
    configuracion_filesystem.RETARDO_ACCESO_BLOQUE = config_get_int_value(filesystem_config,"RETARDO_ACCESO_BLOQUE");
    
    return configuracion_filesystem;
}

void mostrar_valores_de_configuracion_filesystem (config_de_filesystem configuracion_filesystem){
    printf("IP_MEMORIA = %s\n", configuracion_filesystem.IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", configuracion_filesystem.PUERTO_MEMORIA);
    printf("PUERTO_ESCUCHA = %s\n", configuracion_filesystem.PUERTO_ESCUCHA);
    printf("PATH_SUPERBLOQUE = %s\n", configuracion_filesystem.PATH_SUPERBLOQUE);
    printf("PATH_BITMAP = %s\n", configuracion_filesystem.PATH_BITMAP);
    printf("PATH_BLOQUES = %s\n", configuracion_filesystem.PATH_BLOQUES);
    printf("PATH_FCB = %s\n", configuracion_filesystem.PATH_FCB);
    printf("RETARDO_ACCESO_BLOQUE = %d\n", configuracion_filesystem.RETARDO_ACCESO_BLOQUE);
}
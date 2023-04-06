#include <filesystemConfig.h>

configDeFileSystem configuracionFileSystem;

config_de_filesystem obtener_valores_de_configuracion_filesystem(Config* filesystemConfig){
    configuracionFileSystem.IP_MEMORIA = config_get_string_value(filesystemConfig, "IP_MEMORIA");
    configuracionFileSystem.PUERTO_MEMORIA = config_get_string_value(filesystemConfig, "PUERTO_MEMORIA");
    configuracionFileSystem.PUERTO_ESCUCHA = config_get_string_value(filesystemConfig, "PUERTO_ESCUCHA");
    configuracionFileSystem.PATH_SUPERBLOQUE = config_get_string_value(filesystemConfig, "PATH_SUPERBLOQUE");
    configuracionFileSystem.PATH_BITMAP = config_get_string_value(filesystemConfig,"PATH_BITMAP");
    configuracionFileSystem.PATH_BLOQUES = config_get_string_value(filesystemConfig,"PATH_BLOQUES");
    configuracionFileSystem.PATH_FCB = config_get_string_value(filesystemConfig,"PATH_FCB");
    configuracionFileSystem.RETARDO_ACCESO_BLOQUE = config_get_int_value(filesystemConfig,"RETARDO_ACCESO_BLOQUE");
    
    return configuracionFilesystem;
}

void mostrar_valores_de_configuracion_filesystem (configDeFileSystem configuracionFileSystem){
    printf("IP_MEMORIA = %s\n", configuracionFileSystem.IP_MEMORIA);
    printf("PUERTO_MEMORIA = %s\n", configuracionFileSystem.PUERTO_MEMORIA);
    printf("PUERTO_ESCUCHA = %s\n", configuracionFileSystem.PUERTO_ESCUCHA);
    printf("PATH_SUPERBLOQUE = %s\n", configuracionFileSystem.PATH_SUPERBLOQUE);
    printf("PATH_BITMAP = %s\n", configuracionFileSystem.PATH_BITMAP);
    printf("PATH_BLOQUES = %s\n", configuracionFileSystem.PATH_BLOQUES);
    printf("PATH_FCB = %s\n", configuracionFileSystem.PATH_FCB);
    printf("RETARDO_ACCESO_BLOQUE = %d\n", configuracionFileSystem.RETARDO_ACCESO_BLOQUE);
}
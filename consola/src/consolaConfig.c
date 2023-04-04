#include <consolaConfig.h>

int prueba = 2;

config_de_consola configuracion_consola;

config_de_consola obtener_valores_de_configuracion_consola(t_config* consolaConfig)
{
    configuracion_consola.IP_KERNEL = config_get_string_value(consolaConfig,"IP_KERNEL");
    configuracion_consola.PUERTO_KERNEL = config_get_string_value(consolaConfig,"PUERTO_KERNEL");

    return configuracion_consola;
}

void mostrar_valores_de_configuracion_consola(config_de_consola configuracion_consola)
{
    printf("IP_KERNEL = %s\n", configuracion_consola.IP_KERNEL);
    printf("PUERTO_KERNEL = %s\n", configuracion_consola.PUERTO_KERNEL);
}
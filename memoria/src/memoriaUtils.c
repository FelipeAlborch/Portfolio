#include <memoriaUtils.h>

void terminar_programa(int conexion,t_log* logger, t_config* memoria){
    /*LEAKS*/
	liberar_memoria();
	liberar_listas();
	liberar_conexion_memoria(conexion);
	log_destroy(logger);
    liberar_t_config();
	config_destroy(memoria);
	printf("----------FIN------------\n");
};
void liberar_memoria(){
    /* TO DO */
};
void liberar_listas(){
    /* TO DO */
};
void liberar_conexion_memoria(int conexion){
    /* TO DO */
};
void liberar_t_config(){
    /* TO DO */
};
void inicializar_configuracion(){
    /* TO DO */
};

void inicializar_memoria(){
    /* TO DO */
};
void inicializar_segmentos(){
    /* TO DO */
};	
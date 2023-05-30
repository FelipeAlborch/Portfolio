#include <test.h>

int main(int argc, char** argv) {
	t_log* logger = log_create("tests.log", "tests", true, LOG_LEVEL_INFO);
	log_info(logger, "Conectando con memoria");
	socketMemoria = crear_conexion("127.0.0.1","8002" );

	log_info(logger, "Enviando un mensaje a la Memoria...");
    
	t_paquete* conectar= crear_paquete();
	conectar->codigo_operacion = KERNEL;
	/* conectar->buffer->stream = string_duplicate("hola");
	conectar->buffer->size = strlen(conectar->buffer->stream) + 1; */
	enviar_paquete(conectar, socketMemoria);
    log_info(logger, "Mensaje enviado!");
   // conectar_con_memoria();

    proceso(INICIO_PROCESO, 121);
	respuesta_m();

	proceso(INICIO_PROCESO, 122);
	respuesta_m();

	printf( "inicio segmento \n");
	segmento(CREATE_SEGMENT, 121, 1, 10);
	respuesta_m();
	
	segmento(CREATE_SEGMENT, 121, 3, 120);
	respuesta_m();
	printf( "fin segmento \n");
	segmento(DELETE_SEGMENT, 121, 1, 20);
	respuesta_m();
	/* proceso( FIN_PROCESO, 121);
	respuesta_m();
 */
    return 0;
}
void conectar_con_memoria(){  

	t_log* logger = log_create("tests.log", "tests", true, LOG_LEVEL_INFO);
	log_info(logger, "Conectando con memoria");

	

	if(socketMemoria < 0)
    {
		log_error(logger, "Error al conectarse con memoria");
		log_destroy(logger);
		return EXIT_FAILURE;
	}
	
	log_info(logger, "Conexion con la Memoria realizada con exito!");

	log_info(logger, "Enviando un mensaje a la Memoria...");
    
	t_paquete* conectar= crear_paquete();
	conectar->codigo_operacion = KERNEL;
	enviar_paquete(conectar, socketMemoria);
    log_info(logger, "Mensaje enviado!");
    
	log_destroy(logger);
//	free(conectar);
}
void proceso(int cod, int pid) {

    t_paquete* paquete_a_memoria = crear_paquete_operacion(cod);
	agregar_a_paquete(paquete_a_memoria, &pid, sizeof(int));
    enviar_paquete(paquete_a_memoria, socketMemoria);
    eliminar_paquete(paquete_a_memoria);
    printf( "inicio proceso \n");
}
void segmento(int cod, int pid, int id, int tam) {
	
	

    t_paquete* paquete_a_memoria = crear_paquete_operacion(cod);
	agregar_a_paquete(paquete_a_memoria, &id, sizeof(int));
	if (cod == CREATE_SEGMENT)
	{
		agregar_a_paquete(paquete_a_memoria, &tam, sizeof(int));
	}
	
	agregar_a_paquete(paquete_a_memoria, &pid, sizeof(int));
	
    enviar_paquete(paquete_a_memoria, socketMemoria);
    eliminar_paquete(paquete_a_memoria);
    
}
void respuesta_m(){
	t_list* segmentos_recibidos;
	t_list* tabla;
	t_log* logger = log_create("tests.log", "tests", true, LOG_LEVEL_DEBUG);
	switch(recibir_operacion(socketMemoria)){
		case INICIO_PROCESO:
			segmentos_recibidos = _recibir_paquete(socketMemoria);
			tabla = deserializar_tabla_segmentos(segmentos_recibidos);
			list_destroy_and_destroy_elements(segmentos_recibidos,(void*) free);
			log_debug(logger, "Recibi la tabla de segmentos");
			leer_segmentos(tabla);
		break;
		case FIN_PROCESO:
			log_trace(logger, "Fin proceso ok");
			
		break;
		case CREATE_SEGMENT:
			segmentos_recibidos = _recibir_paquete(socketMemoria);
			int basae=*(int*)list_get(segmentos_recibidos,0);
			log_debug(logger, "Recibi la base de segmentos, %d", basae);
			
		break;
		case OUT_OF_MEMORY:
			log_error(logger, "No hay espacio en memoria, ok");
		break;
		case INICIO_COMPACTAR:
			log_warning(logger, "No hay hueco para crear el segmento, hay que compactar,ok");
		break;
		case DELETE_SEGMENT:
			segmentos_recibidos = _recibir_paquete(socketMemoria);
			tabla = deserializar_tabla_segmentos(segmentos_recibidos);
			list_destroy_and_destroy_elements(segmentos_recibidos,(void*) free);
			log_debug(logger, "Recibi la tabla de segmentos");
		break;	
		default:
			printf( "Error al recibir respuesta de la Memoria");
		
	}
		log_destroy(logger);
	
}
void leer_segmentos(t_list* un_pcb)
{
    for(int i = 0; i < list_size(un_pcb); i++)
    {
        t_segmento* segmento = list_get(un_pcb, i);
        printf("El segmento < %d > tiene un tamaño de: < %d > y su base es < %d >\n", i, segmento->size, segmento->base);
		free(segmento);
    }
}

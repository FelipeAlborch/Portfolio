#ifndef MAIN_H_
#define MAIN_H_

#include <memoriaUtils.h>
#include <memoriaConfig.h>
#include <memoriaConexion.h>



t_list* todos_los_segmentos; //tipo t_segmento

int resto_memoria;
int conexion;

t_log* loggerMemoria;
//t_log* logger;

t_config* memoriaConfig;

void startSigHandlers(void) {
	signal(SIGINT, sigHandler_sigint);
}

void sigHandler_sigint(int signo) {
	log_warning(loggerMemoria, "Tiraste un CTRL+C, macho, abortaste el proceso.");
	//metricas();
	//log_debug(logger,"metricas");
	terminar_programa(conexion, loggerMemoria, memoriaConfig);
	printf("-------------------FINAL POR CTRL+C-------------------");

	exit(-1);
}
#endif
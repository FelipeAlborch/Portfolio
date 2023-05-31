#include <memoriaUtils.h>
int running_cpu;
t_log* clogger;

void conectar_cpu(){
    config_memo.cpu=esperar_cliente(server_m);
    t_paquete* paquete; // =malloc(sizeof(t_paquete));
    paquete = recibir_paquete(config_memo.cpu);
    if(paquete->codigo_operacion != CPU){
        log_error(clogger,"Vos no sos el CPU. Se cancela la conexión");
        pthread_detach(hilo_cpu);
	    pthread_exit(&hilo_cpu);
    }
    log_info(clogger,"Se conectó el CPU: %d \n",config_memo.cpu);
		//eliminar_paquete(paquete);
    running_cpu=true;
    ejecutar_cpu();
}

void ejecutar_cpu(){
    int conectar=config_memo.cpu;
    log_trace(clogger, "Por ejecutar las tareas del CPU");

    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_cpu)
    {
        t_list* lista;
        int dir = -1;
        char* texto;
      switch (recibir_operacion(config_memo.cpu))
      {
          case MOV_IN_INSTRUCTION:
            lista = _recibir_paquete(config_memo.cpu);
            dir=*(int*)list_get(lista,0);
            //MOV_IN_SUCCES
            break;
            case MOV_OUT_INSTRUCTION:
            lista = _recibir_paquete(config_memo.cpu);
            dir=*(int*)list_get(lista,0);
            texto=*(char**)list_get(lista,1);
          default:
            break;
      }
      //running_cpu=false;
    }
    log_info(clogger,"Terminando de ejecutar las tareas del CPU");
    
  }
  //sleep(configuracionCPU.RETARDO_INSTRUCCION/1000);
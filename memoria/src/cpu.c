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
    eliminar_paquete(paquete);
    running_cpu=true;
    ejecutar_cpu();
}

void ejecutar_cpu(){
    int conectar=config_memo.cpu;
    log_trace(clogger, "Por ejecutar las tareas del CPU");

    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_cpu)
    {
        
        
      switch (recibir_operacion(config_memo.cpu))
      {
          case MOV_IN_INSTRUCTION:
            move_in();
            break;
            case MOV_OUT_INSTRUCTION:
            move_out();
            break;
          default:
            break;
      }
      //running_cpu=false;
    }
    log_info(clogger,"Terminando de ejecutar las tareas del CPU");
    
}
void move_in(){
    t_list* lista = _recibir_paquete(config_memo.cpu);
    int dir=*(int*)list_get(lista,0);
    int size=*(int*)list_get(lista,1);
    int pid = buscar_pid(dir);
    void* info = leer_dato(dir,size);
    responder_cpu(dir,MOV_IN_SUCCES,info);
    loggear(MOV_IN_INSTRUCTION,pid,"CPU",dir,strlen(info)+1,0);
    list_destroy_and_destroy_elements(lista,free);
}
void move_out(){
    t_list* lista = _recibir_paquete(config_memo.cpu);
    int dir=*(int*)list_get(lista,0);
    char* valor = (char*)list_get(lista,1);
    int pid = buscar_pid(dir);
    int info = escribir_dato(dir,valor);
    responder_cpu(dir,MOV_OUT_INSTRUCTION,info);
    loggear(MOV_OUT_INSTRUCTION,pid,"CPU",dir,strlen(valor)+1,0);
    list_destroy_and_destroy_elements(lista,free);
}
void* leer_dato(int direccion,int size){
    sleep(config_memo.retardo/1000);
    int pid = buscar_pid(direccion);
    t_segmento* segmento = buscar_segmento_dir(direccion);
    if(segmento == NULL){
        free(segmento);
        loggear(M_ERROR,pid,"",direccion,0,0);
        return NULL;
    }
    //memcpy(memoria+direccion,&valor,strlen(valor)+1);
	void* info = malloc(size);
    memcpy(info,memoria+direccion,size);
    loggear(M_READ,pid,info,direccion,size,segmento->base);
    free(segmento);
	return info;
}

int escribir_dato(int direccion,char* valor){
    sleep(config_memo.retardo/1000);
    

    int pid = buscar_pid(direccion);
    int cero = M_ERROR;
    if (pid != M_ERROR)
    {
        memcpy(memoria+direccion,&valor,strlen(valor)+1);
        cero = memcmp(memoria+direccion,valor,strlen(valor)+1);
        if (cero == 0 ){
            return pid;
        }
    }
    if (cero == M_ERROR || pid == M_ERROR)
    {
        
        loggear(M_ERROR,pid,valor,direccion,strlen(valor)+1,0);
        return M_ERROR;
    }

}
int buscar_pid(int dir){
    int pid = M_ERROR;
    bool _buscar_en_tabla (t_tabla_segmentos* tabla) {
        
        if(tabla->direcion_fisica == dir){
            pid = tabla->pid;
            return true;
        }else{
            return false;
        }
        
    }
    bool oki = list_any_satisfy(tabla_segmentos_gral, (void*)_buscar_en_tabla);
    if (oki == false) {
        log_error(mlogger,"No se encontro el segmento");
        return M_ERROR;
    }
    
    return pid;
}

void responder_cpu(int pid, int cod, void* info){
    if (pid == M_ERROR){
        loggear(M_ERROR,pid,"",0,0,0);
        respuestas(config_memo.cpu,M_ERROR,info);
        return;
    }
    respuestas(config_memo.cpu,cod,info);
    
    
}
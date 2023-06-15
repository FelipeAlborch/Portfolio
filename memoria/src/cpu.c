#include <memoriaUtils.h>
int running_cpu;
int running_fs;
t_log* clogger;
t_log* flogger;

void conectar_cpu(){
    pthread_mutex_lock(&m_config);
    config_memo.cpu=esperar_cliente(server_m);
    pthread_mutex_unlock(&m_config);
    t_paquete* paquete; // =malloc(sizeof(t_paquete));
    paquete = recibir_paquete(config_memo.cpu);
    if(paquete->codigo_operacion != CPU){
        log_error(clogger,"Vos no sos el CPU. Se cancela la conexión");
        //pthread_detach(hilo_cpu);
	    pthread_exit(&hilo_cpu);
    }
    log_info(clogger,"Se conectó el CPU: %d \n",config_memo.cpu);
    sem_post(&sem_cpu_conectado);
    eliminar_paquete(paquete);
    running_cpu=true;
    ejecutar_cpu();
}

void ejecutar_cpu(){
    pthread_mutex_lock(&m_config);
    int conectar=config_memo.cpu;
    pthread_mutex_unlock(&m_config);
    log_trace(clogger, "Por ejecutar las tareas del CPU");
    t_list* lista;
    //t_paquete* paquete_cpu =malloc(size_of(t_paquete));
    while (running_cpu)
    {
        
        
      switch (recibir_operacion(conectar))
      {
          case MOV_IN_INSTRUCTION:
            lista = _recibir_paquete(conectar);
            move_in(lista,MOV_IN_INSTRUCTION);
            break;
            case MOV_OUT_INSTRUCTION:
            lista = _recibir_paquete(conectar);
            move_out(lista,MOV_OUT_INSTRUCTION);
            break;
          default:
            break;
      }
      //running_cpu=false;
    }
    log_info(clogger,"Terminando de ejecutar las tareas del CPU");
    
}
void move_in(t_list* lista, int code){
    
    int dir=*(int*)list_get(lista,0);
    int size=*(int*)list_get(lista,1);
    int pid = buscar_pid(dir);
    void* info = leer_dato(dir,size);
    responder_cpu_fs(pid, code, info, dir);
    //list_destroy_and_destroy_elements(lista,free);
}
void move_out(t_list* lista, int code){

    int dir=*(int*)list_get(lista,0);
    char* valor = (char*)list_get(lista,1);
    int pid = buscar_pid(dir);
    int info = escribir_dato(dir,valor);
    responder_cpu_fs(pid, code, info, dir);
    
}
void* leer_dato(int direccion,int size){
    sleep(config_memo.retardo/1000);
    
    int base = buscar_base_dir(direccion);
	void* info = malloc(size);
    pthread_mutex_lock(&m_memoria);
        memcpy(info,memoria+base,size);
    pthread_mutex_unlock(&m_memoria);
    
	return info;
}

int escribir_dato(int direccion,char* valor){
    sleep(config_memo.retardo/1000);
    
    int pid = buscar_pid(direccion);
    int base = buscar_base_dir(direccion);
    int cero = M_ERROR;
    if (pid != M_ERROR)
    {
        pthread_mutex_lock(&m_memoria);
            memcpy(memoria+base,&valor,strlen(valor)+1);
            cero = memcmp(memoria+base,valor,strlen(valor)+1);
        pthread_mutex_unlock(&m_memoria);
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
    pthread_mutex_lock(&m_tabla_segmentos);
    bool oki = list_any_satisfy(tabla_segmentos_gral, (void*)_buscar_en_tabla);
    pthread_mutex_unlock(&m_tabla_segmentos);
    if (oki == false) {
        log_error(mlogger,"No se encontro el segmento");
        return M_ERROR;
    }
    
    return pid;
}

void responder_cpu(int pid, int cod, void* info){
    
    if (pid == M_ERROR){
        loggear(M_ERROR,pid,"",0,0,0);
        pthread_mutex_lock(&m_config);
        respuestas(config_memo.cpu,M_ERROR,info);
        pthread_mutex_unlock(&m_config);
        return;
    }
    pthread_mutex_lock(&m_config);
    respuestas(config_memo.cpu,cod,info);
    pthread_mutex_unlock(&m_config);
    
}


void conectar_fs(){
    pthread_mutex_lock(&m_config);
        config_memo.fs=esperar_cliente(server_m);
    pthread_mutex_unlock(&m_config);
    
    t_paquete* paquete;// =malloc(TAM_PAQ);
    pthread_mutex_lock(&m_config);
        paquete = recibir_paquete(config_memo.fs);
    pthread_mutex_unlock(&m_config);
    
    if(paquete->codigo_operacion != FILE_SYSTEM){
        log_error(flogger,"Vos no sos el FS. Se cancela la conexión %d ",paquete->codigo_operacion);
        paquete_destroy(paquete);
        pthread_exit(&hilo_fs);
    }
    log_info(flogger,"Se conectó el FileSystem: %d \n",config_memo.fs);
    sem_post(&sem_fs_conectado);
    paquete_destroy(paquete);
    running_fs=true;
    ejecutar_fs();
}

void ejecutar_fs(){
    pthread_mutex_lock(&m_config);
    int conectar=config_memo.fs;
    pthread_mutex_unlock(&m_config);
    log_info(flogger, "Por ejecutar las tareas del FileSystem");
    t_list* lista;
    while (running_fs)
    {
        switch (recibir_operacion(conectar)){
            case M_READ:
                lista = _recibir_paquete(conectar);
                move_in(lista,M_READ);
            break;
            case M_WRITE:
                lista = _recibir_paquete(conectar);
                move_out(lista,M_WRITE);    
            break;
            default:
            break;
        }
    }
    //running_fs=false;
    log_info(flogger,"Terminando de ejecutar las tareas del FileSystem");
}
void responder_cpu_fs(int pid, int cod, void* info, int dir){
    pthread_mutex_lock(&m_config);
    switch (cod){
        case M_ERROR:
            loggear(M_ERROR,pid,"",0,0,0);
            respuestas(config_memo.fs,M_ERROR,info);
            break;
        case MOV_IN_INSTRUCTION:
            respuestas(config_memo.cpu,MOV_IN_SUCCES,info);
            loggear(MOV_IN_INSTRUCTION,pid,"CPU",dir,strlen(info)+1,0);
            break;
        case MOV_OUT_INSTRUCTION:
            respuestas(config_memo.cpu,cod,info);
            loggear(MOV_OUT_INSTRUCTION,pid,"CPU",dir,strlen(info)+1,0);
            break;
        case M_READ:
            respuestas(config_memo.fs,cod,info);
            loggear(M_READ,pid,info,dir,strlen(info)+1,0);
            break;
        case M_WRITE:
            respuestas(config_memo.fs,cod,info);
            loggear(M_WRITE,pid,info,dir,strlen(info)+1,0);
            break;
        default:
            loggear(M_ERROR,pid,"",0,0,0);
            respuestas(config_memo.cpu,M_ERROR,info);
            break;
    }
    pthread_mutex_unlock(&m_config);
}
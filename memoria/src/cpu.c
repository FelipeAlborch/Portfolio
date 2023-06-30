#include <memoriaUtils.h>
int running_cpu;
int running_fs;
t_log* clogger;
t_log* flogger;

/**
 * 
 *          FUNCIONES PARA CPU
 * 
*/

void conectar_cpu(){
    pthread_mutex_lock(&m_config);
        config_memo.cpu=esperar_cliente(server_m);
    pthread_mutex_unlock(&m_config);
    
    t_paquete* paquete = recibir_paquete(config_memo.cpu);

    if(paquete->codigo_operacion != CPU){
        log_error(clogger,"Vos no sos el CPU. Se cancela la conexión");
	    paquete_destroy(paquete);
        pthread_exit(&hilo_cpu);
    }
    
    log_info(clogger,"Se conectó el CPU: %d \n",config_memo.cpu);
    sem_post(&sem_cpu_conectado);
    paquete_destroy(paquete);
    running_cpu=true;
    ejecutar_cpu();
}

void ejecutar_cpu(){
    pthread_mutex_lock(&m_config);
        int conectar=config_memo.cpu;
    pthread_mutex_unlock(&m_config);
    log_trace(clogger, "Por ejecutar las tareas del CPU");
    t_list* lista;
    
    while (running_cpu){
        switch (recibir_operacion(conectar)){
            case MOV_IN_INSTRUCTION:
                lista = _recibir_paquete(conectar);
                move_in(lista,MOV_IN_INSTRUCTION);
                list_destroy_and_destroy_elements(lista, free);
            break;
            case MOV_OUT_INSTRUCTION:
                lista = _recibir_paquete(conectar);
                move_out(lista,MOV_OUT_INSTRUCTION);
                list_destroy_and_destroy_elements(lista, free);
            break;
            default:
                log_error(clogger,"No se reconoce la instrucción");
            break;
    }
    //running_cpu=false;
    }
    log_info(clogger,"Terminando de ejecutar las tareas del CPU");
    
}
/**
 * 
 *          FUNCIONES PARA FS
 * 
*/
void conectar_fs(){
    pthread_mutex_lock(&m_config);
        config_memo.fs=esperar_cliente(server_m);
    pthread_mutex_unlock(&m_config);
    
    
    pthread_mutex_lock(&m_config);
        t_paquete* paquete = recibir_paquete(config_memo.fs);
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
                list_destroy_and_destroy_elements(lista, free);
            break;
            case M_WRITE:
                lista = _recibir_paquete(conectar);
                move_out(lista,M_WRITE);  
                list_destroy_and_destroy_elements(lista, free);  
            break;
            default:
            break;
        }
    }
    //running_fs=false;
    log_info(flogger,"Terminando de ejecutar las tareas del FileSystem");
}
/**
 * 
 *          FUNCIONES PARA LEER Y ESCRIBIR
 * 
*/
void move_in(t_list* lista, int code){
    
    int dir=*(int*)list_get(lista,0);
    int size=*(int*)list_get(lista,1);
    int offset=*(int*)list_get(lista,2);
    int pid = buscar_pid(dir);
    void* info = leer_dato(dir,size, offset);

    char* datos = malloc(size);
    memcpy(datos, info, size);
    printf("lei: %s \n", datos);
    //char* datos = void_a_string(info,size);
    
    responder_cpu_fs(pid, code, datos, dir, size);
    free(datos);
    free(info);
}
void move_out(t_list* lista, int code){

    int dir=*(int*)list_get(lista,0);
    char* valor = (char*)list_get(lista,1);
    int size=*(int*)list_get(lista,2); 
    int offset=*(int*)list_get(lista,3);
    printf("valores: %d, %s, %d, %d", dir, valor, size, offset);
    int pid = buscar_pid(dir);

    int info = escribir_dato(dir,valor,size, offset);
    if (info == M_ERROR)
    {
        log_error(clogger,"No se pudo escribir en la memoria");
        return;
    }
    
    responder_cpu_fs(pid, code, NULL, dir, size);
    
}
void* leer_dato(int direccion,int size, int offset){
    sleep(config_memo.retardo/1000);
    
    int base = buscar_base_dir(direccion);
	void* info = malloc(size);
    pthread_mutex_lock(&m_memoria);
        memcpy(info,memoria+base+offset,size);
    pthread_mutex_unlock(&m_memoria);
    
	return info;
}
int escribir_dato(int direccion,char* valor, int size,int offset){
    sleep(config_memo.retardo/1000);
    
    log_debug(clogger,"Direccion: %d  Info: %s  Tamanio: %d",direccion,valor,size);
    int pid = buscar_pid(direccion);

    log_info(clogger,"PID: %d",pid);
    int base = buscar_base_dir(direccion);

    log_info(clogger,"Base: %d",base);  
    int cero = M_ERROR;
    if (pid != M_ERROR)
    {
        pthread_mutex_lock(&m_memoria);
            memcpy(memoria+base+offset,&valor,size);
            cero = memcmp(memoria+base+offset,valor,size);
        pthread_mutex_unlock(&m_memoria);
        printf("Cero: %d\n",cero);
        if (cero == 0 ){
            //loggear(MOV_OUT_INSTRUCTION,pid,valor,direccion,size,0);
            return pid;
        }
    }
    if (cero == M_ERROR || pid == M_ERROR)
    {
        loggear(M_ERROR,pid,valor,direccion,size,0);
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
    
/*     if (pid == M_ERROR){
        loggear(M_ERROR,pid,"",0,0,0);
        pthread_mutex_lock(&m_config);
        respuestas(config_memo.cpu,M_ERROR,info);
        pthread_mutex_unlock(&m_config);
        return;
    }
    pthread_mutex_lock(&m_config);
    respuestas(config_memo.cpu,cod,info);
    pthread_mutex_unlock(&m_config); */
    
}
void responder_cpu_fs(int pid, int cod, void* info, int dir, int size){
    pthread_mutex_lock(&m_config);
    switch (cod){
        case M_ERROR:
            loggear(M_ERROR,pid,"",0,0,0);
            respuestas(config_memo.fs,M_ERROR,info);
            break;
        case MOV_IN_INSTRUCTION:
            respuestas(config_memo.cpu,MOV_IN_SUCCES,info);
            loggear(MOV_IN_INSTRUCTION,pid,"CPU",dir,size,0);
            break;
        case MOV_OUT_INSTRUCTION:
            respuestas(config_memo.cpu,cod,NULL);
            loggear(MOV_OUT_INSTRUCTION,pid,"CPU",dir,size,0);
            break;
        case M_READ:
            respuestas(config_memo.fs,cod,NULL);
            loggear(M_READ,pid,info,dir,size,0);
            break;
        case M_WRITE:
            respuestas(config_memo.fs,cod,info);
            loggear(M_WRITE,pid,info,dir,size,0);
            break;
        default:
            loggear(M_ERROR,pid,"",0,0,0);
            respuestas(config_memo.cpu,M_ERROR,info);
            break;
    }
    pthread_mutex_unlock(&m_config);
}
char* void_a_string(void* info, int size){
    char* dato =malloc(size+1);
    memcpy(&dato ,info,size);
    printf("String: %s\n",dato);
   // string[size]='\0';
    printf("String: %s\n",dato);
    return dato;
}
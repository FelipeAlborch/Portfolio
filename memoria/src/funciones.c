#include <memoriaUtils.h>
t_list* listaS; // pid 121
t_list* listaS1; // pid 122
t_list* listaS2; // pid 124
void crear_proc(){
    
    listaS=crear_tabla_proceso(121);
   // listaS1=crear_tabla_proceso(122);
    listaS2=crear_tabla_proceso(124);

    log_warning(mlogger,"Por imprimir la tabla de segmentos pid 121");  
    imprimir_tabla(listaS);
    /* log_warning(mlogger,"Por imprimir la tabla de segmentos pid 122");
    imprimir_tabla(listaS1); */
    log_warning(mlogger,"Por imprimir la tabla de segmentos pid 124");    
    imprimir_tabla(listaS2);
}
void crear_seg(){
    
    create_segment(121,128,4);
    create_segment(121,64,3);
   // create_segment(122,300,3);
    create_segment(121,50,1);
    create_segment(124,100,1);
    create_segment(121,500,2);
  //  create_segment(122,300,1);
    create_segment(221,1000,4);
    imprimir_huecos();
    imprimir_tabla_gral();
}
void ejecutar_kernel_test(){
    int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");
    int pid =221;
    int tam = 250;
    crear_proceso(pid);
    
    log_trace(klogger,"ejecute la creación del proceso %d",pid);
    sleep(1);
    sleep(1);
    crear_proc();
    crear_seg(); 
    log_trace(klogger,"quiero eliminar el seg 3 %d",pid);
    eliminar_segmento(121,3);
    eliminar_segmento(121,1);
    create_segment(221,45,2);
    eliminar_segmento(121,2);
    eliminar_segmento(124,1);
    create_segment(124,465,3);
   // liberar_proceso(122);
    //liberar_proceso(221);
    /* 
    loggear(FIN_PROCESO,221,NULL,0,0,0);
    loggear(FIN_PROCESO,122,NULL,0,0,0);
     */
    //eliminar_segmento(121,2);
    //liberar_proceso(122);
   // log_trace(klogger,"quiero eliminar el seg 2 %d",pid);
   // imprimir_huecos();
    
    compactar();
    imprimir_tabla_gral();
    tablas_compactadas();
}
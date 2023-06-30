#include <memoriaUtils.h>
t_list* listaS; // pid 121
t_list* listaS1; // pid 122
t_list* listaS2; // pid 124
void crear_proc(){
    
    listaS=crear_tabla_proceso(121);
/*     listaS1=crear_tabla_proceso(122);
    listaS2=crear_tabla_proceso(124); */

    log_warning(klogger,"Por imprimir la tabla de segmentos pid 121");  
    imprimir_tabla(listaS);
/*     log_warning(mlogger,"Por imprimir la tabla de segmentos pid 122");
    imprimir_tabla(listaS1); 
    log_warning(mlogger,"Por imprimir la tabla de segmentos pid 124");    
    imprimir_tabla(listaS2); */
    list_destroy_and_destroy_elements(listaS,(void*)free);
}
void crear_seg(){
    
    create_segment(121,128,1);
    create_segment(121,64 ,2);
    create_segment(121,128,3);
    create_segment(121,64 ,4);
    create_segment(121,128,5);
    create_segment(121,64 ,6);
    create_segment(121,128,7);
    create_segment(121,64 ,8);
    create_segment(121,128,9);
    log_debug(klogger,"se crearon los segmentos del proceso 121");
    imprimir_huecos();
    imprimir_tabla_gral();
}
void ejecutar_kernel_test(){
    /* int conectar=config_memo.kernel;
    log_trace(mlogger, "Por ejecutar las tareas del kernel");
    int pid =121;
    int tam = 250;
    //crear_proceso(pid);
    crear_proc();
    log_trace(klogger,"ejecute la creación del proceso %d",pid);
    sleep(1);
    sleep(1);
    
    crear_seg();  */

    log_trace(klogger,"quiero eliminar el seg 2 %d\n",121);
    eliminar_segmento(121,2);
    log_trace(klogger,"quiero eliminar el seg 4 %d\n",121);
    eliminar_segmento(121,4);

    //imprimir_huecos();
    //compactar();
    //create_segment(121,100,10);
    /* 
    eliminar_segmento(121,2);
    eliminar_segmento(124,1);
    create_segment(124,465,3); */
   // liberar_proceso(122);
    //liberar_proceso(221);
    /* 
    loggear(FIN_PROCESO,221,NULL,0,0,0);
    loggear(FIN_PROCESO,122,NULL,0,0,0);
     */
    //eliminar_segmento(121,2);
    //liberar_proceso(122);
   // log_trace(klogger,"quiero eliminar el seg 2 %d",pid);
   //
    
    
    imprimir_tabla_gral();
   // tablas_compactadas();
    
}
void ejecuteTest(){


    crear_proc();
    crear_seg();
    ejecutar_kernel_test();

    t_list* listaS3 = list_create();
    list_add(listaS3,640);
    list_add(listaS3,"hola como estasss!");
    list_add(listaS3,string_length("hola como estasss!")+1);
    int size = list_get(listaS3,2);
    int dir=list_get(listaS3,0);
    char* dato = list_get(listaS3,1);
    log_debug(klogger,"por escribir en memoria %d",dir);
    int i = escribir_dato(128,dato,size,20);
   // move_out(listaS3,MOV_OUT);

    log_trace(klogger,"quiero crear el seg 10 %d\n",121);
    create_segment(121,100,10);
    dir =512;
    log_debug(klogger,"el dato leido");

    void* info1 = leer_dato(128,size,20);
    char* info =(char*)info1;//= malloc(size+1);
    //info = void_a_string(info1,size,&info);
    
    
    log_debug(klogger,"el dato leido es %s",info);

    terminar_programa(loggerMemoria);
}
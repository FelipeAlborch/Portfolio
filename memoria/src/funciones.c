#include <memoriaUtils.h>
t_list* listaS; // pid 121
t_list* listaS1; // pid 122
t_list* listaS2; // pid 124
void crear_proc(){
    
    listaS=crear_tabla_proceso(121);
    listaS1=crear_tabla_proceso(122);
    listaS2=crear_tabla_proceso(124);

    log_warning(mlogger,"Por imprimir la tabla de segmentos pid 121");  
    imprimir_tabla(listaS);
    log_warning(mlogger,"Por imprimir la tabla de segmentos pid 122");
    imprimir_tabla(listaS1);
    log_warning(mlogger,"Por imprimir la tabla de segmentos pid 124");    
    imprimir_tabla(listaS2);
}
void crear_seg(){
    create_segment(121,64,3);
    create_segment(121,128,4);
    create_segment(122,300,3);
    create_segment(121,50,1);
    create_segment(124,100,1);
    create_segment(121,500,2);
    create_segment(122,300,1);
    imprimir_huecos();
    imprimir_tabla_gral();
}

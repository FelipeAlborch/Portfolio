#include <utils/PCB.h>


void* liberar_instruccion(LineaInstruccion* una_instruccion)
{
    free(una_instruccion->identificador);
    free(una_instruccion->parametros[0]);
    free(una_instruccion->parametros[1]);
    free(una_instruccion->parametros[2]);
    free(una_instruccion->parametros[3]);
    free(una_instruccion);
    return NULL;
}
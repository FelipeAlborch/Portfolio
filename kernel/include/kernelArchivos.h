#ifndef KERNELARCHIVOS_H_
#define KERNELARCHIVOS_H_

#include <commons/collections/dictionary.h>

typedef struct ArchivoAbierto
{
	char *nombreArchivo;	 // Nombre del archivo
	int tamArchivo;			 // Tamaño del archivo en bytes
	int numProcesos;		 // Número de procesos con el archivo abierto
	int *procesosBloqueados; // Lista de procesos bloqueados esperando este archivo
	int puntero;			 // Puntero de lectura/escritura del archivo
} ArchivoAbierto;

void inicializar_archivos_kernel();

#endif
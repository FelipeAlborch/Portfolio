#ifndef FSUTILS_H_
#define FSUTILS_H_
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fun.h>

typedef struct FCB {
    char *file_name;
    int file_size;
    char *dptr;
    t_list *iptr;
} FCB;

typedef struct FCB_table {
    char *block_store;
    int block_size;
    int block_count;
    t_bitarray *bitmap;
    t_dictionary *index;
} FCB_table;

/**
 * print_cwd - imprime el directorio actual de trabajo.
 */
void print_cwd();

/**
 * data_byte_count - devuelve la cantidad de bytes que ocupa el almacenamiento de datos.
 * 
 * @param fcb_table: tabla de control de archivos
 * @return cantidad de bytes que ocupa el almacenamiento de datos
 */
int data_byte_count(FCB_table *fcb_table);

/**
 * bitmap_byte_count - devuelve la cantidad de bytes que ocupa el bitmap.
 * 
 * @param fcb_table: tabla de control de archivos
 * @return cantidad de bytes que ocupa el bitmap
 */
int bitmap_byte_count(FCB_table *fcb_table);

/**
 * fcb_table_init - inicializa campos de tabla de control de archivos.
 * 
 * @param fcb_table: tabla de control de archivos
 * @param block_store_file: archivo de almacenamiento de bloques
 * @param block_size: tamaño de bloque
 * @param block_count: cantidad de bloques
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
 int fcb_table_init(FCB_table *fcb_table, char *bitmap_file, char *store_file, int block_size, int block_count);

/**
 * mmap_file - mapea un archivo en memoria.
 * 
 * @param file_name: nombre del archivo
 * @param length: longitud del archivo en bytes
 * @param file_pointer: contenido del archivo
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
int mmap_file_sync(char *file_name, int length, char **file_pointer);

/**
 * f_open - busca el archivo recibido por parámetro en la tabla global de archivos abiertos.
 * Si lo encuentra, agrega una entrada a la tabla de archivos abiertos del proceso.
 * Si no lo encuentra, lo crea y agrega una entrada a la tabla global de archivos abiertos
 * y a la tabla de archivos abiertos del proceso.
 *
 * @param global_table: tabla global de archivos abiertos
 * @param process_table: tabla de archivos abiertos del proceso
 * @param file_name: nombre del archivo
 * @param flags: banderas de apertura
 * @param mode: modo de apertura
 * @return descriptor de archivo si se abrió o creó correctamente, -1 si ocurrió un error
 */
// int f_open(global_table* global_table, process_table* process_table, char* file_name, int flags, mode_t mode);

/**
 * f_create - crea un archivo en la tabla global de archivos abiertos y en la tabla de archivos abiertos del proceso
 *
 * @param global_table: tabla global de archivos abiertos
 * @param process_table: tabla de archivos abiertos del proceso
 * @param file_name: nombre del archivo
 * @return descriptor de archivo si se creó correctamente, -1 si ocurrió un error
 */
FCB *fcb_create(char *file_name);

/**
 * f_destroy - destruye un archivo de la tabla global de archivos abiertos y de la tabla de archivos abiertos del proceso
 *
 * @param global_table: tabla global de archivos abiertos
 * @param process_table: tabla de archivos abiertos del proceso
 * @param file_name: nombre del archivo
 * @return 0 si se destruyó correctamente, -1 si ocurrió un error
 */
int fcb_destroy(FCB *fcb);

/**
 * fcb_table_destroy - libera memoria de tabla de control de archivos.
 * 
 * @param fcb_table: tabla de control de archivos
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
int fcb_table_destroy(FCB_table *fcb_table);

/**
 * fcb_realloc - redimensiona un archivo de la tabla global de archivos abiertos y de la tabla de archivos abiertos del proceso
 *
 * @param new_size: nuevo tamaño del archivo
 * @param fcb: archivo a redimensionar
 * @param fcb_table: tabla de archivos abiertos del proceso
 * @return 0 si se redimensionó correctamente, -1 si ocurrió un error
 */
int fcb_realloc(int new_size, FCB *fcb, FCB_table *fcb_table);

/**
 * fcb_dealloc - libera los bloques de un archivo de la tabla global de archivos abiertos y de la tabla de archivos abiertos del proceso
 *
 * @param fcb: archivo a liberar
 * @param fcb_table: tabla de archivos abiertos del proceso
 * @return 0 si se liberó correctamente, -1 si ocurrió un error
 */
int fcb_dealloc(FCB *fcb, FCB_table *fcb_table);

/**
 * fcb_table_add - agrega un archivo a la tabla de archivos abiertos del proceso
 *
 * @param fcb: archivo a agregar
 * @param fcb_table: tabla de archivos abiertos del proceso
 * @return 0 si se agregó correctamente, -1 si ocurrió un error
 */
int fcb_table_add(FCB *fcb, FCB_table *fcb_table);

/**
 * fcb_table_remove - quita un archivo de la tabla de archivos abiertos del proceso
 *
 * @param file_name: nombre del archivo a quitar
 * @param fcb_table: tabla de archivos abiertos del proceso
 * @return 0 si se quitó correctamente, -1 si ocurrió un error
 */
int fcb_table_remove(char *file_name, FCB_table *fcb_table);

// int f_create(...);

// int f_close(...);

// int f_seek(...);

// int f_truncate(...);

// int f_read(...);

// int f_write(...);

#endif
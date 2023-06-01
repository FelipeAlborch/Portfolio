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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fun.h>

typedef struct FCB {
    char *file_name;
    int file_size;
    char *direct_ptr;
    char *indirect_ptr;
} FCB;

typedef struct FCB_table {
    char *block_store;
    int bit_count;
    int byte_count;
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
 * fcb_table_init - inicializa campos de tabla de control de archivos.
 * 
 * @param fcb_table: tabla de control de archivos
 * @param block_store_file: archivo de almacenamiento de bloques
 * @param block_size: tamaño de bloque
 * @param block_count: cantidad de bloques
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
 int fcb_table_init(FCB_table *fcb_table, char *block_store_file, int block_size, int block_count);

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
 * f_close - quita la entrada correspondiente al archivo recibido por parámetro
 * de la tabla de archivos abiertos del proceso
 *
 * @param global_table: tabla global de archivos abiertos
 * @param process_table: tabla de archivos abiertos del proceso
 * @param fd: descriptor de archivo
 * @return 0 si se cerró el archivo correctamente, -1 si ocurrió un error
 */
// int f_close(global_table* global_table, process_table* process_table, int fd);

/**
 * f_read - lee datos de un archivo
 *
 * @param process_table: tabla de archivos abiertos del proceso
 * @param fd: descriptor de archivo
 * @param buffer: buffer donde se almacenarán los datos leídos
 * @param count: cantidad de bytes a leer
 * @return cantidad de bytes leídos, -1 si ocurrió un error
 */
// int f_seek(process_table* process_table, int fd, off_t offset, int whence);

/**
 * f_truncate - solicita al módulo File System que actualice el tamaño del archivo al nuevo tamaño pasado por parámetro
 *
 * @param process_table: tabla de archivos abiertos del proceso
 * @param fd: descriptor de archivo
 * @param length: nuevo tamaño del archivo
 * @return 0 si la operación se realizó correctamente, -1 si ocurrió un error
 */
// int f_truncate(process_table* process_table, int fd, off_t length);

/**
 * f_read - solicita al módulo File System que lea desde el archivo y guarde los datos leídos en el buffer pasado por parámetro
 *
 * @param process_table: tabla de archivos abiertos del proceso
 * @param fd: descriptor de archivo
 * @param buffer: buffer donde se guardarán los datos leídos
 * @param count: cantidad de bytes a leer
 * @return la cantidad de bytes leídos si la operación se realizó correctamente, -1 si ocurrió un error
 */
// int f_read(process_table* process_table, int fd, void* buffer, size_t count);

/**
 * f_write - solicita al módulo File System que escriba en el archivo los datos pasados por parámetro
 *
 * @param process_table: tabla de archivos abiertos del proceso
 * @param fd: descriptor de archivo
 * @param buffer: buffer con los datos a escribir
 * @param count: cantidad de bytes a escribir
 * @return la cantidad de bytes escritos si la operación se realizó correctamente, -1 si ocurrió un error
 */
// int f_write(process_table* process_table, int fd, const void* buffer, size_t count);

#endif
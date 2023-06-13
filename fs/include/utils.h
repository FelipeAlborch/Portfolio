#ifndef FSUTILS_H_
#define FSUTILS_H_
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <commons/log.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <config.h>
#include <fun.h>

typedef struct FCB {
    char *file_path;
    char *file_name;
    int file_size;
    uint32_t dptr;
    uint32_t iptr;
} FCB;

typedef struct Superbloque {
    int BLOCK_SIZE;
    int BLOCK_COUNT;
} Superbloque;

typedef struct FS {
    t_log *log;
    fs_config *config;
    char *bloques;
    t_bitarray *bitmap;
    Superbloque *superbloque;
} FS;

/**
 * print_cwd - imprime el directorio actual de trabajo.
 */
void print_cwd();

/**
 * data_byte_count - devuelve la cantidad de bytes que ocupa el almacenamiento de datos.
 * 
 * @param superbloque: superbloque 
 * @return cantidad de bytes que ocupa el almacenamiento de datos
 */
int data_byte_count(Superbloque *superbloque);

/**
 * bitmap_byte_count - devuelve la cantidad de bytes que ocupa el bitmap.
 * 
 * @param superbloque: superbloque
 * @return cantidad de bytes que ocupa el bitmap
 */
int bitmap_byte_count(Superbloque *superbloque);

/**
 * fs_create - crea un filesystem.
 * 
 * @param config_path: archivo de configuración del sistema de archivos
 * @return filesystem
 */
FS * fs_create(char *config_path);

/**
 * fs_destroy - destruye un filesystem.
 * 
 * @param fs: filesystem
 */
void fs_destroy(FS *fs);

/**
 * mmap_file - mapea un archivo en memoria.
 * 
 * @param file_path: ruta del archivo
 * @param length: longitud del archivo en bytes
 * @param file_pointer: contenido del archivo
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
int mmap_file_sync(char *file_path, int length, char **file_pointer);

/**
 * fcb_create - crea un file control block (FCB) y el archivo asociado.
 * 
 * @param file_name: nombre del archivo
 * @param fs: filesystem
 * @param fcb: puntero al FCB creado
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
int fcb_create(char *file_name, FS *fs, FCB **fcb);

/**
 * fcb_update - actualiza un file control block (FCB).
 * 
 * @param fcb: archivo a actualizar
 * @return 0 si se actualizó correctamente, -1 si ocurrió un error
 */
int fcb_update(FCB *fcb);

/**
 * fcb_create_from_file - crea un file control block (FCB) a partir de un archivo.
 * 
 * @param path: ruta del archivo
 * @param fs: filesystem
 * @param fcb: puntero al FCB creado
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
int fcb_create_from_file(char *path, FS *fs, FCB **fcb);

/**
 * fcb_destroy - destruye un file control block (FCB) y el archivo asociado.
 * 
 * @param fcb: archivo a destruir
 */
void fcb_destroy(FCB *fcb);

/**
 * fcb_alloc - asigna bloques a un archivo.
 * 
 * @param size: tamaño a reservar en archivo
 * @param fcb: archivo a asignar bloques
 * @param fs: filesystem
 * @return 0 si se asignaron correctamente, -1 si ocurrió un error
 */
int fcb_alloc(int size, FCB *fcb, FS *fs);

/**
 * fcb_realloc - redimensiona un archivo
 *
 * @param new_size: nuevo tamaño del archivo
 * @param fcb: archivo a redimensionar
 * @param fs: filesystem
 * @return 0 si se redimensionó correctamente, -1 si ocurrió un error
 */
int fcb_realloc(int new_size, FCB *fcb, FS *fs);

/**
 * fcb_dealloc - libera los bloques de un archivo
 *
 * @param size: tamaño a liberar en archivo
 * @param fcb: archivo a liberar
 * @param fs: filesystem
 * @return 0 si se liberó correctamente, -1 si ocurrió un error
 */
int fcb_dealloc(int size, FCB *fcb, FS *fs);

/**
 * superbloque_create_from_file - crea un superbloque a partir de un archivo.
 * 
 * @param file_path: ruta del archivo
 * @return puntero al superbloque creado
 */
Superbloque * superbloque_create_from_file(char *file_path);

/**
 * superbloque_destroy - destruye un superbloque.
 * 
 * @param superbloque: superbloque
 */
void superbloque_destroy(Superbloque *superbloque);

/**
 * f_open - abre un archivo.
 * 
 * @param file_name: nombre del archivo
 * @param fs: filesystem
 * @return 0 si se abrió correctamente, -1 si ocurrió un error
 */
int f_open(char *file_name, FS *fs);

/**
 * f_create - crea un archivo.
 * 
 * @param file_name: nombre del archivo
 * @param fs: filesystem
 * @return 0 si se creó correctamente, -1 si ocurrió un error
 */
int f_create(char *file_name, FS *fs);

/**
 * f_truncate - trunca un archivo.
 * 
 * @param file_name: nombre del archivo
 * @param size: tamaño del archivo
 * @param fs: filesystem
 * @return 0 si se truncó correctamente, -1 si ocurrió un error
 */
int f_truncate(char *file_name, int size, FS *fs);

/**
 * f_read - lee un archivo.
 * 
 * @param file_name: nombre del archivo
 * @param offset: offset del archivo
 * @param size: tamaño del archivo
 * @param buffer: buffer donde se almacenará el contenido del archivo
 * @param fs: filesystem
 * @return 0 si se leyó correctamente, -1 si ocurrió un error
 */
int f_read(char *file_name, int offset, int size, void **buffer, FS *fs);

/**
 * f_write - escribe un archivo.
 * 
 * @param file_name: nombre del archivo
 * @param offset: offset del archivo
 * @param size: tamaño del archivo
 * @param buffer: buffer donde se almacenará el contenido del archivo
 * @param fs: filesystem
 * @return 0 si se escribió correctamente, -1 si ocurrió un error
 */
int f_write(char *file_name, int offset, int size, void *buffer, FS *fs);

#endif
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
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <config.h>
#include <fun.h>

typedef struct FCB {
    char *file_name;
    int file_size;
    uint32_t dptr;
    uint32_t iptr;
} FCB;

typedef struct Superbloque {
    int BLOCK_SIZE;
    int BLOCK_COUNT;
} Superbloque;

typedef struct Disk {
    char *bloques;
    t_bitarray *bitmap;
    Superbloque *superbloque;
} Disk;

typedef struct Env {
    t_log *logger;
    fs_config *config;
    Disk *disk;
} Env;

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
 * disk_create - crea un disco.
 * 
 * @param config: configuración del sistema de archivos
 * @return disco
 */
Disk * disk_create(fs_config *config);

/**
 * disk_destroy - destruye un disco.
 * 
 * @param disk: disco
 */
void disk_destroy(Disk *disk);

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
 * fcb_create - crea un file control block (FCB) para un archivo.
 * 
 * @param file_name: nombre del archivo
 * @return puntero al FCB creado
 */
FCB * fcb_create(char *file_name);

/**
 * fcb_create_from_file - crea un file control block (FCB) para un archivo existente.
 * 
 * @param file_name: nombre del archivo
 * @param disk: disco
 * @return puntero al FCB creado
 */
FCB * fcb_create_from_file(char *file_name, Disk *disk);

/**
 * fcb_destroy - destruye un file control block (FCB).
 * 
 * @param fcb: archivo a destruir
 */
void fcb_destroy(FCB *fcb);

/**
 * fcb_alloc - asigna bloques a un archivo.
 * 
 * @param size: tamaño a reservar en archivo
 * @param fcb: archivo a asignar bloques
 * @param disk: disco
 * @return 0 si se asignaron correctamente, -1 si ocurrió un error
 */
int fcb_alloc(int size, FCB *fcb, Disk *disk);

/**
 * fcb_realloc - redimensiona un archivo
 *
 * @param new_size: nuevo tamaño del archivo
 * @param fcb: archivo a redimensionar
 * @param disk: disco
 * @return 0 si se redimensionó correctamente, -1 si ocurrió un error
 */
int fcb_realloc(int new_size, FCB *fcb, Disk *disk);

/**
 * fcb_dealloc - libera los bloques de un archivo
 *
 * @param size: tamaño a liberar en archivo
 * @param fcb: archivo a liberar
 * @param disk: disco
 * @return 0 si se liberó correctamente, -1 si ocurrió un error
 */
int fcb_dealloc(int size, FCB *fcb, Disk *disk);

/**
 * superbloque_create_from_file - crea un superbloque a partir de un archivo.
 * 
 * @param file_name: nombre del archivo
 * @return puntero al superbloque creado
 */
Superbloque * superbloque_create_from_file(char *file_name);

/**
 * superbloque_destroy - destruye un superbloque.
 * 
 * @param superbloque: superbloque
 */
void superbloque_destroy(Superbloque *superbloque);

// int f_create(...);

// int f_close(...);

// int f_seek(...);

// int f_truncate(...);

// int f_read(...);

// int f_write(...);

#endif
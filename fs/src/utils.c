#include <utils.h>

void print_cwd() {
    printf("PWD = %s\n", getcwd(NULL, 0));
}

int data_byte_count(Superbloque *superbloque) {
    return superbloque->BLOCK_SIZE * superbloque->BLOCK_COUNT;
}

int bitmap_byte_count(Superbloque *superbloque) {
    return (superbloque->BLOCK_COUNT + 7) / 8;
}

FS *fs_create(char *config_path) {
    int error = 0;
    FS *fs = malloc(sizeof(FS));

    fs->log = log_create("fs.log", "FS", 1, LOG_LEVEL_TRACE);
    fs->config = config_create_fs_from_file(config_path);

    print_cwd();
    config_print_fs(fs->config);

    fs->superbloque = superbloque_create_from_file(fs->config->PATH_SUPERBLOQUE);
    fs->bloques = malloc(data_byte_count(fs->superbloque));

    if (access(fs->config->PATH_BITMAP, F_OK) != -1) {
        int fd = open(fs->config->PATH_BITMAP, O_RDONLY);
        char *content = malloc(bitmap_byte_count(fs->superbloque));
        error = read(fd, content, bitmap_byte_count(fs->superbloque));
        assert(error == bitmap_byte_count(fs->superbloque));
        fs->bitmap = bitarray_create_with_mode(content, fs->superbloque->BLOCK_COUNT, MSB_FIRST);
        close(fd);
    } else {
        char *zeroes = calloc(bitmap_byte_count(fs->superbloque), 8);
        fs->bitmap = bitarray_create_with_mode(zeroes, bitmap_byte_count(fs->superbloque), MSB_FIRST);
    }

    error = mmap_file_sync(fs->config->PATH_BITMAP, bitmap_byte_count(fs->superbloque), &fs->bitmap->bitarray);
    assert(error != -1);

    error = mmap_file_sync(fs->config->PATH_BLOQUES, data_byte_count(fs->superbloque), &fs->bloques);
    assert(error != -1);

    return fs;
}

void fs_destroy(FS *fs) {
    superbloque_destroy(fs->superbloque);
    bitarray_destroy(fs->bitmap);
    free(fs->bloques);
    config_destroy_fs(fs->config);
    log_destroy(fs->log);
    free(fs);
}

int mmap_file_sync(char *file_path, int length, char **file_content){

    struct stat file_stat;

    int fd = open(file_path, O_RDWR | O_CREAT, 0775);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    fstat(fd, &file_stat);

    if (file_stat.st_size == 0) {
        if (ftruncate(fd, length) == -1) {
            perror("ftruncate");
            return -1;
        }
    }

    fstat(fd, &file_stat);

    *file_content = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (*file_content == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    close(fd);
    return 0;
}

int fcb_create(char *file_name, FS *fs, FCB **fcb) {
    char *file_path = string_from_format("%s%c%s", fs->config->PATH_FCB, '/', file_name);
    int fd = open(file_path, O_RDWR | O_CREAT, 0775);
    if (fd == -1) {
        perror("open");
        free(file_path);
        return -1;
    }
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "NOMBRE_ARCHIVO=%s\n", file_name);
    fprintf(fp, "TAMANIO_ARCHIVO=%d\n", 0);
    fprintf(fp, "PUNTERO_DIRECTO=%d\n", 0);
    fprintf(fp, "PUNTERO_INDIRECTO=%d\n", 0);
    fclose(fp);
    close(fd);
    *fcb = malloc(sizeof(FCB));
    (*fcb)->file_path = file_path;
    (*fcb)->file_name = strdup(file_name);
    (*fcb)->file_size = 0;
    (*fcb)->dptr = 0;
    (*fcb)->iptr = 0;
    return 0;
}

int fcb_update(FCB *fcb) {
    int fd = open(fcb->file_path, O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    ftruncate(fd, 0);
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "NOMBRE_ARCHIVO=%s\n", fcb->file_name);
    fprintf(fp, "TAMANIO_ARCHIVO=%d\n", fcb->file_size);
    fprintf(fp, "PUNTERO_DIRECTO=%d\n", fcb->dptr);
    fprintf(fp, "PUNTERO_INDIRECTO=%d\n", fcb->iptr);
    fclose(fp);
    close(fd);
    return 0;
}

int fcb_create_from_file(char *file_name, FS *fs, FCB **fcb) {
    char *file_path = string_from_format("%s%c%s", fs->config->PATH_FCB, '/', file_name);
    if (access(file_path, F_OK) == -1) {
        perror("access");
        free(file_path);
        return -1;
    }
    t_config *config = config_create(file_path);
    *fcb = malloc(sizeof(FCB));
    (*fcb)->file_path = file_path; 
    (*fcb)->file_name = strdup(config_get_string_value(config, "NOMBRE_ARCHIVO"));
    (*fcb)->file_size = config_get_int_value(config, "TAMANIO_ARCHIVO");
    (*fcb)->dptr = (uint32_t)config_get_int_value(config, "PUNTERO_DIRECTO");
    (*fcb)->iptr = (uint32_t)config_get_int_value(config, "PUNTERO_INDIRECTO");
    config_destroy(config);
    return 0;
}

void fcb_destroy(FCB *fcb) {
    free(fcb->file_path);
    free(fcb->file_name);
    free(fcb);
}

int block_count(int size, FS *fs) {
    return (size + fs->superbloque->BLOCK_SIZE - 1) / fs->superbloque->BLOCK_SIZE;
}

int block_index(uint32_t local_address, FS *fs) {
    return local_address / fs->superbloque->BLOCK_SIZE;
}

uint32_t block_local_address(uint32_t block_index, FS *fs) {
    return block_index * fs->superbloque->BLOCK_SIZE;
}

uintptr_t block_address(uint32_t block_index, FS *fs) {
    return (uintptr_t)fs->bloques + block_local_address(block_index, fs);
}

uintptr_t ptr_address(uint32_t local_adress, FS *fs) {
    return (uintptr_t)fs->bloques + local_adress;
}

uint32_t ptr_local_address(int file_size, FS *fs) {
    int count = block_count(file_size, fs);
    assert(count > 1); // because of the direct pointer block
    return (count - 2) * sizeof(uint32_t);
}

int bitarray_count_free(t_bitarray *bitarray) {
    int count = 0;
    for (int i = 0; i < bitarray->size * 8; i++) {
        if (!bitarray_test_bit(bitarray, i))
            count++;
    }
    return count;
}

int bitarray_next_free(FS *fs) {
    for (int i = 0; i < fs->bitmap->size * 8; i++) {
        log_trace(fs->log, "Acceso a Bitmap - Bloque: %d - Estado: %d", 
            i, bitarray_test_bit(fs->bitmap, i)
        );
        if (!bitarray_test_bit(fs->bitmap, i))
            return i;
    }
    return -1;
}

int fcb_alloc(int size, FCB *fcb, FS *fs) {
    int free_blocks = bitarray_count_free(fs->bitmap);
    int blocks_reserved = block_count(fcb->file_size, fs);
    int blocks_required = block_count(fcb->file_size + size, fs) - blocks_reserved;
    if (blocks_required > free_blocks) {
        return -1;
    }
    if (blocks_reserved == 0 && blocks_required > 0) {
        int free_block = bitarray_next_free(fs);
        assert(free_block != -1);
        bitarray_set_bit(fs->bitmap, free_block);
        fcb->dptr = block_local_address(free_block, fs);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_reserved, block_index(fcb->iptr, fs)
        );
        blocks_reserved ++;
        blocks_required --;
    }
    if (blocks_reserved == 1 && blocks_required > 0) {
        int free_block = bitarray_next_free(fs);
        assert(free_block != -1);
        bitarray_set_bit(fs->bitmap, free_block);
        fcb->iptr = block_local_address(free_block, fs);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_reserved, block_index(fcb->iptr, fs)
        );
    }
    while (blocks_reserved >= 1 && blocks_required > 0) {
        int free_block = bitarray_next_free(fs);
        assert(free_block != -1);
        bitarray_set_bit(fs->bitmap, free_block);
        uintptr_t iptr_offset_address = ptr_address(fcb->iptr, fs) + blocks_reserved - 1;
        uint32_t free_block_local_address = block_local_address(free_block, fs);
        *(uint32_t *)iptr_offset_address = free_block_local_address;
        blocks_reserved ++;
        blocks_required --;
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_reserved, block_index(free_block_local_address, fs)
        );
    }
    fcb->file_size += size;
    return fcb_update(fcb);
}

int fcb_realloc(int new_size, FCB *fcb, FS *fs) {
    int difference = new_size - fcb->file_size;
    if (difference > 0) {
        return fcb_alloc(difference, fcb, fs);
    } else if (difference < 0) {
        return fcb_dealloc(-difference, fcb, fs);
    }
    return 0;
}

int fcb_dealloc(int size, FCB *fcb, FS *fs) {
    int blocks_reserved = block_count(fcb->file_size, fs);
    int blocks_required = blocks_reserved - block_count(fcb->file_size - size, fs);
    if (blocks_required > blocks_reserved) {
        return -1;
    }
    while (blocks_reserved > 2 && blocks_required > 0) {
        uint32_t local_adress = *((uint32_t *)ptr_address(fcb->iptr, fs) + blocks_reserved - 2);
        bitarray_clean_bit(fs->bitmap, block_index(local_adress, fs));
        log_trace(fs->log, "Acceso a Bitmap - Bloque: %d - Estado: %d", 
            block_index(local_adress, fs), bitarray_test_bit(fs->bitmap, block_index(local_adress, fs))
        );
        blocks_reserved --;
        blocks_required --;
    }
    if (blocks_reserved == 2 && blocks_required > 0) {
        uint32_t local_adress = *((uint32_t *)ptr_address(fcb->iptr, fs) + blocks_reserved - 2);
        bitarray_clean_bit(fs->bitmap, block_index(local_adress, fs));
        log_trace(fs->log, "Acceso a Bitmap - Bloque: %d - Estado: %d", 
            block_index(local_adress, fs), bitarray_test_bit(fs->bitmap, block_index(local_adress, fs))
        );
        bitarray_clean_bit(fs->bitmap, block_index(fcb->iptr, fs));
        log_trace(fs->log, "Acceso a Bitmap - Bloque: %d - Estado: %d", 
            block_index(fcb->iptr, fs), bitarray_test_bit(fs->bitmap, block_index(fcb->iptr, fs))
        );
        fcb->iptr = 0;
        blocks_reserved --;
        blocks_required --;
    }
    if (blocks_reserved == 1 && blocks_required > 0) {
        bitarray_clean_bit(fs->bitmap, block_index(fcb->dptr, fs));
        log_trace(fs->log, "Acceso a Bitmap - Bloque: %d - Estado: %d", 
            block_index(fcb->dptr, fs), bitarray_test_bit(fs->bitmap, block_index(fcb->dptr, fs))
        );
        fcb->dptr = 0;
        blocks_reserved --;
        blocks_required --;
    }
    fcb->file_size -= size;
    return fcb_update(fcb);
}

Superbloque *superbloque_create_from_file(char *file_path) {
    assert(access(file_path, F_OK) == 0);
    t_config *t_config = config_create(file_path);
    Superbloque *superbloque = malloc(sizeof(Superbloque));
    superbloque->BLOCK_SIZE = config_get_int_value(t_config, "BLOCK_SIZE");
    superbloque->BLOCK_COUNT = config_get_int_value(t_config, "BLOCK_COUNT");
    config_destroy(t_config);
    return superbloque;
}

void superbloque_destroy(Superbloque *superbloque) {
    free(superbloque);
}

int f_open(char *file_name, FS *fs) {
    
    log_trace(fs->log, "Abrir Archivo: %s", file_name);
    
    char *file_path = string_from_format("%s%c%s", fs->config->PATH_FCB, '/', file_name);
    if (access(file_path, F_OK) == -1) {
        log_warning(fs->log, "No existe el archivo: %s", file_name);
        free(file_path);
        return -1;
    }
    free(file_path);
    return 0;
}

int f_create(char *file_name, FS *fs) {
    
    log_trace(fs->log, "Crear Archivo: %s", file_name);
    
    char *file_path = string_from_format("%s%c%s", fs->config->PATH_FCB, '/', file_name);
    if (access(file_path, F_OK) == 0) {
        log_warning(fs->log, "Ya existe el archivo: %s", file_name);
        free(file_path);
        return -1;
    }
    FCB *fcb;
    fcb_create(file_name, fs, &fcb);
    fcb_destroy(fcb);
    free(file_path);
    return 0;
}

int f_truncate(char *file_name, int size, FS *fs) {
    
    log_trace(fs->log, "Truncar Archivo: %s - size: %d", file_name, size);
    
    FCB *fcb;
    if (fcb_create_from_file(file_name, fs, &fcb) == -1) {
        log_warning(fs->log, "No existe el archivo: %s", file_name);
        return -1;
    }
    fcb_realloc(size, fcb, fs);
    fcb_destroy(fcb);

    return 0;
}

int size_to_read(int size, int blocks_read, FS *fs) {
    int bytes_to_read = size - blocks_read * fs->superbloque->BLOCK_SIZE;
    return bytes_to_read < fs->superbloque->BLOCK_SIZE 
        ? bytes_to_read
        : fs->superbloque->BLOCK_SIZE;
}

int f_read(char *file_name, int offset, int size, int dir, void **buffer, FS *fs) {

    log_error(fs->log, "Leer Archivo: %s - Puntero: %d - Memoria: %d - Tamaño: %d", file_name, offset, dir, size);

    FCB *fcb;
    if (fcb_create_from_file(file_name, fs, &fcb) == -1) {
        log_warning(fs->log, "No existe el archivo: %s", file_name);
        return -1;
    }
    if (offset + size > fcb->file_size) {
        log_warning(fs->log, "El archivo: %s no tiene %d bytes", file_name, offset + size);
        fcb_destroy(fcb);
        return -1;
    }
    *buffer = malloc(size);
    int blocks_required = block_count(size, fs);
    int blocks_offset = block_count(offset, fs);
    int blocks_read = 0;
    while (blocks_offset > 0 && blocks_read < blocks_required) {
        uint32_t local_adress = *((uint32_t *)ptr_address(fcb->iptr, fs) + blocks_offset - 1);
        sleep(fs->config->RETARDO_ACCESO_BLOQUE/1000);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_offset + blocks_read, block_index(local_adress, fs)
        );
        memcpy(
            *buffer + blocks_read * fs->superbloque->BLOCK_SIZE,
            (void *)ptr_address(local_adress, fs),
            size_to_read(size, blocks_read, fs)
        );
        blocks_offset --;
        blocks_read ++;
    }
    if (blocks_offset == 0 && blocks_read < blocks_required) {
        uint32_t local_adress = fcb->dptr;
        sleep(fs->config->RETARDO_ACCESO_BLOQUE/1000);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_offset + blocks_read, block_index(local_adress, fs)
        );

        memcpy(
            *buffer + blocks_read * fs->superbloque->BLOCK_SIZE,
            (void *)ptr_address(local_adress, fs),
            size_to_read(size, blocks_read, fs)
        );
        blocks_read ++;
    }
    while (blocks_offset == 0 && blocks_read < blocks_required) {
        uint32_t local_adress = *((uint32_t *)ptr_address(fcb->iptr, fs) + blocks_read - 1);
        sleep(fs->config->RETARDO_ACCESO_BLOQUE/1000);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_offset + blocks_read, block_index(local_adress, fs)
        );
        memcpy(
            *buffer + blocks_read * fs->superbloque->BLOCK_SIZE,
            (void *)ptr_address(local_adress, fs),
            size_to_read(size, blocks_read, fs)
        );
        blocks_read ++;
    }
    fcb_destroy(fcb);
    return 0;
}

int f_write(char *file_name, int offset, int size, int dir, void *buffer, FS *fs) {
    
    log_error(fs->log, "Escribir Archivo: %s - Puntero: %d - Memoria: %d - Tamaño: %d", file_name, offset, dir, size);
    
    FCB *fcb;
    if (fcb_create_from_file(file_name, fs, &fcb) == -1) {
        log_warning(fs->log, "No existe el archivo: %s", file_name);
        return -1;
    }
    if (offset + size > fcb->file_size) {
        log_warning(fs->log, "El archivo: %s no tiene %d bytes", file_name, offset + size);
        fcb_destroy(fcb);
        return -1;
    }
    int blocks_required = block_count(size, fs);
    int blocks_offset = block_count(offset, fs);
    int blocks_written = 0;
    while (blocks_offset > 0 && blocks_written < blocks_required) {
        uint32_t local_adress = *((uint32_t *)ptr_address(fcb->iptr, fs) + blocks_offset - 1);
        sleep(fs->config->RETARDO_ACCESO_BLOQUE/1000);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_offset + blocks_written, block_index(local_adress, fs)
        );
        memcpy((void *)ptr_address(local_adress, fs), buffer + blocks_written * fs->superbloque->BLOCK_SIZE, fs->superbloque->BLOCK_SIZE);
        blocks_offset --;
        blocks_written ++;
    }
    if (blocks_offset == 0 && blocks_written < blocks_required) {
        uint32_t local_adress = fcb->dptr;
        sleep(fs->config->RETARDO_ACCESO_BLOQUE/1000);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_offset + blocks_written, block_index(local_adress, fs)
        );
        memcpy((void *)ptr_address(local_adress, fs), buffer + blocks_written * fs->superbloque->BLOCK_SIZE, fs->superbloque->BLOCK_SIZE);
        blocks_written ++;
    }
    while (blocks_offset == 0 && blocks_written < blocks_required) {
        uint32_t local_adress = *((uint32_t *)ptr_address(fcb->iptr, fs) + blocks_written - 1);
        sleep(fs->config->RETARDO_ACCESO_BLOQUE/1000);
        log_trace(fs->log, 
            "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d", 
            fcb->file_name, blocks_offset + blocks_written, block_index(local_adress, fs)
        );
        memcpy((void *)ptr_address(local_adress, fs), buffer + blocks_written * fs->superbloque->BLOCK_SIZE, fs->superbloque->BLOCK_SIZE);
        blocks_written ++;
    }
    fcb_destroy(fcb);
    return 0;
}

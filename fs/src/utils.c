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

Disk *disk_create(fs_config *config) {
    int error = 0;
    Disk *disk = malloc(sizeof(Disk));
    disk->superbloque = superbloque_create_from_file(config->PATH_SUPERBLOQUE);
    disk->bloques = malloc(data_byte_count(disk->superbloque));

    if (access(config->PATH_BITMAP, F_OK) != -1) {
        int fd = open(config->PATH_BITMAP, O_RDONLY);
        char *content = malloc(bitmap_byte_count(disk->superbloque));
        error = read(fd, content, bitmap_byte_count(disk->superbloque));
        assert(error == bitmap_byte_count(disk->superbloque));
        disk->bitmap = bitarray_create_with_mode(content, disk->superbloque->BLOCK_COUNT, MSB_FIRST);
        close(fd);
    } else {
        char *zeroes = calloc(bitmap_byte_count(disk->superbloque), 8);
        disk->bitmap = bitarray_create_with_mode(zeroes, bitmap_byte_count(disk->superbloque), MSB_FIRST);
    }

    error = mmap_file_sync(config->PATH_BITMAP, bitmap_byte_count(disk->superbloque), &disk->bitmap->bitarray);
    assert(error != -1);

    error = mmap_file_sync(config->PATH_BLOQUES, data_byte_count(disk->superbloque), &disk->bloques);
    assert(error != -1);

    return disk;
}

void disk_destroy(Disk *disk) {
    superbloque_destroy(disk->superbloque);
    bitarray_destroy(disk->bitmap);
    free(disk->bloques);
    free(disk);
}

int mmap_file_sync(char *file_name, int length, char **file_content){

    struct stat file_stat;

    int fd = open(file_name, O_RDWR | O_CREAT, 0775);
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

FCB *fcb_create(char *file_name) {
    FCB *fcb = malloc(sizeof(FCB));
    fcb->file_name = strdup(file_name);
    fcb->file_size = 0;
    fcb->dptr = 0;
    fcb->iptr = 0;
    return fcb;
}

FCB *fcb_create_from_file(char *path, Disk *disk) {
    assert(access(path, F_OK) == 0);

    t_config *config = config_create(path);
    FCB *fcb = malloc(sizeof(FCB));
    fcb->file_name = strdup(config_get_string_value(config, "NOMBRE_ARCHIVO"));
    fcb->file_size = config_get_int_value(config, "TAMANIO_ARCHIVO");
    int dptr = config_get_int_value(config, "PUNTERO_DIRECTO");
    int iptr = config_get_int_value(config, "PUNTERO_INDIRECTO");
    config_destroy(config);

    fcb->dptr = (uint32_t)(uintptr_t)disk->bloques + dptr * disk->superbloque->BLOCK_SIZE;
    fcb->iptr = (uint32_t)(uintptr_t)disk->bloques + iptr * disk->superbloque->BLOCK_SIZE;

    return fcb;
}

void fcb_destroy(FCB *fcb) {
    free(fcb->file_name);
    free(fcb);
}

int blocks_count(int size, Disk *disk) {
    return (size + disk->superbloque->BLOCK_SIZE - 1) / disk->superbloque->BLOCK_SIZE;
}

int blocks_allocated(FCB *fcb, Disk *disk) {
    return blocks_count(fcb->file_size, disk);
}

uintptr_t block_address(uint32_t index, Disk *disk) {
    return (uintptr_t)disk->bloques + index * disk->superbloque->BLOCK_SIZE;
}

uint32_t iptr_offset(int file_size, Disk *disk) {
    int block_count = blocks_count(file_size, disk);
    assert(block_count > 2);
    return (block_count - 2) * sizeof(uint32_t);
}

int bitarray_count_free(t_bitarray *bitarray) {
    int count = 0;
    for (int i = 0; i < bitarray->size; i++) {
        if (!bitarray_test_bit(bitarray, i))
            count++;
    }
    return count;
}

int bitarray_next_free(t_bitarray *bitarray) {
    for (int i = 0; i < bitarray->size; i++) {
        if (!bitarray_test_bit(bitarray, i))
            return i;
    }
    return -1;
}

int fcb_alloc(int size, FCB *fcb, Disk *disk) {
    int free_blocks = bitarray_count_free(disk->bitmap);
    int blocks_needed = blocks_count(size, disk);
    if (blocks_needed > free_blocks) {
        return -1;
    }
    if (blocks_allocated(fcb, disk) == 0 && blocks_needed > 0) {
        int free_block = bitarray_next_free(disk->bitmap);
        if (free_block == -1)
            return -1;
        bitarray_set_bit(disk->bitmap, free_block);
        fcb->dptr = block_address(free_block, disk);
        fcb->file_size += disk->superbloque->BLOCK_SIZE;
        blocks_needed --;
    }
    if (blocks_allocated(fcb, disk) == 1 && blocks_needed > 0) {
        int free_block = bitarray_next_free(disk->bitmap);
        if (free_block == -1)
            return -1;
        bitarray_set_bit(disk->bitmap, free_block);
        fcb->iptr = block_address(free_block, disk);
    }
    while (blocks_allocated(fcb, disk) > 1 && blocks_needed > 0) {
        int free_block = bitarray_next_free(disk->bitmap);
        if (free_block == -1)
            return -1;
        bitarray_set_bit(disk->bitmap, free_block);
        memset((void *)(uintptr_t)(uint32_t)block_address(fcb->iptr, disk) + iptr_offset(fcb->file_size, disk), block_address(free_block, disk), sizeof(uint32_t));
        fcb->file_size += disk->superbloque->BLOCK_SIZE;
        blocks_needed --;
    }
    return 0;
}

int fcb_realloc(int new_size, FCB *fcb, Disk *disk) {
    int difference = new_size - fcb->file_size;
    if (difference > 0) {
        fcb_alloc(difference, fcb, disk);
    } else if (difference < 0) {
        fcb_dealloc(-difference, fcb, disk);
    }
    return 0;
}

int fcb_dealloc(int size, FCB *fcb, Disk *disk) {
    int allocated_blocks = blocks_allocated(fcb, disk);
    int blocks_needed = blocks_count(size, disk);
    if (blocks_needed > allocated_blocks) {
        return -1;
    }
    while (blocks_allocated(fcb, disk) > 2 && blocks_needed > 0) {
        uint32_t *block_index = (uint32_t *)block_address(fcb->iptr, disk) + iptr_offset(fcb->file_size, disk);
        bitarray_clean_bit(disk->bitmap, *block_index);
        fcb->file_size -= disk->superbloque->BLOCK_SIZE;
        blocks_needed --;
    }
    if (blocks_allocated(fcb, disk) == 2 && blocks_needed > 0) {
        uint32_t *block_index = (uint32_t *)block_address(fcb->iptr, disk) + iptr_offset(fcb->file_size, disk);
        bitarray_clean_bit(disk->bitmap, *block_index);
        bitarray_clean_bit(disk->bitmap, fcb->iptr);
        fcb->file_size -= disk->superbloque->BLOCK_SIZE;
        blocks_needed --;
    }
    if (blocks_allocated(fcb, disk) == 1 && blocks_needed > 0) {
        bitarray_clean_bit(disk->bitmap, fcb->dptr);
        fcb->file_size -= disk->superbloque->BLOCK_SIZE;
        blocks_needed --;
    }
    return 0;
}

Superbloque *superbloque_create_from_file(char *path) {
    assert(access(path, F_OK) == 0);
    t_config *t_config = config_create(path);
    Superbloque *superbloque = malloc(sizeof(Superbloque));
    superbloque->BLOCK_SIZE = config_get_int_value(t_config, "BLOCK_SIZE");
    superbloque->BLOCK_COUNT = config_get_int_value(t_config, "BLOCK_COUNT");
    config_destroy(t_config);
    return superbloque;
}

void superbloque_destroy(Superbloque *superbloque) {
    free(superbloque);
}

int f_open(char *file_name, FCB **fcb) {

    int fd = open(file_name, O_RDWR | O_CREAT, 0775);
    assert(fd != -1);

    struct stat file_stat;
// NOMBRE_ARCHIVO=Notas1erParcialK9999
// TAMANIO_ARCHIVO=256
// PUNTERO_DIRECTO=12
// PUNTERO_INDIRECTO=45

    assert(fstat(fd, &file_stat) != -1);

    if (file_stat.st_size == 0) {
        *fcb = fcb_create(file_name);
        FILE *fp = fdopen(fd, "w");
        fprintf(fp, "NOMBRE_ARCHIVO=%s\n", file_name);
        fprintf(fp, "TAMANIO_ARCHIVO=%d\n", 0);
        fprintf(fp, "PUNTERO_DIRECTO=%d\n", 0);
        fprintf(fp, "PUNTERO_INDIRECTO=%d\n", 0);
        fclose(fp);
        (*fcb)->file_size = 0;
        (*fcb)->dptr = 0;
        (*fcb)->iptr = 0;
    } else {
        *fcb = fcb_create(file_name);
        FILE *fp = fdopen(fd, "r");
        fclose(fp);
    }

    assert(close(fd) != -1);

    return 0;
}

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
    fcb->dptr = NULL;
    fcb->iptr = list_create();
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
    
    fcb->dptr = disk->bloques + dptr * disk->superbloque->BLOCK_SIZE;
    fcb->iptr = list_create();
    for (int i = 0; i < disk->superbloque->BLOCK_SIZE / sizeof(int); i++) {
        char **ptr = malloc(sizeof(char));
        *ptr = disk->bloques + (iptr * disk->superbloque->BLOCK_SIZE + i * sizeof(int));
        list_add(fcb->iptr, ptr);
    }
    return fcb;
}

void fcb_destroy(FCB *fcb) {
    free(fcb->file_name);
    free(fcb->dptr);
    free(fcb->iptr);
    free(fcb);
}

// reallocates fcb new_size blocks
// first accumulates the difference between current_size and new_size
// if not enough blocks are available, returns -1
// then allocates or deallocates blocks depending on the sign of the difference
// always allocates and deallocates from the end of the file
int fcb_realloc(int new_size, FCB *fcb, Disk *disk) {
    // int difference = new_size - fcb->file_size;
    // if (difference > 0) {
    //     // alloc
    //     for (int i = 0; i < difference; i++) {
    //         int block_index = bitarray_scan(fcb_table->bitmap, 0, 1, 0);
    //         if (block_index == -1)
    //             return -1;
    //         char *block_ptr = fcb_table->block_store + block_index * fcb_table->block_size;
    //         list_add(fcb->iptr, block_ptr);
    //         bitarray_set_bit(fcb_table->bitmap, block_index);
    //     }
    // } else if (difference < 0) {
    //     // dealloc
    //     for (int i = 0; i < -difference; i++) {
    //         char *block_ptr = list_remove(fcb->iptr, list_size(fcb->iptr) - 1);
    //         bitarray_clean_bit(fcb_table->bitmap, (block_ptr - fcb_table->block_store) / fcb_table->block_size);
    //     }
    // }
    // fcb->file_size = new_size;
    return 0;
}

int fcb_dealloc(FCB *fcb, Disk *disk) {
    t_list_iterator *iterator = list_iterator_create(fcb->iptr);
    while(list_iterator_has_next(iterator)) {
        char *block_ptr = list_iterator_next(iterator);
        memset(block_ptr, 0, disk->superbloque->BLOCK_SIZE);
        bitarray_clean_bit(disk->bitmap, (block_ptr - disk->bloques) / disk->superbloque->BLOCK_SIZE);
        list_remove(fcb->iptr, list_iterator_index(iterator));
    }
    list_iterator_destroy(iterator);
    return 0;
}

// Function to find a free block in the FCB table
// int find_free_block(FCB_table *fcb_table) {
//     int i;
//     for (i = 0; i < fcb_table->bit_count; i++) {
//         // Check if the bit is free (0)
//         if ((fcb_table->bitmap[i / 8] & (1 << (i % 8))) == 0) {
//             // Mark the bit as used in the bitmap
//             fcb_table->bitmap[i / 8] |= (1 << (i % 8));

//             // Calculate the block index and offset
//             int block_index = i / (fcb_table->block_size * 8);
//             int offset = i % (fcb_table->block_size * 8);

//             // Set the indirect pointer to point to the block
//             fcb_table->iptr[block_index] = fcb_table->block_store + (block_index * fcb_table->block_size);

//             // Update metadata
//             fcb_table->byte_count += fcb_table->block_size;
//             fcb_table->block_count++;

//             return i;
//         }
//     }

//     // No free block found
//     return -1;
// }

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
        (*fcb)->iptr = NULL;
    } else {
        *fcb = fcb_create(file_name);
        FILE *fp = fdopen(fd, "r");
        fclose(fp);
    }

    assert(close(fd) != -1);

    return 0;
}

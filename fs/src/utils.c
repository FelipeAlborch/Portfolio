#include <utils.h>

void print_cwd() {
    printf("PWD = %s\n", getcwd(NULL, 0));
}

int data_byte_count(FCB_table *fcb_table) {
    return fcb_table->block_size * fcb_table->block_count;
}

int bitmap_byte_count(FCB_table *fcb_table) {
    return (fcb_table->block_count + 7) / 8;
}

int fcb_table_init(FCB_table **fcb_table_, fs_config *config) {
    int error = 0;
    *fcb_table_ = malloc(sizeof(FCB_table));
    FCB_table *fcb_table = *fcb_table_;
    fcb_table->index = dictionary_create();
    t_superbloque *superbloque = superbloque_create_from_file(config->PATH_SUPERBLOQUE);
    fcb_table->block_size = superbloque->BLOCK_SIZE;
    fcb_table->block_count = superbloque->BLOCK_COUNT;
    superbloque_destroy(superbloque);

    if (access(config->PATH_BITMAP, F_OK) != -1) {
        int fd = open(config->PATH_BITMAP, O_RDONLY);
        char *content = malloc(bitmap_byte_count(fcb_table));
        read(fd, content, bitmap_byte_count(fcb_table));
        fcb_table->bitmap = bitarray_create_with_mode(content, bitmap_byte_count(fcb_table), MSB_FIRST);
        close(fd);
    } else {
        char *zeroes = calloc(bitmap_byte_count(fcb_table), 8);
        fcb_table->bitmap = bitarray_create_with_mode(zeroes, bitmap_byte_count(fcb_table), MSB_FIRST);
    }

    error = mmap_file_sync(config->PATH_BITMAP, bitmap_byte_count(fcb_table), &fcb_table->bitmap->bitarray);
    if (error == -1) { 
        perror("mmap_file_sync");
        return error;
    }

    error = mmap_file_sync(config->PATH_BLOQUES, data_byte_count(fcb_table), &fcb_table->block_store);
    if (error == -1) { 
        perror("mmap_file_sync");
        return error;
    }

    return error;
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
    fcb->iptr = NULL;
    return fcb;
}

FCB *fcb_create_from_file(char *path) {
    assert(access(path, F_OK) == 0);
    t_config *t_config = config_create(path);
    FCB *fcb = malloc(sizeof(FCB));
    fcb->file_name = strdup(config_get_string_value(t_config, "NOMBRE_ARCHIVO"));
    fcb->file_size = config_get_int_value(t_config, "TAMANIO_ARCHIVO");
    fcb->dptr = (char *)config_get_long_value(t_config, "PUNTERO_DIRECTO");
    fcb->iptr = (uint32_t *)config_get_long_value(t_config, "PUNTERO_DIRECTO");
    config_destroy(t_config);
    return fcb;
}

int fcb_destroy(FCB *fcb) {
    free(fcb->file_name);
    free(fcb->dptr);
    free(fcb->iptr);
    free(fcb);
    return 0;
}

int fcb_table_destroy(FCB_table *fcb_table) {
    munmap(fcb_table->bitmap->bitarray, bitmap_byte_count(fcb_table));
    bitarray_destroy(fcb_table->bitmap);
    munmap(fcb_table->block_store, data_byte_count(fcb_table));
    dictionary_destroy_and_destroy_elements(fcb_table->index, (void(*)(void *))fcb_destroy);
    free(fcb_table);
    return 0;
}

// reallocates fcb new_size blocks
// first accumulates the difference between current_size and new_size
// if not enough blocks are available, returns -1
// then allocates or deallocates blocks depending on the sign of the difference
// always allocates and deallocates from the end of the file
int fcb_realloc(int new_size, FCB *fcb, FCB_table *fcb_table) {
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

int fcb_dealloc(FCB *fcb, FCB_table *fcb_table) {
    t_list_iterator *iterator = list_iterator_create(fcb->iptr);
    while(list_iterator_has_next(iterator)) {
        char *block_ptr = list_iterator_next(iterator);
        // dealloc block store
        memset(block_ptr, 0, fcb_table->block_size);
        // dealloc block bitmap
        bitarray_clean_bit(fcb_table->bitmap, (block_ptr - fcb_table->block_store) / fcb_table->block_size);
        list_remove(fcb->iptr, list_iterator_index(iterator));
    }
    return 0;
}

int fcb_table_add(FCB *fcb, FCB_table *fcb_table) {
    if (dictionary_has_key(fcb_table->index, fcb->file_name))
        return -1;
    dictionary_put(fcb_table->index, fcb->file_name, fcb);
    return 0;
}

int fcb_table_remove(char *file_name, FCB_table *fcb_table) {
    if (!dictionary_has_key(fcb_table->index, file_name))
        return -1;
    FCB *fcb = dictionary_get(fcb_table->index, file_name);
    dictionary_remove(fcb_table->index, file_name);
    fcb_destroy(fcb);
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

t_superbloque *superbloque_create_from_file(char *path) {
    assert(access(path, F_OK) == 0);
    t_config *t_config = config_create(path);
    t_superbloque *superbloque = malloc(sizeof(t_superbloque));
    superbloque->BLOCK_SIZE = config_get_int_value(t_config, "BLOCK_SIZE");
    superbloque->BLOCK_COUNT = config_get_int_value(t_config, "BLOCK_COUNT");
    config_destroy(t_config);
    return superbloque;
}

void superbloque_destroy(t_superbloque *superbloque) {
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

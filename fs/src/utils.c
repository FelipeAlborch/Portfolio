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

int fcb_table_init(FCB_table *fcb_table, char *bitmap_file, char *data_file, int block_size, int block_count) {
    int error = 0;
    fcb_table->block_size = block_size;
    fcb_table->block_count = block_count;
    fcb_table->index = dictionary_create();

    if (access(bitmap_file, F_OK) == 0) {
        int fd = open(bitmap_file, O_RDONLY);
        char *content = malloc(bitmap_byte_count(fcb_table));
        read(fd, content, bitmap_byte_count(fcb_table));
        fcb_table->bitmap = bitarray_create_with_mode(content, bitmap_byte_count(fcb_table), MSB_FIRST);
        close(fd);
    } else {
        char *zeroes = calloc(bitmap_byte_count(fcb_table), 8);
        fcb_table->bitmap = bitarray_create_with_mode(zeroes, bitmap_byte_count(fcb_table), MSB_FIRST);
    }

    error = mmap_file_sync(bitmap_file, bitmap_byte_count(fcb_table), &fcb_table->bitmap->bitarray);
    if (error == -1) { 
        perror("mmap_file_sync");
        return error;
    }

    error = mmap_file_sync(data_file, data_byte_count(fcb_table), &fcb_table->block_store);
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
    fcb->iptr = list_create();
    return fcb;
}

int fcb_destroy(FCB *fcb) {
    free(fcb->file_name);
    list_destroy(fcb->iptr);
    free(fcb);
    return 0;
}

int fcb_table_destroy(FCB_table *fcb_table) {
    munmap(fcb_table->bitmap->bitarray, bitmap_byte_count(fcb_table));
    bitarray_destroy(fcb_table->bitmap);
    munmap(fcb_table->block_store, data_byte_count(fcb_table));
    dictionary_destroy_and_destroy_elements(fcb_table->index, (void(*)(void *))fcb_destroy);
    return 0;
}

int fcb_realloc(int new_size, FCB *fcb, FCB_table *fcb_table) {
    if (list_size(fcb->iptr) * fcb_table->block_size < new_size) {
        for (int i = 0; i < fcb_table->block_count; i++) {
            if (bitarray_test_bit(fcb_table->bitmap, i) == 0) {
                // Check if we have enough blocks
                if (list_size(fcb->iptr) * fcb_table->block_size >= new_size) {
                    return 0;
                }
                // Set the bit to 1
                bitarray_set_bit(fcb_table->bitmap, i);
                // Add the block to the FCB
                list_add(fcb->iptr, fcb_table->block_store + (i * fcb_table->block_size));
            }
        }
        return 0;
    } else if (list_size(fcb->iptr) * fcb_table->block_size > new_size) {

    }  
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

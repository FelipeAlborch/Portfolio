#include <utils.h>

void print_cwd() {
    printf("PWD = %s\n", getcwd(NULL, 0));
}

 int fcb_table_init(FCB_table *fcb_table, char *block_store_file, int block_size, int block_count) {
    int error = 0;
    fcb_table->bit_count = block_size * block_count;
    fcb_table->byte_count = (fcb_table->bit_count + 7) / 8;
    fcb_table->block_size = block_size;
    fcb_table->block_count = block_count;
    fcb_table->index = dictionary_create();
    void* bits = calloc(fcb_table->bit_count, 1);
    fcb_table->bitmap = bitarray_create_with_mode(bits, fcb_table->byte_count, MSB_FIRST);
    error = mmap_file_sync(block_store_file, fcb_table->bit_count, &fcb_table->block_store);
    if (error == -1) { 
        perror("mmap_file_sync");
        return error;
    }
    return error;
}

int fcb_table_destroy(FCB_table *fcb_table) {
    dictionary_destroy(fcb_table->index);
    bitarray_destroy(fcb_table->bitmap);
    munmap(fcb_table->block_store, fcb_table->bit_count);
    return 0;
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
    fcb->direct_ptr = NULL;
    fcb->indirect_ptr = NULL;
    return fcb;
}

int fcb_destroy(FCB *fcb) {
    free(fcb);
    return 0;
}

int fcb_realloc(FCB *fcb, FCB_table *fcb_table, int new_size) {
    // TODO: realloc
    return 0;
}

int fcb_dealloc(FCB *fcb, FCB_table *fcb_table) {
    // TODO: dealloc
    return 0;
}

int fcb_table_add(FCB_table *fcb_table, char *file_name,
    int file_size, void **direct_ptr, void **indirect_ptr) {
    if (dictionary_has_key(fcb_table->index, file_name)) return -1;
    FCB *fcb = fcb_create(file_name);
    dictionary_put(fcb_table->index, file_name, fcb);
    return 0;
}

int fcb_table_remove(FCB_table *fcb_table, char *file_name) {
    if (!dictionary_has_key(fcb_table->index, file_name)) return -1;
    FCB *fcb = dictionary_get(fcb_table->index, file_name);
    dictionary_remove(fcb_table->index, file_name);
    fcb_destroy(fcb);
    return 0;
}

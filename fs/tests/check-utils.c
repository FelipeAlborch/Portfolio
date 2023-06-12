#include <check.h>
#include <utils.h>
#include <config.h>

START_TEST(test_files_and_memory)
{
    FILE *fp;
    char *file_name = "test-file";
    char *file_path = "drive/fcb/test-file";
    char *path_config = "drive/test-fs.config";
    char *path_bitmap = "drive/test-bitmap.dat";
    char *path_bloques = "drive/test-bloques.dat";
    char *path_superbloque = "drive/test-superbloque.dat";
    int block_size = 128;
    int block_count = 16;

    fp = fopen(path_config, "w");
    fprintf(fp, "IP_FSYSTEM=127.0.0.1\n");
    fprintf(fp, "IP_MEMORIA=127.0.0.1\n");
    fprintf(fp, "PUERTO_MEMORIA=8002\n");
    fprintf(fp, "PUERTO_ESCUCHA=8003\n");
    fprintf(fp, "PATH_SUPERBLOQUE=%s\n", path_superbloque);
    fprintf(fp, "PATH_BITMAP=%s\n", path_bitmap);
    fprintf(fp, "PATH_BLOQUES=%s\n", path_bloques);
    fprintf(fp, "PATH_FCB=drive/fcb\n");
    fprintf(fp, "RETARDO_ACCESO_BLOQUE=15000\n");
    fclose(fp);
    
    fp = fopen(path_superbloque, "w");
    fprintf(fp, "BLOCK_SIZE=%d\n", block_size);
    fprintf(fp, "BLOCK_COUNT=%d\n", block_count);
    fclose(fp);

    FCB *fcb;
    int error;
    FS *fs = fs_create(path_config);

    error = fcb_create(file_name, fs, &fcb);
    assert(error == 0);
    error = fcb_realloc(129, fcb, fs);
    assert(error == 0);
    fcb_destroy(fcb);
    error = fcb_create_from_file(file_name, fs, &fcb);
    assert(error == 0);
    ck_assert_int_eq(fcb->file_size, 129);
    ck_assert_int_eq(fcb->dptr, 0);
    ck_assert_int_eq(fcb->iptr, 128);
    fcb_destroy(fcb);

    error = fcb_create_from_file(file_name, fs, &fcb);
    assert(error == 0);
    error = fcb_realloc(140, fcb, fs);
    assert(error == 0);
    fcb_destroy(fcb);
    error = fcb_create_from_file(file_name, fs, &fcb);
    assert(error == 0);
    ck_assert_int_eq(fcb->file_size, 140);
    ck_assert_int_eq(fcb->dptr, 0);
    ck_assert_int_eq(fcb->iptr, 128);
    fcb_destroy(fcb);

    // unallocate the memory
    munmap(fs->bloques, block_size * block_count);
    munmap(fs->bitmap->bitarray, block_count);

    // delete created files
    if (access(file_path, F_OK) != -1) remove(file_path);
    if (access(path_config, F_OK) != -1) remove(path_config);
    if (access(path_bitmap, F_OK) != -1) remove(path_bitmap);
    if (access(path_bloques, F_OK) != -1) remove(path_bloques);
    if (access(path_superbloque, F_OK) != -1) remove(path_superbloque);

    ck_assert(true);
}
END_TEST

START_TEST(test_fs_config)
{
    FILE *fp = fopen("test-fs.config", "w");
    fprintf(fp, "IP_FSYSTEM=127.0.0.1\n");
    fprintf(fp, "IP_MEMORIA=127.0.0.1\n");
    fprintf(fp, "PUERTO_MEMORIA=8002\n");
    fprintf(fp, "PUERTO_ESCUCHA=8003\n");
    fprintf(fp, "PATH_SUPERBLOQUE=drive/superbloque.dat\n");
    fprintf(fp, "PATH_BITMAP=drive/bitmap.dat\n");
    fprintf(fp, "PATH_BLOQUES=drive/bloques.dat\n");
    fprintf(fp, "PATH_FCB=drive/fcb\n");
    fprintf(fp, "RETARDO_ACCESO_BLOQUE=15000\n");
    fclose(fp);

    fs_config *config = config_create_fs_from_file("test-fs.config");
    ck_assert(access("test-fs.config", F_OK) != -1);
    ck_assert_str_eq(config->IP_FSYSTEM, "127.0.0.1");
    ck_assert_str_eq(config->IP_MEMORIA, "127.0.0.1");
    ck_assert_str_eq(config->PUERTO_MEMORIA, "8002");
    ck_assert_str_eq(config->PUERTO_ESCUCHA, "8003");
    ck_assert_str_eq(config->PATH_SUPERBLOQUE, "drive/superbloque.dat");
    ck_assert_str_eq(config->PATH_BITMAP, "drive/bitmap.dat");
    ck_assert_str_eq(config->PATH_BLOQUES, "drive/bloques.dat");
    ck_assert_str_eq(config->PATH_FCB, "drive/fcb");
    ck_assert_int_eq(config->RETARDO_ACCESO_BLOQUE, 15000);

    config_destroy_fs(config);

    if (access("test-fs.config", F_OK) != -1) remove("test-fs.config");

    ck_assert(access("test-fs.config", F_OK) == -1);
}
END_TEST

START_TEST(test_superbloque_config)
{
    FILE *fp = fopen("test-superbloque.dat", "w");
    fprintf(fp, "BLOCK_SIZE=64\n");
    fprintf(fp, "BLOCK_COUNT=1024\n");
    fclose(fp);

    Superbloque *superbloque = superbloque_create_from_file("test-superbloque.dat");
    ck_assert(access("test-superbloque.dat", F_OK) != -1);
    ck_assert_int_eq(superbloque->BLOCK_SIZE, 64);
    ck_assert_int_eq(superbloque->BLOCK_COUNT, 1024);

    superbloque_destroy(superbloque);

    if (access("test-superbloque.dat", F_OK) != -1) remove("test-superbloque.dat");

    ck_assert(access("test-superbloque.dat", F_OK) == -1);
}
END_TEST

START_TEST(test_fcb_table_init)
{
    // setup
    FILE *fp;
    char *path_config = "test-fs.config";
    char *path_bitmap = "test-bitmap.dat";
    char *path_bloques = "test-bloques.dat";
    char *path_superbloque = "test-superbloque.dat";
    int block_size = 100;
    int block_count = 10;

    fp = fopen(path_config, "w");
    fprintf(fp, "IP_FSYSTEM=127.0.0.1\n");
    fprintf(fp, "IP_MEMORIA=127.0.0.1\n");
    fprintf(fp, "PUERTO_MEMORIA=8002\n");
    fprintf(fp, "PUERTO_ESCUCHA=8003\n");
    fprintf(fp, "PATH_SUPERBLOQUE=%s\n", path_superbloque);
    fprintf(fp, "PATH_BITMAP=%s\n", path_bitmap);
    fprintf(fp, "PATH_BLOQUES=%s\n", path_bloques);
    fprintf(fp, "PATH_FCB=drive/fcb\n");
    fprintf(fp, "RETARDO_ACCESO_BLOQUE=15000\n");
    fclose(fp);
    
    fp = fopen(path_superbloque, "w");
    fprintf(fp, "BLOCK_SIZE=%d\n", block_size);
    fprintf(fp, "BLOCK_COUNT=%d\n", block_count);
    fclose(fp);

    FS *disk = fs_create(path_config);
    
    // check members
    ck_assert_ptr_ne(disk->bitmap, NULL);
    ck_assert_int_eq(data_byte_count(disk->superbloque), block_size * block_count);
    ck_assert_int_eq(bitmap_byte_count(disk->superbloque), (block_count + 7) / 8);
    ck_assert_ptr_ne(disk->bloques, MAP_FAILED);

    // check max bit
    int bitmap_max_bit = bitarray_get_max_bit(disk->bitmap);
    ck_assert_int_eq(bitmap_max_bit, bitmap_byte_count(disk->superbloque) * 8);

    // check memory allocation is initialized with 0
    char *zeroes = calloc(bitmap_max_bit, 1);
    int bitmap_zeroed = memcmp(disk->bitmap->bitarray, zeroes, bitmap_max_bit);
    ck_assert(bitmap_zeroed == 0);

    // unallocate the memory
    munmap(disk->bloques, block_size * block_count);
    munmap(disk->bitmap->bitarray, block_count);

    // delete created files
    if (access(path_config, F_OK) != -1) remove(path_config);
    if (access(path_bitmap, F_OK) != -1) remove(path_bitmap);
    if (access(path_bloques, F_OK) != -1) remove(path_bloques);
    if (access(path_superbloque, F_OK) != -1) remove(path_superbloque);
}
END_TEST

START_TEST(test_mmap_file_sync)
{
    int block_size = 64;
    int block_count = 65536;
    char* file_name = "test-file.txt";
    char* file_content;
    int file_size = block_size * block_count;

    // Call the function to test
    int result = mmap_file_sync(file_name, file_size, &file_content);

    // Make assertions to check the result and behavior
    ck_assert_int_eq(result, 0);
    ck_assert_ptr_ne(file_content, MAP_FAILED);

    char *hola_mundo = "hola mundo";

    // write "hola mundo" to file_content
    memcpy(file_content, hola_mundo, strlen(hola_mundo));
    // read memory and check the content
    ck_assert_str_eq(file_content, hola_mundo);

    // read the file and check the content
    int fd = open(file_name, O_RDONLY);
    char *read_file = malloc(strlen(hola_mundo));
    read(fd, read_file, strlen(hola_mundo));
    ck_assert_str_eq(read_file, hola_mundo);

    // Free the mapped memory
    if (file_content != MAP_FAILED) {
        munmap(file_content, file_size);
    }

    // if file exists remove
    if (access(file_name, F_OK) != -1) {
        remove(file_name);
    }
}
END_TEST

Suite *utils_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_mmap_file_sync);
    tcase_add_test(tc, test_fcb_table_init);
    tcase_add_test(tc, test_superbloque_config);
    tcase_add_test(tc, test_fs_config);
    tcase_add_test(tc, test_files_and_memory);
    // tcase_add_unchecked_fixture(tc, utils_unchecked_setup, utils_unchecked_teardown);
    suite_add_tcase(s, tc);

    return s;
}
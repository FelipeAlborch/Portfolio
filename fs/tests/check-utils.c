#include <check.h>
#include <utils.h>

const int BLOCK_SIZE = 64;
const int BLOCK_COUNT = 65536;

START_TEST(test_fcb_table_init)
{
    FCB_table fcb_table;
    int size = BLOCK_SIZE * BLOCK_COUNT;
    int status = fcb_table_init(&fcb_table, "test-file.txt", BLOCK_SIZE, BLOCK_COUNT);
    ck_assert_int_eq(status, 0);
    ck_assert_ptr_ne(fcb_table.index, NULL);
    ck_assert_ptr_ne(fcb_table.bitmap, NULL);
    ck_assert_int_eq(fcb_table.bit_count, size);
    ck_assert_ptr_ne(fcb_table.block_store, MAP_FAILED);

    // check max bit
    int bitmap_max_bit = bitarray_get_max_bit(fcb_table.bitmap);
    ck_assert_int_eq(bitmap_max_bit, fcb_table.bit_count);

    // check memory allocation initialized with 0
    char *zero = malloc(fcb_table.bit_count);
    memset(zero, 0, fcb_table.bit_count);
    int bitmap_zeroed = memcmp(fcb_table.bitmap->bitarray, zero, bitmap_max_bit);
    ck_assert(bitmap_zeroed == 0);

    // unallocate the memory
    if (fcb_table.block_store != MAP_FAILED) {
        munmap(fcb_table.block_store, size);
    }
}
END_TEST

START_TEST(test_mmap_file_sync)
{
    char* file_name = "test-file.txt";
    char* file_content;
    int file_size = BLOCK_SIZE * BLOCK_COUNT;

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
    // tcase_add_unchecked_fixture(tc, utils_unchecked_setup, utils_unchecked_teardown);
    suite_add_tcase(s, tc);

    return s;
}
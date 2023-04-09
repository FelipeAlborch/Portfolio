#include <check.h>
#include <ipc.h>

START_TEST (read_socket_create_test)
{

}
END_TEST

Suite *ipc_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, read_socket_create_test);
    suite_add_tcase(s, tc);

    return s;
}
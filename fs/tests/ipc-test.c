#include <check.h>
#include <ipc.h>

START_TEST (test_message_create)
{
//     char *expected = "hola";
//     t_message *message = message_create(expected);
//     char *actual = message_create(message);
//     ck_assert_str_eq(expected, actual);
}
END_TEST

Suite *ipc_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_message_create);
    suite_add_tcase(s, tc);

    return s;
}
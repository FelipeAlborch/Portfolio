#include <check.h>
#include "check-tlv.c"
#include "check-ipc.c"

Suite *test_suite(void)
{
    Suite *s = suite_create("");
    TCase *tc = tcase_create("");
    suite_add_tcase(s, tc);

    return s;
}

int main(void)
{
    int number_failed;
    SRunner *sr;

    sr = srunner_create(test_suite());
    srunner_add_suite(sr, tlv_test_suite());
    srunner_add_suite(sr, ipc_test_suite());
    // check_set_option("color", CK_TRUE);
    printf("\033[1;35m");
    srunner_run_all(sr, CK_NORMAL);
    printf("\033[0m");
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : -1;
}

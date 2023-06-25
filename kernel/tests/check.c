#include <check.h>
#include "helloTest.c"

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
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_add_suite(sr, kernel_test_suite());
    printf("\033[1;35m");
    srunner_run_all(sr, CK_VERBOSE);
    // srunner_run_all(sr, CK_NORMAL);
    printf("\033[0m");
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : -1;
}

#include <check.h>
#include "tlv-test.c"
#include "ipc-test.c"

Suite *test_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("FS tests");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_tlv_create_int);
    tcase_add_test(tc_core, test_payload_create_string);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = test_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : -1;
}

#include <check.h>
#include <tlv.h>

START_TEST (test_tlv_create_int)
{
    int expected = 10;
    t_tlv *tlv = tlv_create_int(expected);
    int actual = tlv_get_int(tlv);
    ck_assert_int_eq(expected, actual);
}
END_TEST

Suite *tlv_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_tlv_create_int);
    suite_add_tcase(s, tc);

    return s;
}
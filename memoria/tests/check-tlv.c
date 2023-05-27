#include <check.h>
#include <tlv.h>
#include <memoriaUtils.h>

START_TEST (tlv_create_int_test)
{
    int expected = 10;
    t_tlv *tlv = tlv_create_int(expected);
    int actual = tlv_get_int(tlv);
    ck_assert_int_eq(expected, actual);
}
END_TEST

START_TEST (tlv_create_seg)
{
    int pid=1;
    int tam=10;
    int id=1;
    ;
    int actual = tlv_get_seg(tlv);
    ck_assert_int_eq(1, actual);
}
END_TEST

Suite *tlv_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, tlv_create_int_test);
    tcase_add_test(tc, tlv_create_seg);
    suite_add_tcase(s, tc);

    return s;
}
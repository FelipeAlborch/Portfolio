#include <check.h>

START_TEST(test_hola_mundo)
{
  ck_assert_str_eq("hola mundo", "hola mundo");
}
END_TEST

Suite *kernel_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_hola_mundo);
    suite_add_tcase(s, tc);

    return s;
}
#include <CUnit/CUnit.h>

void tlv_create_int_test()
{
    int expected = 10;
    t_tlv *tlv = tlv_create_int(expected);
    int actual = tlv_get_int(tlv);
    CU_ASSERT_EQUAL(expected, actual);
}

void tlv_test()
{
    CU_ASSERT(2 * 3 == 6);
}

void add_tlv_suite() {
    CU_pSuite suite = CU_add_suite(__FILE__, NULL, NULL);
    CU_add_test(suite, "tlv_create_int_test", tlv_create_int_test);
    CU_add_test(suite, "tlv_test", tlv_test);
}
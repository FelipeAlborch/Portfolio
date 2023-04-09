#include <check.h>
#include <tlv.h>

START_TEST (test_tlv_create_int)
{
    t_tlv *result = tlv_create_int(10);
    ck_assert_int_eq(tlv_get_int(result), 10);
}
END_TEST
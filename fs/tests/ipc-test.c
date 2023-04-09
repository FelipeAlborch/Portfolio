#include <check.h>
#include <ipc.h>

START_TEST (test_payload_create_string)
{
    t_payload *result = payload_create_string("hola");
    ck_assert_str_eq(payload_get_string(result), "hola");
}
END_TEST
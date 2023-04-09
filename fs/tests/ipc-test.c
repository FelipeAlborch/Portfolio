#include <check.h>
#include <ipc.h>

START_TEST (test_payload_create_string)
{
    char *expected = "hola";
    t_payload *payload = payload_create_string(expected);
    char *actual = payload_get_string(payload);
    ck_assert_str_eq(expected, actual);
}
END_TEST
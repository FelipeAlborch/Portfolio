#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "cunit-tlv.c"
#include "test.c"

int main() {
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_initialize_registry();

    // Add the test suites to the registry

    add_tlv_suite();
    add_test_suite();

    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
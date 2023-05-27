#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "cunit-ipc.c"
#include "cunit-tlv.c"

int main() {
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_initialize_registry();

    // Add the test suites to the registry
    add_ipc_suite();
    add_tlv_suite();

    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
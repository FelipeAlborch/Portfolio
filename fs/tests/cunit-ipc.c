#include <CUnit/CUnit.h>
#include <tlv.h>

void read_socket_create_test()
{
    CU_ASSERT(2 * 3 == 6);
}

void ipc_test()
{
    CU_ASSERT(2 * 3 == 6);
}

int init() { return 0; }

int clean() { return 0; }

void add_ipc_suite() {
    CU_pSuite suite = CU_add_suite(__FILE__, init, clean);
    CU_add_test(suite, "read_socket_create_test", read_socket_create_test);
    CU_add_test(suite, "ipc_test", ipc_test);
}
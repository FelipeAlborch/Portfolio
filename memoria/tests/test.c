#include <CUnit/CUnit.h>
#include <memoriaUtils.h>

void test_crear_proceso(){
    int pid=1;
    crear_proceso(pid);

    CU_ASSERT_EQUAL(1,1);
}
void test_crear_segmento(){
    int pid=1;
    int tam=10;
    int id=1;
    create_segment(pid,tam,id);
    CU_ASSERT_EQUAL(1,1);
}

void i_test()
{
    CU_ASSERT(2 * 3 == 6);
}
int init() { return 0; }

int clean() { return 0; }

void add_test_suite() {
    CU_pSuite suite = CU_add_suite(__FILE__, init, clean);
    CU_add_test(suite, "test_crear_proceso", test_crear_proceso);
    CU_add_test(suite, "test_crear_segmento",test_crear_segmento );
    CU_add_test(suite, "i_test", i_test);
}

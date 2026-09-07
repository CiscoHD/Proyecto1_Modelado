extern "C" {
#include <unity.h>
}

void setUp(void) {}
void tearDown(void) {}

void testPrueba(void) {
    TEST_ASSERT_TRUE(true);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testPrueba);
    return UNITY_END();
}

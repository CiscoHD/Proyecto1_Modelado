#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

void testPrueba(void) {
  TEST_ASSERT_EQUAL_INT(42, 42);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(testPrueba);
  return UNITY_END();
}

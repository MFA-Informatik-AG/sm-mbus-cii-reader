#include "unity.h"
#include "test_hdlc.h"
#include "test_decrypt.h"
#include "test_memory.h"

// runt tests in a PlatformIO Terminal window using 
// pio test -e testnative -v


void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

int runUnityTests(void) {
  
  UNITY_BEGIN();
  
  // increase loops to test for memory/reentry issues
  size_t minloops = 1;

  for (size_t i = 0; i < minloops; i++)
  {
    RUN_TEST(test_full_hdlc);
    RUN_TEST(test_gbt_array2);
    RUN_TEST(test_decript_hdlc);
  }

  // RUN_TEST(test_memory_leaks);

  return UNITY_END();
}

int main(void) {
  return runUnityTests();
}
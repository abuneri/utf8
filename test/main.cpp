
#include <gtest/gtest.h>
#include <climits>

int main(int argc, char **argv) {
  static_assert(CHAR_BIT == 8, "Must be on a platform with 8-bit chars");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

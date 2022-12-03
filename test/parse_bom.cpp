#include <gtest/gtest.h>

#include <auc/u8text.hpp>
#include <fstream>
#include <string>

TEST(parse_bom, valid_utf8) {
  std::ifstream bom_utf8_file("../test/bom.txt");
  ASSERT_TRUE(bom_utf8_file.is_open());
  std::string bytes;
  std::getline(bom_utf8_file, bytes);

  auc::u8text bom_utf8(bytes);
  char utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
  EXPECT_STREQ(utf8_chars, bom_utf8.data().data());
}

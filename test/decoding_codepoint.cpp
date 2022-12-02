#include <gtest/gtest.h>

#include <auc/u8text.hpp>

TEST(decoding_codepoint, valid_utf8) {
  char utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::u8char>& chars = utf8_text.get_chars();
  ASSERT_EQ(chars.size(), 9);

  EXPECT_EQ(chars[0].get_codepoint().get_num(), 0x07FF);
  EXPECT_EQ(chars[1].get_codepoint().get_num(), 0x1000);
  EXPECT_EQ(chars[2].get_codepoint().get_num(), 0x0024);
  EXPECT_EQ(chars[3].get_codepoint().get_num(), 0x00A3);
  EXPECT_EQ(chars[4].get_codepoint().get_num(), 0x0939);
  EXPECT_EQ(chars[5].get_codepoint().get_num(), 0x20AC);
  EXPECT_EQ(chars[6].get_codepoint().get_num(), 0xD55C);
  EXPECT_EQ(chars[7].get_codepoint().get_num(), 0x10348);
  EXPECT_EQ(chars[8].get_codepoint().get_num(), 0x9FFFF);
}

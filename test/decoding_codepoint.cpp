#include <gtest/gtest.h>

#include <auc/u8text.hpp>

TEST(decoding_codepoint, valid_utf8) {
  char8_t utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::codepoint> codepoints = utf8_text.get_codepoints();
  ASSERT_EQ(codepoints.size(), 9);

  EXPECT_EQ(codepoints[0].get_num(), 0x07FF);
  EXPECT_EQ(codepoints[1].get_num(), 0x1000);
  EXPECT_EQ(codepoints[2].get_num(), 0x0024);
  EXPECT_EQ(codepoints[3].get_num(), 0x00A3);
  EXPECT_EQ(codepoints[4].get_num(), 0x0939);
  EXPECT_EQ(codepoints[5].get_num(), 0x20AC);
  EXPECT_EQ(codepoints[6].get_num(), 0xD55C);
  EXPECT_EQ(codepoints[7].get_num(), 0x10348);
  EXPECT_EQ(codepoints[8].get_num(), 0x9FFFF);
}

#include <gtest/gtest.h>

#include <auc/u8text.hpp>

TEST(encoding_codepoint, valid_utf8) {
  const auto utf8_text =
      auc::u8text::from_codepoints({0x07FF, 0x1000, 0x0024, 0x00A3, 0x0939,
                                    0x20AC, 0xD55C, 0x10348, 0x9FFFF});

  std::string raw_data = utf8_text.data();
  char utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
  EXPECT_STREQ(utf8_chars, raw_data.data());
}

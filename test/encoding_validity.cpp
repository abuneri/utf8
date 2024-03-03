#include <gtest/gtest.h>

#include <auc/u8text.hpp>

TEST(encoding_validity, valid_utf8) {
  char8_t utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
  auc::u8text utf8_text(utf8_chars);
  EXPECT_TRUE(utf8_text.is_valid());
}

TEST(encoding_validity, invalid_utf8) {
  char invalid_chars[] =
      "\u07FF\u1000\0024\u00A3\u0939\u20AC\uD55C\u10348\u09FFFF";
  auc::u8text invalid_text(invalid_chars);
#ifdef _WIN32
  EXPECT_FALSE(invalid_text.is_valid());
#else
  EXPECT_TRUE(invalid_text.is_valid());
#endif
}

TEST(encoding_validity, valid_7bit_ascii) {
  char ascii_chars[] = "Hello, World!";
  auc::u8text ascii_text(ascii_chars);
  EXPECT_TRUE(ascii_text.is_valid());
}

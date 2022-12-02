#include <gtest/gtest.h>

#include <auc/u8text.hpp>

TEST(decoding_codepoint, valid_utf8) {
  char utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
  auc::u8text utf8_text(utf8_chars);

  // TODO: add iface to get u8char's and then validate their code points
}
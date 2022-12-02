#pragma once

#include <vector>

namespace auc {
namespace detail {

enum u8flags : unsigned char {
  SeqChar = 0x80,         // '10' high bits
  TwoOctetChar = 0xC0,    // '110' high bits
  ThreeOctetChar = 0xE0,  // '1110' high bits
  FourOctetChar = 0xF0    // '11110' high bits
};

bool is_extended_ascii(const char c);

bool is_sequence(const char c);

std::size_t num_octets(const char c);

}  // namespace detail

class u8char {
 public:
  explicit u8char(const char byte);

  explicit u8char(const char* bytes, const std::size_t length);

  bool is_valid() const;

  // TODO: generate an object auc::code_point that represents the code point of
  // the utf8 char

 private:
  bool valid_encoding_{false};
  // TODO: this is the encoded storage, rename/clarify that
  std::vector<char> storage_;

  // TODO: store the decoded character data (i.e. a unicode code point)
};

}  // namespace auc
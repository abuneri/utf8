#pragma once

#include <string>
#include <vector>

#include "codepoint.hpp"

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

std::uint32_t to_codepoint(const std::vector<char>& encoded_bytes);

std::vector<char> to_encoded_bytes(std::uint32_t codepoint);

}  // namespace detail

// https://www.rfc-editor.org/rfc/rfc3629#section-3
//   Char. number range  |        UTF-8 octet sequence
//      (hexadecimal)    |              (binary)
//   --------------------+---------------------------------------------
//   0000 0000-0000 007F | 0xxxxxxx
//   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
//   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
//   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
class u8char {
 public:
  explicit u8char(const char byte);

  explicit u8char(const char* bytes, const std::size_t length);

  // TODO: add std::string ctor

  static u8char from_codepoint(codepoint cp);

  bool is_valid() const;

  codepoint get_codepoint() const { return codepoint_; }

  std::size_t get_num_bytes() const { return encoded_storage_.size(); }

  std::string data() const {
    return std::string(encoded_storage_.data(), encoded_storage_.size());
  }

 private:
  explicit u8char(const std::vector<char>& encoded_bytes,
                  const bool valid_encoding);

  bool valid_encoding_{false};
  std::vector<char> encoded_storage_;
  codepoint codepoint_{0};
};

}  // namespace auc

#pragma once

#include "codepoint.hpp"

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

std::uint32_t to_codepoint(const std::vector<char>& encoded_bytes);

}  // namespace detail

class u8char {
 public:
  explicit u8char(const char byte);

  explicit u8char(const char* bytes, const std::size_t length);

  // TODO: add std::string ctor

  bool is_valid() const;

  codepoint get_codepoint() const { return codepoint_; }

 private:
  bool valid_encoding_{false};
  std::vector<char> encoded_storage_;
  codepoint codepoint_{0};
};

}  // namespace auc

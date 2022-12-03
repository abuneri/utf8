#pragma once

#include "u8char.hpp"

#include <string>
#include <string_view>

namespace auc {

class u8text {
 public:
  explicit u8text(std::string_view bytes);

  static u8text from_codepoints(const std::vector<codepoint>& cps);

  bool is_valid() const;

  const std::vector<u8char>& get_chars() const { return chars_; }

  std::string data() const;

  // TODO: All implementation details for the util/helper methods
  //  (e.g. length, find/contains,sort) on the class will
  //  be implemented via the list of u8char's

  // TODO: Depend on libfmt and add support for printing and formatting
  // auc::u8text.
  //  do this via __has_include to optionally define the libfmt formatter
  //  and printer if the user has the libfmt dependency

 private:
  explicit u8text(const std::vector<u8char> chars);

  // TODO: handle Byte-Order-Mark (BOM)
  // TODO: handle Grapheme Clusters, need this before we can implement utf8 text
  // searching/sorting
  void parse_chars(const char* bytes, const std::size_t length);

  std::vector<u8char> chars_;
};

}  // namespace auc

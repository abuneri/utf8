#pragma once

#include <cstdint>

namespace auc {

class codepoint {
 public:
  codepoint(std::uint32_t num) : num_(num) {}

  std::uint32_t get_num() const { return num_; }

  // TODO: use https://www.unicode.org/Public/15.0.0/ucd/emoji/emoji-data.txt to
  // buyild a lookup that can determine if a given emoji is
  // Extended_Pictographic (ExtPict) by generating a file called
  // "emojidata_lookup.hpp" from the python scripts
  // lookup. Then this can be used to complete the grapheme break implementation

  // TODO: Add helper to convert to a hex string representation (maybe also with
  // U+ prefix?)
 private:
  std::uint32_t num_ = 0;
};

}  // namespace auc

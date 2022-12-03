#pragma once

#include <cstdint>

namespace auc {

class codepoint {
 public:
  codepoint(std::uint32_t num) : num_(num) {}

  std::uint32_t get_num() const { return num_; }

  // TODO: Add helper to convert to a hex string representation (maybe also with
  // U+ prefix?)
 private:
  std::uint32_t num_ = 0;
};

}  // namespace auc

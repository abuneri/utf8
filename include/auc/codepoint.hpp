#pragma once

#include <cstdint>

namespace auc {

// TODO: This class can be used for things like unicode equality or
// starts_with/ends_with/contains/sorting u8text methods
class codepoint {
 public:
  codepoint(std::uint32_t num) : num_(num) {}

  // TODO: Add helper to convert to a hex string representation
 private:
  std::uint32_t num_ = 0;
};

}  // namespace auc

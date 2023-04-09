#pragma once

#include "property.hpp"

#include <cstdint>
#include <functional>
#include <optional>

namespace auc {

class codepoint {
 public:
  codepoint(std::uint32_t num);

  std::uint32_t get_num() const { return num_; }

  const property& get_property() const;

  // TODO: Add helper to convert to a hex string representation (maybe also with
  // U+ prefix?)
 private:
  std::uint32_t num_ = 0;
  mutable std::optional<property> prop_;
};

inline bool operator==(const codepoint& c1, const codepoint& c2) {
  return (c1.get_num() == c2.get_num());
}

}  // namespace auc


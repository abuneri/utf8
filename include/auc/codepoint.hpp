#pragma once

#include "property.hpp"

#include <cstdint>
#include <functional>

namespace auc {

class codepoint {
 public:
  codepoint(std::uint32_t num) : num_(num) {}

  std::uint32_t get_num() const { return num_; }

  property get_property() const;

  // TODO: Add helper to convert to a hex string representation (maybe also with
  // U+ prefix?)
 private:
  std::uint32_t num_ = 0;
};

inline bool operator==(const codepoint& c1, const codepoint& c2) {
  return (c1.get_num() == c2.get_num());
}

}  // namespace auc

template <>
struct std::hash<auc::codepoint> {
  std::size_t operator()(const auc::codepoint& cp) const noexcept {
    return std::hash<std::uint32_t>{}(cp.get_num());
  }
};

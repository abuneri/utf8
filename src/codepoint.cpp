#include <auc/codepoint.hpp>

namespace auc {

codepoint::codepoint(std::uint32_t num) : num_(num) {}

const property& codepoint::get_property() const {
  if (!prop_.has_value()) {
    prop_ = property{num_};
  }
  return *prop_;
}

}  // namespace auc

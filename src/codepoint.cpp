#include <auc/codepoint.hpp>

#include "emojiproperty_lookup.hpp"
#include "graphemebreakproperty_lookup.hpp"

namespace auc {

namespace detail {}  // namespace detail

property codepoint::get_property() const {
  if (auto prop_itr = detail::codepoint_break_lookup.find(*this);
      prop_itr != detail::codepoint_break_lookup.end()) {
    return prop_itr->second;
  } else if (auto emoji_prop_itr = detail::codepoint_emoji_lookup.find(*this);
             emoji_prop_itr != detail::codepoint_emoji_lookup.end()) {
    return emoji_prop_itr->second;
  }

  return property::Other;
}

}  // namespace auc

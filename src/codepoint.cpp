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
    // TODO: think of better way to represent properties so emoji codepoints
    // which contain multiple properties are better represented. Maybe we should
    // have property classes (GraphemeBreak and Emoji) where
    // codepoint::get_property returns a std::variant of the 2 classes, instead
    // of a single property enum
    return emoji_prop_itr->second.back();
  }

  return property::Other;
}

}  // namespace auc

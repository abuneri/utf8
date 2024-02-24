#include "emojiproperty_lookup.hpp"
#include "graphemebreakproperty_lookup.hpp"

#include <auc/property.hpp>

#include <algorithm>

namespace auc {

namespace detail {

// https://en.cppreference.com/w/cpp/utility/variant/visit
// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;



}  // namespace detail

property::property_t property::init_prop(std::uint32_t cp) {
  if (auto prop_itr = detail::codepoint_break_lookup.find(cp);
      prop_itr != detail::codepoint_break_lookup.end()) {
    return graphemebreak{prop_itr->second};
  } else if (auto emoji_prop_itr = detail::codepoint_emoji_lookup.find(cp);
             emoji_prop_itr != detail::codepoint_emoji_lookup.end()) {
    return emoji{emoji_prop_itr->second};
  } else {
    return graphemebreak{property::type::Other};
  }
}

property::property(std::uint32_t cp) : prop_(init_prop(cp)) {}

bool property::has_property(property::type prop_type) const {
  bool has_prop = false;
  std::visit(
      detail::overloaded{
          [&](const graphemebreak& gb) { has_prop = (gb.prop_ == prop_type); },
          [&](const emoji& e) {
            has_prop = (std::find(e.props_.begin(), e.props_.end(),
                                  prop_type) != e.props_.end());
          }},
      prop_);
  return has_prop;
}

bool property::has_property(
    const std::vector<property::type>& prop_types) const {
  bool has_prop = false;
  std::visit(detail::overloaded{
                 [&](const graphemebreak& gb) {
                   has_prop = (std::find(prop_types.begin(), prop_types.end(),
                                         gb.prop_) != prop_types.end());
                   ;
                 },
                 [&](const emoji& e) {
                   has_prop = (std::find_if(
                                   prop_types.begin(), prop_types.end(),
                                   [&](property::type prop) {
                                     return (std::find(e.props_.begin(),
                                                       e.props_.end(),
                                                       prop) != e.props_.end());
                                   }) != prop_types.end());
                 }},
             prop_);
  return has_prop;
}

}  // namespace auc

#pragma once

#include <cstdint>
#include <unordered_map>
#include <variant>
#include <vector>

namespace auc {

class property {
 public:
  enum class type : int {
    Other = 0,
    CR = 1,
    LF = 2,
    Control = 3,
    Extend = 4,
    RI = 5,
    Prepend = 6,
    SpacingMark = 7,
    L = 8,
    V = 9,
    T = 10,
    LV = 11,
    LVT = 12,
    ZWJ = 13,
    Emoji = 14,
    Emoji_Pres = 15,
    Emoji_Mod = 16,
    Emoji_Mod_Base = 17,
    Emoji_Comp = 18,
    Ext_Pict = 19
  };

  property(std::uint32_t cp);

  bool has_property(property::type prop_type) const;
  bool has_property(const std::vector<property::type>& prop_types) const;

 private:
  struct graphemebreak {
    type prop_ = type::Other;
  };
  struct emoji {
    std::vector<type> props_;
  };
  using property_t = std::variant<graphemebreak, emoji>;

  property_t init_prop(std::uint32_t cp);

  property_t prop_;
};

}  // namespace auc

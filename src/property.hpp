#pragma once

namespace auc {
namespace detail {

enum class property : int {
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
  Emoji_Mod_Base = 16,
  Emoji_Comp = 17,
  Ext_Pict = 18
};

}  // namespace detail
}  // namespace auc

#include <auc/graphemecluster.hpp>
#include "graphemebreakproperty_lookup.hpp"

#include <tuple>

namespace auc {

namespace detail {

std::vector<grapheme_cluster_break> get_char_breaks(
    const std::vector<u8char>& chars) {
  std::vector<grapheme_cluster_break> breaks;

  for (const auto& c : chars) {
    const std::uint32_t cp = c.get_codepoint().get_num();

    if (codepoint_break_lookup.find(cp) == codepoint_break_lookup.end()) {
      breaks.push_back(
          grapheme_cluster_break{c.get_codepoint(), property::Other});
    } else {
      breaks.push_back(codepoint_break_lookup[cp]);
    }
  }

  return breaks;
}

// The following is what can be used to implement the breaking rules instead of
// the table below.
// https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
//
// See
// https://github.com/foliojs/grapheme-breaker/blob/master/src/GraphemeBreaker.js
// as an older example of how it can be translated to code


// https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakTest.html
bool has_break(const grapheme_cluster_break& b1,
               const grapheme_cluster_break& b2) {
      static bool break_chart[14][14] = {
              /*  Other  | CR |  LF  | Control | Extend | RI | Prepend | SpacingMark |   L   |   V   |   T   |  LV  |  LVT  | ZWJ  */
      /* Other */ true,   true, true,   true,    false,  true,  true,      false,      true,   true,   true,   true,  true,  false,
         /* CF */ true,   true, false,  true,    true,   true,  true,      true,       true,   true,   true,   true,  true,  true,
         /* LF */ true,   true, true,   true,    true,   true,  true,      true,       true,   true,   true,   true,  true,  true,
    /* Control */ true,   true, true,   true,    true,   true,  true,      true,       true,   true,   true,   true,  true,  true,
     /* Extend */ true,   true, true,   true,    false,  true,  true,      false,      true,   true,   true,   true,  true,  false,
         /* RI */ true,   true, true,   true,    false,  false, true,      false,      true,   true,   true,   true,  true,  false,
    /* Prepend */ false,  true, true,   true,    false,  false, false,     false,      false,  false,  false,  false, false, false,
/* SpacingMark */ true,   true, true,   true,    false,  true,  true,      false,      true,   true,   true,   true,  true,  false,
          /* L */ true,   true, true,   true,    false,  true,  true,      false,      false,  false,  true,   false, false, false,
          /* V */ true,   true, true,   true,    false,  true,  true,      false,      true,   false,  false,  true,  true,  false,
          /* T */ true,   true, true,   true,    false,  true,  true,      false,      true,   true,   false,  true,  true,  false,
         /* LV */ true,   true, true,   true,    false,  true,  true,      false,      true,   false,  false,  true,  true,  false,
        /* LVT */ true,   true, true,   true,    false,  true,  true,      false,      true,   true,   false,  true,  true,  false,
        /* ZWJ */ true,   true, true,   true,    false,  true,  true,      false,      true,   true,   true,   true,  true,  false
  };

  return break_chart[static_cast<int>(b1.prop_)][static_cast<int>(b2.prop_)];
}

// Specialized RI break rules (can only have 2 in a cluster bfore
// we must break)
void handle_regionalindicator_breaks(std::vector<graphemecluster>& clusters) {
  // TODO
}

std::vector<graphemecluster> build_grapheme_clusters(
    const std::vector<u8char>& chars) {

  std::vector<graphemecluster> grapheme_clusters;

  const std::vector<grapheme_cluster_break> breaks = get_char_breaks(chars);

  // Trivial cases
  if (breaks.empty())
    return {};
  else if (breaks.size() == 1u) {
    grapheme_clusters.push_back(graphemecluster{chars});
    return grapheme_clusters;
  } else if (breaks.size() == 2u) {
    if (has_break(breaks[0], breaks[1])) {
      grapheme_clusters.push_back(graphemecluster{{chars[0]}});
      grapheme_clusters.push_back(graphemecluster{{chars[1]}});
    } else {
      grapheme_clusters.push_back(graphemecluster{chars});
    }
    return grapheme_clusters;
  }

  std::vector<u8char> cluster;
  for (std::size_t current_break_idx = 1u; current_break_idx < breaks.size();
       ++current_break_idx) {
    const std::size_t previous_break_idx = current_break_idx - 1u;
    const grapheme_cluster_break& previous_break = breaks[previous_break_idx];
    const grapheme_cluster_break& current_break = breaks[current_break_idx];

    cluster.push_back(chars[previous_break_idx]);
    if (has_break(previous_break, current_break)) {
      grapheme_clusters.push_back(graphemecluster{cluster});
      cluster.clear();
    }
    if (current_break_idx + 1u == breaks.size()) {
      grapheme_clusters.push_back(graphemecluster{{chars.back()}});
    }
  }

  // TODO: remove this and handle RI's within the above break checking for
  // simplicity (may need to do double break lookback instead of single lookback
  // to satisfy the case where more than 2 consecutive RI's require a break
  handle_regionalindicator_breaks(grapheme_clusters);
  return grapheme_clusters;
}

}  // namespace detail

}  // namespace auc
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

// TODO: make all 24 samples pass:
// https://www.unicode.org/reports/tr44/#Extended_Pictographic
// https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
// https://www.unicode.org/Public/15.0.0/ucd/emoji/emoji-data.txt

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

  // TODO: Do a cleaner algorithm for building up grapheme clusters

  std::size_t grapheme_start = 0u;
  std::size_t grapheme_end = 1u;

  const std::size_t num_breaks = breaks.size();
  while (grapheme_end < num_breaks) {
    std::size_t temp_grapheme_start = grapheme_start;
    std::size_t temp_grapheme_end = grapheme_end;

    bool clustering = true;
    while (clustering && temp_grapheme_end < num_breaks) {
      const grapheme_cluster_break b1 = breaks[temp_grapheme_start];
      const grapheme_cluster_break b2 = breaks[temp_grapheme_end];

      clustering = !has_break(b1, b2);
      ++temp_grapheme_start;
      ++temp_grapheme_end;
    }

    grapheme_end = temp_grapheme_start;

    const std::size_t num_cluster_chars = grapheme_end - grapheme_start;
    graphemecluster cluster;
    cluster.chars_.reserve(num_cluster_chars);

    for (std::size_t char_idx = grapheme_start; char_idx < grapheme_end;
         ++char_idx) {
      cluster.chars_.push_back(chars[char_idx]);
    }

    grapheme_clusters.push_back(cluster);

    grapheme_start = grapheme_end;

    // Last character is a a single cluster, add that as incrementing again will
    // breka the loop
    if (num_cluster_chars > 0u && grapheme_start == breaks.size() - 1) {
      grapheme_clusters.push_back(graphemecluster{{chars[grapheme_end]}});
    }
    ++grapheme_end;
  }

  handle_regionalindicator_breaks(grapheme_clusters);
  return grapheme_clusters;
}

}  // namespace detail

}  // namespace auc
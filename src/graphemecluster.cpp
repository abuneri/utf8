#include "emojiproperty_lookup.hpp"
#include "graphemebreakproperty_lookup.hpp"

#include <auc/graphemecluster.hpp>

#include <tuple>
#include <algorithm>

namespace auc {

namespace detail {

std::vector<grapheme_cluster_break> get_char_breaks(
    const std::vector<u8char>& chars) {
  std::vector<grapheme_cluster_break> breaks;

  for (const auto& c : chars) {
    const std::uint32_t cp = c.get_codepoint().get_num();

    if (auto prop_itr = codepoint_break_lookup.find(cp);
        prop_itr != codepoint_break_lookup.end()) {
      breaks.push_back(prop_itr->second);
    } else if (auto emoji_prop_itr = codepoint_emoji_lookup.find(cp);
               emoji_prop_itr != codepoint_emoji_lookup.end()) {
      breaks.push_back({emoji_prop_itr->first, emoji_prop_itr->second});
    } else {
      breaks.push_back({c.get_codepoint(), property::Other});
    }
  }

  return breaks;
}

bool contains_prop(const std::vector<property>& props, property prop) {
  return (std::find(props.begin(), props.end(), prop) != props.end());
};

int num_current_regind_props(const std::vector<u8char>& current_cluster) {
  auto is_regind_prop = [](const codepoint& cp) {
    auto itr = codepoint_break_lookup.find(cp.get_num());
    if (itr != codepoint_break_lookup.end()) {
      return (itr->second.prop_ == property::RI);
    }
    return false;
  };
  return std::count_if(current_cluster.begin(), current_cluster.end(),
                       [&is_regind_prop](const u8char& c) {
                         return is_regind_prop(c.get_codepoint());
                       });
};

// https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
bool has_break(const std::vector<u8char>& current_cluster,
               const grapheme_cluster_break& previous,
               const grapheme_cluster_break& current) {
  // Break at the start and end of text, unless the text is empty.
  // GB1/GB2 are handled implicitly

  // Do not break between a CR and LF. Otherwise, break before and after
  // controls.
  if (previous.prop_ == property::CR && current.prop_ == property::LF) {
    // GB3
    return false;
  } else if (contains_prop({property::Control, property::CR, property::LF},
                           previous.prop_)) {
    // GB4
    return true;
  } else if (contains_prop({property::Control, property::CR, property::LF},
                           current.prop_)) {
    // GB5
    return true;
  }
  // Do not break Hangul syllable sequences.
  else if (previous.prop_ == property::L &&
           contains_prop(
               {property::L, property::V, property::LV, property::LVT},
               current.prop_)) {
    // GB6
    return false;
  } else if (contains_prop({property::LV, property::V}, previous.prop_) &&
             contains_prop({property::V, property::T}, current.prop_)) {
    // GB7
    return false;
  } else if (contains_prop({property::LVT, property::T}, previous.prop_) &&
             current.prop_ == property::T) {
    // GB8
    return false;
  }
  // Do not break before extending characters or ZWJ.
  else if (contains_prop({property::Extend, property::ZWJ}, current.prop_)) {
    // GB9
    return false;
  }
  // NOTE: Currently we always assume we support extended grapheme clusters over
  // legacy grapheme clusters

  // Do not break before SpacingMarks, or after Prepend characters.
  else if (current.prop_ == property::SpacingMark) {
    // GB9a
    return false;
  } else if (previous.prop_ == property::Prepend) {
    // GB9b
    return false;
  }
  // TODO: Do not break within emoji modifier sequences or emoji zwj sequences.
  // GB11

  // Do not break within emoji flag sequences. That is, do not break between
  // regional indicator (RI) symbols if there is an odd number of RI characters
  // before the break point.
  else if (previous.prop_ == property::RI && current.prop_ == property::RI) {
    // GB12 and GB13
    return !(num_current_regind_props(current_cluster) <= 1);
  }
  // Otherwise, break everywhere.
  else {
    // GB999
    return true;
  }
}

std::vector<graphemecluster> build_grapheme_clusters(
    const std::vector<u8char>& chars) {
  std::vector<u8char> cluster;
  std::vector<graphemecluster> grapheme_clusters;

  const std::vector<grapheme_cluster_break> breaks = get_char_breaks(chars);

  // Trivial cases
  if (breaks.empty())
    return {};
  else if (breaks.size() == 1u) {
    grapheme_clusters.push_back(graphemecluster{chars});
    return grapheme_clusters;
  } else if (breaks.size() == 2u) {
    if (has_break(cluster, breaks[0], breaks[1])) {
      grapheme_clusters.push_back(graphemecluster{{chars[0]}});
      grapheme_clusters.push_back(graphemecluster{{chars[1]}});
    } else {
      grapheme_clusters.push_back(graphemecluster{chars});
    }
    return grapheme_clusters;
  }

  for (std::size_t current_break_idx = 1u; current_break_idx < breaks.size();
       ++current_break_idx) {
    const std::size_t previous_break_idx = current_break_idx - 1u;
    const grapheme_cluster_break& previous_break = breaks[previous_break_idx];
    const grapheme_cluster_break& current_break = breaks[current_break_idx];

    cluster.push_back(chars[previous_break_idx]);
    if (has_break(cluster, previous_break, current_break)) {
      grapheme_clusters.push_back(graphemecluster{cluster});
      cluster.clear();
    }
    if (current_break_idx + 1u == breaks.size()) {
      grapheme_clusters.push_back(graphemecluster{{chars.back()}});
    }
  }

  return grapheme_clusters;
}

}  // namespace detail

}  // namespace auc
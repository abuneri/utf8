#include <auc/graphemecluster.hpp>

#include <tuple>
#include <algorithm>

namespace auc {

namespace detail {

bool contains_prop(const std::vector<property>& props, property prop) {
  return (std::find(props.begin(), props.end(), prop) != props.end());
};

int num_current_regind_props(const std::vector<u8char>& current_cluster) {
  return std::count_if(
      current_cluster.begin(), current_cluster.end(), [](const u8char& c) {
        return (c.get_codepoint().get_property() == property::RI);
      });
};

// https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
bool has_break(const std::vector<u8char>& current_cluster,
               const codepoint& previous, const codepoint& current) {
  const property previous_prop = previous.get_property();
  const property current_prop = current.get_property();

  // Break at the start and end of text, unless the text is empty.
  // GB1/GB2 are handled implicitly

  // Do not break between a CR and LF. Otherwise, break before and after
  // controls.
  if (previous_prop == property::CR && current_prop == property::LF) {
    // GB3
    return false;
  } else if (contains_prop({property::Control, property::CR, property::LF},
                           previous_prop)) {
    // GB4
    return true;
  } else if (contains_prop({property::Control, property::CR, property::LF},
                           current_prop)) {
    // GB5
    return true;
  }
  // Do not break Hangul syllable sequences.
  else if (previous_prop == property::L &&
           contains_prop(
               {property::L, property::V, property::LV, property::LVT},
               current_prop)) {
    // GB6
    return false;
  } else if (contains_prop({property::LV, property::V}, previous_prop) &&
             contains_prop({property::V, property::T}, current_prop)) {
    // GB7
    return false;
  } else if (contains_prop({property::LVT, property::T}, previous_prop) &&
             current_prop == property::T) {
    // GB8
    return false;
  }
  // Do not break before extending characters or ZWJ.
  else if (contains_prop({property::Extend, property::ZWJ}, current_prop)) {
    // GB9
    return false;
  }
  // NOTE: Currently we always assume we support extended grapheme clusters over
  // legacy grapheme clusters

  // Do not break before SpacingMarks, or after Prepend characters.
  else if (current_prop == property::SpacingMark) {
    // GB9a
    return false;
  } else if (previous_prop == property::Prepend) {
    // GB9b
    return false;
  }
  // TODO: Do not break within emoji modifier sequences or emoji zwj sequences.
  // GB11

  // Do not break within emoji flag sequences. That is, do not break between
  // regional indicator (RI) symbols if there is an odd number of RI characters
  // before the break point.
  else if (previous_prop == property::RI && current_prop == property::RI) {
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

  // Trivial cases
  if (chars.empty())
    return {};
  else if (chars.size() == 1u) {
    grapheme_clusters.push_back(graphemecluster{chars});
    return grapheme_clusters;
  } else if (chars.size() == 2u) {
    if (has_break(cluster, chars[0].get_codepoint(),
                  chars[1].get_codepoint())) {
      grapheme_clusters.push_back(graphemecluster{{chars[0]}});
      grapheme_clusters.push_back(graphemecluster{{chars[1]}});
    } else {
      grapheme_clusters.push_back(graphemecluster{chars});
    }
    return grapheme_clusters;
  }

  // Non-trivial cases
  for (std::size_t current_codepoint_idx = 1u;
       current_codepoint_idx < chars.size(); ++current_codepoint_idx) {
    const std::size_t previous_codepoint_idx = current_codepoint_idx - 1u;
    const codepoint previous_codepoint =
        chars[previous_codepoint_idx].get_codepoint();
    const codepoint current_codepoint =
        chars[current_codepoint_idx].get_codepoint();

    cluster.push_back(chars[previous_codepoint_idx]);
    if (has_break(cluster, previous_codepoint, current_codepoint)) {
      grapheme_clusters.push_back(graphemecluster{cluster});
      cluster.clear();
    }
    if (current_codepoint_idx + 1u == chars.size()) {
      cluster.push_back(chars.back());
      grapheme_clusters.push_back(graphemecluster{cluster});
    }
  }

  return grapheme_clusters;
}

}  // namespace detail

}  // namespace auc
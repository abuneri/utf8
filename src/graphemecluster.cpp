#include <auc/graphemecluster.hpp>
#include "graphemebreakproperty_lookup.hpp"

#include <tuple>
#include <algorithm>

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


bool has_break(const grapheme_cluster_break& previous,
               const grapheme_cluster_break& current) {
  auto contains_prop = [](const std::vector<auc::detail::property>& props,
                          auc::detail::property prop) {
    return (std::find(props.begin(), props.end(), prop) != props.end());
  };

  // https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
  // Break at the start and end of text, unless the text is empty.
  // GB1/GB2 are handled implicitly

  // Do not break between a CR and LF. Otherwise, break before and after
  // controls.
  if (previous.prop_ == auc::detail::property::CR &&
      current.prop_ == auc::detail::property::LF) {
    // GB3
    return false;
  } else if (contains_prop(
                 {auc::detail::property::Control, auc::detail::property::CR,
                  auc::detail::property::LF},
                 previous.prop_)) {
    // GB4
    return true;
  } else if (contains_prop(
                 {auc::detail::property::Control, auc::detail::property::CR,
                  auc::detail::property::LF},
                 current.prop_)) {
    // GB5
    return true;
  }
  // Do not break Hangul syllable sequences.
  else if (previous.prop_ == auc::detail::property::L &&
           contains_prop(
               {auc::detail::property::L, auc::detail::property::V,
                auc::detail::property::LV, auc::detail::property::LVT},
               current.prop_)) {
    // GB6
    return false;
  } else if (contains_prop(
                 {auc::detail::property::LV, auc::detail::property::V},
                 previous.prop_) &&
             contains_prop({auc::detail::property::V, auc::detail::property::T},
                           current.prop_)) {
    // GB7
    return false;
  } else if (contains_prop(
                 {auc::detail::property::LVT, auc::detail::property::T},
                 previous.prop_) &&
             current.prop_ == auc::detail::property::T) {
    // GB8
    return false;
  }
  // Do not break before extending characters or ZWJ.
  else if (contains_prop(
               {auc::detail::property::Extend, auc::detail::property::ZWJ},
               current.prop_)) {
    // GB9
    return false;
  }
  // NOTE: Currently we always assume we support extended grapheme clusters over
  // legacy grapheme clusters Do not break before SpacingMarks, or after Prepend
  // characters.
  else if (current.prop_ == auc::detail::property::SpacingMark) {
    // GB9a
    return false;
  } else if (previous.prop_ == auc::detail::property::Prepend) {
    // GB9b
    return false;
  }
  // TODO: Support these rules once we parse emoji data to include ExtPict break
  // property in our lookup table Do not break within emoji modifier sequences
  // or emoji zwj sequences. GB11

  // TODO: Need to specially handle RI's to ensure we break after seeing 2
  // instances of RI in a row Do not break within emoji flag sequences. That is,
  // do not break between regional indicator (RI) symbols if there is an odd
  // number of RI characters before the break point. GB12 GB13

  // Otherwise, break everywhere.
  else {
    // GB999
    return true;
  }
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

    // TODO: count RI's in the cluster. we need to break to ensure we don't end
    // up with an odd number > 2 in a cluster

    if (has_break(previous_break, current_break)) {
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
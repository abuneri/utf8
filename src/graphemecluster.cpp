#include <auc/graphemecluster.hpp>

#include <tuple>
#include <algorithm>

namespace auc {

namespace detail {

const property& get_prop(const u8char& c) {
  return c.get_codepoint().get_property();
}

int num_current_regind_props(const std::vector<u8char>& current_cluster) {
  return std::count_if(current_cluster.begin(), current_cluster.end(),
                       [](const u8char& c) {
                         return (get_prop(c).has_property(property::type::RI));
                       });
};

bool is_emoji_sequence(const std::vector<u8char>& current_cluster) {
  if (current_cluster.size() < 2u) {
    return false;
  }
  bool is_seq =
      (get_prop(current_cluster.front())
           .has_property(property::type::Ext_Pict)) &&
      (get_prop(current_cluster.back()).has_property(property::type::ZWJ));

  if (current_cluster.size() > 2u) {
    for (std::size_t c_idx = 1u; c_idx < current_cluster.size() - 1u; ++c_idx) {
      const u8char& c = current_cluster[c_idx];
      is_seq &= (get_prop(c).has_property(property::type::Extend));
    }
  }

  return is_seq;
}

// https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
bool has_break(const std::vector<u8char>& current_cluster,
               const codepoint& previous, const codepoint& current) {
  const property& previous_prop = previous.get_property();
  const property& current_prop = current.get_property();

  // Break at the start and end of text, unless the text is empty.
  // GB1/GB2 are handled implicitly

  // Do not break between a CR and LF. Otherwise, break before and after
  // controls.
  if (previous_prop.has_property(property::type::CR) &&
      current_prop.has_property(property::type::LF)) {
    // GB3
    return false;
  } else if (previous_prop.has_property({property::type::Control,
                                         property::type::CR,
                                         property::type::LF})) {
    // GB4
    return true;
  } else if (current_prop.has_property({property::type::Control,
                                        property::type::CR,
                                        property::type::LF})) {
    // GB5
    return true;
  }
  // Do not break Hangul syllable sequences.
  else if (previous_prop.has_property(property::type::L) &&
           current_prop.has_property({property::type::L, property::type::V,
                                      property::type::LV,
                                      property::type::LVT})) {
    // GB6
    return false;
  } else if (previous_prop.has_property(
                 {property::type::LV, property::type::V}) &&
             current_prop.has_property(
                 {property::type::V, property::type::T})) {
    // GB7
    return false;
  } else if (previous_prop.has_property(
                 {property::type::LVT, property::type::T}) &&
             current_prop.has_property(property::type::T)) {
    // GB8
    return false;
  }
  // Do not break before extending characters or ZWJ.
  else if (current_prop.has_property(
               {property::type::Extend, property::type::ZWJ})) {
    // GB9
    return false;
  }
  // NOTE: Currently we always assume we support extended grapheme clusters over
  // legacy grapheme clusters

  // Do not break before SpacingMarks, or after Prepend characters.
  else if (current_prop.has_property(property::type::SpacingMark)) {
    // GB9a
    return false;
  } else if (previous_prop.has_property(property::type::Prepend)) {
    // GB9b
    return false;
  }
  // Do not break within emoji modifier sequences or emoji zwj sequences.
  else if (current_prop.has_property(property::type::Ext_Pict)) {
    // GB11
    return !is_emoji_sequence(current_cluster);
  }
  // Do not break within emoji flag sequences. That is, do not break between
  // regional indicator (RI) symbols if there is an odd number of RI characters
  // before the break point.
  else if (previous_prop.has_property(property::type::RI) &&
           current_prop.has_property(property::type::RI)) {
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
#include <auc/graphemecluster.hpp>
#include <fstream>
#include <optional>
#include <sstream>
#include <tuple>
#include <unordered_map>

namespace auc {

namespace detail {

bool starts_with(const std::string& source, const std::string& sub_str) {
  // https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a
  return (source.rfind(sub_str, 0) == 0);
}

void trim_whitespace(std::string& source) {
  // https://stackoverflow.com/questions/14233065/remove-whitespace-in-stdstring

  // In our case, we're just parsing ASCII only data so don't need to worry
  // about other locales
  source.erase(std::remove_if(source.begin(), source.end(), std::isspace),
               source.end());
}

std::vector<std::string> split(const std::string& source, const char delim) {
  std::vector<std::string> parts;

  // https://stackoverflow.com/questions/5167625/splitting-a-c-stdstring-using-tokens-e-g
  std::istringstream stream(source);
  std::string part;
  while (std::getline(stream, part, delim)) {
    trim_whitespace(part);
    parts.push_back(part);
  }

  return parts;
}

std::vector<codepoint> codepoint_range(const std::string& cp) {
  // Get potential code point range
  if (cp.find("..") != std::string::npos) {
    std::uint32_t start{0};
    std::uint32_t end{0};

    if (cp.size() == 10u) {
      // U+XXXX
      start = std::stoul(cp.substr(0, 4), nullptr, 16);
      end = std::stoul(cp.substr(6, 4), nullptr, 16);
    } else if (cp.size() == 12u) {
      // U+XXXXX
      start = std::stoul(cp.substr(0, 5), nullptr, 16);
      end = std::stoul(cp.substr(7, 5), nullptr, 16);
    }

    if (start > end) return {};

    const std::size_t num_range = end - start;
    std::vector<codepoint> codepoints;
    codepoints.reserve(num_range);

    for (std::uint32_t c = start; c <= end; ++c) {
      codepoints.emplace_back(c);
    }
    return codepoints;

  } else {
    return {codepoint(std::stoul(cp, nullptr, 16))};
  }
}

// https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakProperty.txt
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
  ZWJ = 13
};

property to_property(const std::string& prop_str) {
  static std::unordered_map<std::string, property> prop_conv = {
      {"CR", property::CR},
      {"LF", property::LF},
      {"Control", property::Control},
      {"Extend", property::Extend},
      {"RI", property::RI},
      {"Prepend", property::Prepend},
      {"SpacingMark", property::SpacingMark},
      {"L", property::L},
      {"V", property::V},
      {"T", property::T},
      {"LV", property::LV},
      {"LVT", property::LVT},
      {"ZWJ", property::ZWJ}};

  if (prop_conv.find(prop_str) == prop_conv.end()) {
    return property::Other;
  }

  return prop_conv[prop_str];
}

struct grapheme_cluster_break {
  codepoint codepoint_{0};
  property prop_{property::Other};
};

static bool breaks_populated_ = false;

static std::unordered_map<std::uint32_t, grapheme_cluster_break>
    codepoint_break_lookup_ = {};

void parse_grapheme_breaks() {
  if (!breaks_populated_) {
    // TODO: take what we have here and generate output that can be included in
    // a std::unordered_map. See
    // https://github.com/jmicjm/GraphemeBreakProperty_parser for example of
    // data to output, tailor that to our enum. BONUS: See if mesonbuild can do
    // the generation. This should be possible via
    // https://stackoverflow.com/questions/65390930/how-can-i-run-a-custom-command-at-build-time-in-meson.
    // In our case, we can write a python script for the custom target called
    // graphemebreakproperty_autogen.h which can then be included in the source
    // here
    // NOTE: Potentially, if it seems like a bottleneck, keep the range information intact for more efficient data, instead
    // of unrolling to every single codepoint within a range
    std::ifstream grapheme_break_prop_file("../src/GraphemeBreakProperty.txt");

    std::string line;
    while (std::getline(grapheme_break_prop_file, line)) {
      if (starts_with(line, "#") || line.empty()) continue;

      // Split at ; then split at # and throw away the values after the #
      const std::vector<std::string> first_section = split(line, ';');
      if (first_section.size() != 2u) continue;

      const std::string cp = first_section[0];
      const std::vector<codepoint> codepoints = codepoint_range(cp);

      const std::vector<std::string> second_section =
          split(first_section[1], '#');
      if (second_section.size() != 2u) continue;

      const std::string prop_type = second_section[0];
      const property prop = to_property(prop_type);

      for (const auto& c : codepoints) {
        codepoint_break_lookup_[c.get_num()] = grapheme_cluster_break{c, prop};
      }
    }

    // https://www.unicode.org/reports/tr44/#Regional_Indicator
    // NOTE: For some reason the RI property types aren't included in
    // GraphemePropertyText.txt, so adding them manually according to the
    // Unicode Character Database spec
    std::vector<codepoint> regional_indicators =
        codepoint_range("1F1E6..1F1FF");
    const property prop = to_property("RI");
    for (const auto& c : regional_indicators) {
      codepoint_break_lookup_[c.get_num()] = grapheme_cluster_break{c, prop};
    }

    breaks_populated_ = true;
  }
}

std::vector<grapheme_cluster_break> get_char_breaks(
    const std::vector<u8char>& chars) {
  std::vector<grapheme_cluster_break> breaks;

  for (const auto& c : chars) {
    const std::uint32_t cp = c.get_codepoint().get_num();

    if (codepoint_break_lookup_.find(cp) == codepoint_break_lookup_.end()) {
      breaks.push_back(
          grapheme_cluster_break{c.get_codepoint(), property::Other});
    } else {
      breaks.push_back(codepoint_break_lookup_[cp]);
    }
  }

  return breaks;
}

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
  if (!breaks_populated_) {
    parse_grapheme_breaks();
  }

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
#include <auc/u8text.hpp>
#include <cstring>

namespace auc {

namespace detail {

std::size_t parse_bom(const char* bytes, const std::size_t length) {
  //  https://www.rfc-editor.org/rfc/rfc3629#section-6
  if (length < 3) return 0;

  auto bom_seq = [&bytes](std::size_t idx) -> bool {
    return ((bytes[idx] & 0xEF) == 0xEF && (bytes[idx + 1u] & 0xBB) == 0xBB &&
            (bytes[idx + 2u] & 0xBF) == 0xBF);
  };

  std::size_t idx{0};

  while (idx < length) {
    if (bom_seq(idx)) {
      idx += 3u;
    } else {
      break;
    }
  }

  return idx;
}
}  // namespace detail

u8text::u8text(std::string_view bytes) {
  parse_chars(bytes.data(), bytes.length());
}

u8text::u8text(const std::vector<u8char> chars) : chars_(chars) {}

u8text u8text::from_codepoints(const std::vector<codepoint>& cps) {
  std::vector<u8char> chars;
  chars.reserve(cps.size());

  for (const auto& cp : cps) {
    chars.push_back(u8char::from_codepoint(cp));
  }

  return u8text(chars);
}

bool u8text::is_valid() const {
  bool valid = true;
  for (const auto& c : chars_) {
    valid &= c.is_valid();
  }
  return valid;
}

std::vector<codepoint> u8text::get_codepoints() const {
  std::vector<codepoint> codepoints;
  codepoints.reserve(chars_.size());
  for (const auto& c : chars_) {
    codepoints.push_back(c.get_codepoint());
  }
  return codepoints;
}

std::string u8text::data() const {
  std::string raw_data;

  for (const auto& c : chars_) {
    raw_data.append(c.data());
  }

  return raw_data;
}

void u8text::parse_chars(const char* bytes, const std::size_t length) {
  std::size_t idx = detail::parse_bom(bytes, length);

  for (; idx < length;) {
    const char initial_byte = bytes[idx];

    // Peek first to get number of octets in the given utf8 character
    const std::size_t num_octets = detail::num_octets(initial_byte);
    if (num_octets < 1 || num_octets > 4) {
      // Data is not valid utf-8, continue on to parse all characters
      chars_.push_back(u8char{initial_byte});
      ++idx;
      continue;
    }

    // Up until the start + 1 index and last index
    std::vector<char> u8bytes;
    u8bytes.reserve(num_octets);
    u8bytes.push_back(initial_byte);
    for (std::size_t seq = idx + 1; seq < idx + num_octets; ++seq) {
      u8bytes.push_back(bytes[seq]);
    }

    chars_.emplace_back(std::string_view{u8bytes.data(), num_octets});

    // Skip to beginning of next utf8 character
    idx += num_octets;
  }
}

}  // namespace auc

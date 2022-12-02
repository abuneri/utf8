#include <auc/u8text.hpp>
#include <cstring>

namespace auc {

u8text::u8text(const char* chars) {
  const std::size_t num_chars = std::strlen(chars);
  parse_chars(chars, num_chars);
}

u8text::u8text(const char* bytes, const std::size_t length) {
  parse_chars(bytes, length);
}

bool u8text::is_valid() const {
  bool valid = true;
  for (const auto& c : chars_) {
    valid &= c.is_valid();
  }
  return valid;
}

void u8text::parse_chars(const char* bytes, const std::size_t length) {
  // TODO: handle Byte-Order-Mark (BOM)
  for (std::size_t idx = 0; idx < length;) {
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

    chars_.push_back(u8char{u8bytes.data(), num_octets});

    // Skip to beginning of next utf8 character
    idx += num_octets;
  }
}

}  // namespace auc
#include <auc/u8char.hpp>
#include <climits>

namespace auc {
namespace detail {

bool is_extended_ascii(const char c) { return ((c >> (CHAR_BIT - 1)) & 1); }

bool is_sequence(const char c) {
  // https://www.rfc-editor.org/rfc/rfc3629#section-3
  // Does the char contain '10' in the high bits, if yes it is
  // the 2nd-4th octet sequence for a utf-8 character
  return ((c & SeqChar) == SeqChar);
}

std::size_t num_octets(const char c) {
  // https://www.rfc-editor.org/rfc/rfc3629#section-3
  if ((c & TwoOctetChar) != TwoOctetChar) {
    // If the character is 7-bit ASCII we only needs a
    // single octet representation; however, if the character is
    // Extended ASCII (Latin-1 or Windows-1252), this is not valid utf8,
    // so we have 0 octets.
    return is_extended_ascii(c) ? 0 : 1;
  }

  if ((c & FourOctetChar) == FourOctetChar) {
    return 4;
  } else if ((c & ThreeOctetChar) == ThreeOctetChar) {
    return 3;
  } else {
    return 2;
  }
}

}  // namespace detail

u8char::u8char(const char byte) {
  valid_encoding_ = !detail::is_extended_ascii(byte);
  storage_.push_back(byte);
}

u8char::u8char(const char* bytes, const std::size_t length) {
  // TODO: extract the actual non octet/sequence high bits from the chars to
  // define the real code point
  if (length > 0) {
    const char initial_byte = bytes[0];
    const std::size_t num_octets = detail::num_octets(initial_byte);
    bool valid = (num_octets > 0 && num_octets <= 4);
    storage_.push_back(initial_byte);

    // Ensure each char contains '10' high bits. If at least one doesn't,
    // we don't have valid a UTF-8 character
    for (std::size_t idx = 1; idx < length; ++idx) {
      const char seq_byte = bytes[idx];
      valid &= detail::is_sequence(seq_byte);
      storage_.push_back(seq_byte);
    }

    valid_encoding_ = valid;
  }
}

bool u8char::is_valid() const { return valid_encoding_; }

}  // namespace auc

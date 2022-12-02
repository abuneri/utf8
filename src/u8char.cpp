#include <auc/u8char.hpp>
#include <cassert>
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

std::uint32_t to_codepoint(const std::vector<char>& encoded_bytes) {
  std::uint32_t codepoint{0};

  auto decode_bit = [&encoded_bytes, &codepoint](const std::size_t enc_byte_idx,
                                                 const std::size_t enc_bit,
                                                 const std::size_t cp_bit) {
    if ((encoded_bytes[enc_byte_idx] >> enc_bit) & 1u) {
      codepoint |= 1u << cp_bit;
    } else {
      codepoint &= ~(1u << cp_bit);
    }
  };

  auto decode_bits = [&decode_bit](const std::size_t enc_byte_idx,
                                   const std::size_t high_enc_bit,
                                   const std::size_t low_enc_bit,
                                   const std::size_t high_cp_bit,
                                   const std::size_t low_cp_bit) {
    std::size_t enc_bit = high_enc_bit;
    std::size_t cp_bit = high_cp_bit;
    for (; enc_bit >= low_enc_bit && cp_bit >= low_cp_bit;
         --enc_bit, --cp_bit) {
      decode_bit(enc_byte_idx, enc_bit, cp_bit);

      if (enc_bit == 0u || cp_bit == 0u) break;
    }
  };

  /*
   https://www.rfc-editor.org/rfc/rfc3629#section-3
   1 Octect: 0xxxxxxx
   2 Octets: 110xxxxx 10xxxxxx
   3 Octets: 1110xxxx 10xxxxxx 10xxxxxx
   4 Octets: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
  */
  switch (encoded_bytes.size()) {
    case 4u:
      // First 21 bits of the 32-bit integer need to be filled

      // The 3 LSB (2-0) from encoded_bytes index 0 fill the (20-18) bits in the
      // integer
      decode_bits(0, 2, 0, 20, 18);

      // The 6 LSB (5-0) from encoded_bytes index 1 fill the (17-12)
      // bits in the integer
      decode_bits(1, 5, 0, 17, 12);

      // The 6 LSB (5-0) from encoded_bytes index 2 fill the
      // (11-6) bits in the integer
      decode_bits(2, 5, 0, 11, 6);

      // The 6 LSB (5-0) from encoded_bytes index 3
      // fill the (5-0) bits in the integer
      decode_bits(3, 5, 0, 5, 0);
      break;
    case 3u:
      // First 16 bits of the 32-bit integer need to be filled

      // The 4 LSB (3-0) from encoded_bytes index 0 fill the (15-12) bits in the
      // integer
      decode_bits(0, 3, 0, 15, 12);

      // the 6 LSB (5-0) from encoded_bytes index 1 fill the (11-6) bits
      // in the integer
      decode_bits(1, 5, 0, 11, 6);

      // The 6 LSB (5-0) from encoded_bytes index 2 fill the
      // (5-0) bits in the integer
      decode_bits(2, 5, 0, 5, 0);
      break;
    case 2u:
      // First 11 bits of the 32-bit integer need to be filled

      // The 5 LSB (4-0) from encoded_bytes index 0 fill the (10-6) bits in the
      // integer
      decode_bits(0, 4, 0, 10, 6);

      // The 6 LSB (5-0) from encoded_bytes index 1 fill the (5-0) bits
      // in the integer
      decode_bits(1, 5, 0, 5, 0);
      break;
    case 1u:
      // Can directly convert byte value into codepoint integer
      codepoint = static_cast<std::uint32_t>(encoded_bytes[0]);
    case 0u:
    default:
      break;
  }
  return codepoint;
}

}  // namespace detail

u8char::u8char(const char byte) {
  valid_encoding_ = !detail::is_extended_ascii(byte);
  encoded_storage_.push_back(byte);

  // TODO: Is this safe? dd unit tests for when extended and 7bit ascii
  codepoint_ = static_cast<std::uint32_t>(byte);
}

u8char::u8char(const char* bytes, const std::size_t length) {
  // TODO: extract the actual non octet/sequence high bits from the chars to
  // define the real code point
  if (length > 0) {
    const char initial_byte = bytes[0];
    const std::size_t num_octets = detail::num_octets(initial_byte);
    bool valid = (num_octets > 0 && num_octets <= 4);
    encoded_storage_.push_back(initial_byte);

    // Ensure each char contains '10' high bits. If at least one doesn't,
    // we don't have valid a UTF-8 character
    for (std::size_t idx = 1; idx < length; ++idx) {
      const char seq_byte = bytes[idx];
      valid &= detail::is_sequence(seq_byte);
      encoded_storage_.push_back(seq_byte);
    }

    valid_encoding_ = valid;
    codepoint_ = detail::to_codepoint(encoded_storage_);
  }
}

bool u8char::is_valid() const { return valid_encoding_; }

}  // namespace auc

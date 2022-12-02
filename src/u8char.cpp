#include <auc/u8char.hpp>
#include <limits>

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

struct decode_params {
  std::size_t enc_byte_idx_{0};
  std::size_t high_enc_bit_{0};
  std::size_t low_enc_bit_{0};
  std::size_t high_cp_bit_{0};
  std::size_t low_cp_bit_{0};
};

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

      // Guard against underflow
      if (enc_bit == 0u || cp_bit == 0u) break;
    }
  };

  // TODO: clean up  successive calls to decode_bits by creating a vector of
  // decode_params which we can then iterate over and only call decode_bits once

  const std::size_t num_octets = encoded_bytes.size();
  switch (num_octets) {
    case 4u: {
      // First 21 bits of the 32-bit integer need to be filled
      std::vector<decode_params> params;
      params.reserve(4u);

      // The 3 LSB (2-0) from encoded_bytes index 0 fill the (20-18) bits in the
      // integer
      params.push_back(decode_params{0u, 2u, 0u, 20u, 18u});

      // The 6 LSB (5-0) from encoded_bytes index 1 fill the (17-12)
      // bits in the integer
      params.push_back(decode_params{1u, 5u, 0u, 17u, 12u});

      // The 6 LSB (5-0) from encoded_bytes index 2 fill the
      // (11-6) bits in the integer
      params.push_back(decode_params{2u, 5u, 0u, 11u, 6u});

      // The 6 LSB (5-0) from encoded_bytes index 3
      // fill the (5-0) bits in the integer
      params.push_back(decode_params{3u, 5u, 0u, 5u, 0u});

      for (const auto& param : params) {
        decode_bits(param.enc_byte_idx_, param.high_enc_bit_,
                    param.low_enc_bit_, param.high_cp_bit_, param.low_cp_bit_);
      }
      break;
    }
    case 3u: {
      // First 16 bits of the 32-bit integer need to be filled
      std::vector<decode_params> params;
      params.reserve(3u);

      // The 4 LSB (3-0) from encoded_bytes index 0 fill the (15-12) bits in the
      // integer
      params.push_back(decode_params{0u, 3u, 0u, 15u, 12u});

      // the 6 LSB (5-0) from encoded_bytes index 1 fill the (11-6) bits
      // in the integer
      params.push_back(decode_params{1u, 5u, 0u, 11u, 6u});

      // The 6 LSB (5-0) from encoded_bytes index 2 fill the
      // (5-0) bits in the integer
      params.push_back(decode_params{2u, 5u, 0u, 5u, 0u});

      for (const auto& param : params) {
        decode_bits(param.enc_byte_idx_, param.high_enc_bit_,
                    param.low_enc_bit_, param.high_cp_bit_, param.low_cp_bit_);
      }
      break;
    }
    case 2u: {
      // First 11 bits of the 32-bit integer need to be filled
      std::vector<decode_params> params;
      params.reserve(2u);

      // The 5 LSB (4-0) from encoded_bytes index 0 fill the (10-6) bits in the
      // integer
      params.push_back(decode_params{0u, 4u, 0u, 10u, 6u});

      // The 6 LSB (5-0) from encoded_bytes index 1 fill the (5-0) bits
      // in the integer
      params.push_back(decode_params{1u, 5u, 0u, 5u, 0u});

      for (const auto& param : params) {
        decode_bits(param.enc_byte_idx_, param.high_enc_bit_,
                    param.low_enc_bit_, param.high_cp_bit_, param.low_cp_bit_);
      }
      break;
    }
    case 1u:
      // Can directly convert byte value into codepoint integer
      codepoint = static_cast<std::uint32_t>(encoded_bytes[0]);
    case 0u:
    default:
      break;
  }
  return codepoint;
}

struct encode_params {
  char octet_{0};
  std::size_t low_cp_bit_{0};
  std::size_t high_cp_bit_{0};
  std::size_t low_enc_bit_{0};
  std::size_t high_enc_bit_{0};
};

std::vector<char> to_encoded_bytes(std::uint32_t codepoint) {
  /*
  https://www.rfc-editor.org/rfc/rfc3629#section-3
  Encoding a character to UTF-8 proceeds as follows:

   1.  Determine the number of octets required from the character number
       and the first column of the table above.  It is important to note
       that the rows of the table are mutually exclusive, i.e., there is
       only one valid way to encode a given character.

   2.  Prepare the high-order bits of the octets as per the second
       column of the table.

   3.  Fill in the bits marked x from the bits of the character number,
       expressed in binary.  Start by putting the lowest-order bit of
       the character number in the lowest-order position of the last
       octet of the sequence, then put the next higher-order bit of the
       character number in the next higher-order position of that octet,
       etc.  When the x bits of the last octet are filled in, move on to
       the next to last octet, then to the preceding one, etc. until all
       x bits are filled in.
   */

  std::vector<char> encoded_bytes;
  auto encode_bit = [&encoded_bytes, &codepoint](char& octet,
                                                 const std::size_t enc_bit,
                                                 const std::size_t cp_bit) {
    if ((codepoint >> cp_bit) & 1u) {
      octet |= 1 << enc_bit;
    } else {
      octet &= ~(1 << enc_bit);
    }
  };

  auto encode_bits = [&encode_bit](char octet, const std::size_t low_cp_bit,
                                   const std::size_t high_cp_bit,
                                   const std::size_t low_enc_bit,
                                   const std::size_t high_enc_bit) -> char {
    std::size_t enc_bit = low_enc_bit;
    std::size_t cp_bit = low_cp_bit;
    for (; enc_bit <= high_enc_bit && cp_bit <= high_cp_bit;
         ++enc_bit, ++cp_bit) {
      encode_bit(octet, enc_bit, cp_bit);

      // Guard against overflow
      if (enc_bit == std::numeric_limits<std::size_t>::max() ||
          cp_bit == std::numeric_limits<std::size_t>::max())
        break;
    }
    return octet;
  };

  if (codepoint >= 0x00u && codepoint <= 0x7Fu) {
    encoded_bytes.reserve(1u);

    // TODO: Is this safe? add unit tests for when extended and 7bit ascii
    encoded_bytes.push_back(static_cast<char>(codepoint));

  } else if (codepoint >= 0x80u && codepoint <= 0x7FFu) {
    std::vector<encode_params> params;
    params.reserve(2u);

    // The 6 LSB (0-5) from the integer fill the (0-5) bits in the
    // encoded octet
    char second_octet = detail::SeqChar;
    params.push_back(encode_params{second_octet, 0u, 5u, 0u, 5u});

    // The 5 higher-order bits (6-10) from the integer fill the (0-4) bits in
    // the encoded octet
    char first_octet = detail::TwoOctetChar;
    params.push_back(encode_params{first_octet, 6u, 10u, 0u, 4u});

    encoded_bytes.reserve(2u);
    for (auto itr = params.rbegin(); itr != params.rend(); ++itr) {
      auto& param = *itr;
      encoded_bytes.push_back(
          encode_bits(param.octet_, param.low_cp_bit_, param.high_cp_bit_,
                      param.low_enc_bit_, param.high_enc_bit_));
    }

  } else if (codepoint >= 0x800u && codepoint <= 0xFFFFu) {
    std::vector<encode_params> params;
    params.reserve(3u);

    // The 6 LSB (0-5) from the integer fill the (0-5) bits in the
    // encoded octet
    char third_octet = detail::SeqChar;
    params.push_back(encode_params{third_octet, 0u, 5u, 0u, 5u});

    // The 6 higher-order bits (6-11) from the integer fill the (0-5) bits in
    // the encoded octet
    char second_octet = detail::SeqChar;
    params.push_back(encode_params{second_octet, 6u, 11u, 0u, 5u});

    // The 4 higher-order bits (12-15) from the integer fill the (0-3) bits in
    // the encoded octet
    char first_octet = detail::ThreeOctetChar;
    params.push_back(encode_params{first_octet, 12u, 15u, 0u, 3u});

    encoded_bytes.reserve(3u);
    for (auto itr = params.rbegin(); itr != params.rend(); ++itr) {
      auto& param = *itr;
      encoded_bytes.push_back(
          encode_bits(param.octet_, param.low_cp_bit_, param.high_cp_bit_,
                      param.low_enc_bit_, param.high_enc_bit_));
    }

  } else if (codepoint >= 0x10000u && codepoint <= 0x10FFFFu) {
    std::vector<encode_params> params;
    params.reserve(4u);

    // The 6 LSB (0-5) from the integer fill the (0-5) bits in the
    // encoded octet
    char fourth_octet = detail::SeqChar;
    params.push_back(encode_params{fourth_octet, 0u, 5u, 0u, 5u});

    // The 6 higher-order bits (6-11) from the integer fill the (0-5) bits in
    // the encoded octet
    char third_octet = detail::SeqChar;
    params.push_back(encode_params{third_octet, 6u, 11u, 0u, 5u});

    // The 6 higher-order bits (12-17) from the integer fill the (0-5) bits in
    // the encoded octet
    char second_octet = detail::SeqChar;
    params.push_back(encode_params{second_octet, 12u, 17u, 0u, 5u});

    // The 3 higher-order bits (18-20) from the integer fill the (0-2) bits in
    // the encoded octet
    char first_octet = detail::FourOctetChar;
    params.push_back(encode_params{first_octet, 18u, 20u, 0u, 2u});

    encoded_bytes.reserve(4u);
    for (auto itr = params.rbegin(); itr != params.rend(); ++itr) {
      auto& param = *itr;
      encoded_bytes.push_back(
          encode_bits(param.octet_, param.low_cp_bit_, param.high_cp_bit_,
                      param.low_enc_bit_, param.high_enc_bit_));
    }
  }

  return encoded_bytes;
}

}  // namespace detail

u8char::u8char(const char byte) {
  valid_encoding_ = !detail::is_extended_ascii(byte);
  encoded_storage_.push_back(byte);

  // TODO: Is this safe? add unit tests for when extended and 7bit ascii
  codepoint_ = static_cast<std::uint32_t>(byte);
}

u8char::u8char(const char* bytes, const std::size_t length) {
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

u8char::u8char(const std::vector<char>& encoded_bytes,
               const bool valid_encoding)
    : encoded_storage_(encoded_bytes), valid_encoding_(valid_encoding) {}

u8char u8char::from_codepoint(codepoint cp) {
  bool valid = true;
  std::vector<char> encoded_bytes = detail::to_encoded_bytes(cp.get_num());

  if (encoded_bytes.size() == 0u) {
    valid = false;
  }
  if (encoded_bytes.size() == 1u) {
    valid = !detail::is_extended_ascii(encoded_bytes[0]);
  }

  return u8char(encoded_bytes, valid);
}

bool u8char::is_valid() const { return valid_encoding_; }

}  // namespace auc

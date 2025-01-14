#pragma once

#include <string>
#include <string_view>

#include "graphemecluster.hpp"
#include "u8char.hpp"

namespace auc {

class u8text {
 public:
  explicit u8text(std::string_view bytes);
  explicit u8text(std::u8string_view bytes);

  static u8text from_codepoints(const std::vector<codepoint>& cps);

  bool is_valid() const;

  std::vector<codepoint> get_codepoints() const;

  // TODO: std::vector<u8char> get_codepoint_characters() const;

  const std::vector<graphemecluster>& get_grapheme_clusters() const;

  std::string data() const;

  // TODO: All implementation details for the util/helper methods
  //  (e.g. length, find/contains,sort) on the class will
  //  be implemented via the list of u8char's

  // TODO: Depend on libfmt and add support for printing and formatting
  // auc::u8text.
  //  do this via __has_include to optionally define the libfmt formatter
  //  and printer if the user has the libfmt dependency

 private:
  explicit u8text(const std::vector<u8char> chars);

  void parse_chars(const char* bytes, const std::size_t length);

  std::vector<u8char> chars_;

  // TODO: Handle Grapheme Clusters, need this before we can implement utf8 text
  // things like unicode equality or starts_with/ends_with/contains/sorting
  // u8text methods
  std::vector<graphemecluster> grapheme_clusters_;
};

}  // namespace auc

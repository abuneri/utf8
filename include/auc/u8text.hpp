#pragma once

#include "u8char.hpp"

namespace auc
{

class u8text
{
public:
    // Expects null terminated data
    explicit u8text(const char* chars);

    // Expects non null terminated data
    explicit u8text(const char* bytes, const std::size_t length);

    bool is_valid() const;

    // TODO: All implementation details for the util/helper methods
    //  (e.g. length, find/contains,sort) on the class will
    //  be implemented via the list of u8char's

    // TODO: Retrieving the raw char* data from a auc::u8test will be in the form of a
    //  std::unique_ptr<char[]> so ownership is clear. The data will be built up by
    //  iterating through each auc::u8char and memcopying their storage into their
    //   respective position in the outgoing char*

    // TODO: Depend on libfmt and add support for printing and formatting auc::u8text.
    //  do this via __has_include to optionally define the libfmt formatter
    //  and printer if the user has the libfmt dependency

private:
    void parse_chars(const char* bytes, const std::size_t length);

    std::vector<u8char> chars_;
};

} // namespace auc

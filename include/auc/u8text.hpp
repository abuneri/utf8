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

private:
    void parse_chars(const char* bytes, const std::size_t length);

    std::vector<u8char> chars_;
};

} // namespace auc

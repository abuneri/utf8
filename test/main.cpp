
#include <iostream>
#include <string>
#include <auc/u8text.hpp>

static_assert(CHAR_BIT == 8, "Must be on a platform with 8-bit chars");

// TODO: Use gtest for unit tests
// TODO: convert char* into list of auc::u8char, where each instance holds their own 
//          memcopied std::vector<char> for raw storage of octets that make up a utf-8 character

// TODO: auc::u8text holds/owns the list of auc::u8char's, and all implementation details
//          for the util/helper methods (e.g. length, find/contains,sort) on the class will
//          be implemented via the list of auc::u8char's

// TODO: retrieving the raw char* data from a auc::u8test will be in the form of a 
//          std::unique_ptr<char[]> so ownership is clear. The data will be built up by 
//          iterating through each auc::u8char and memcopying their storage into their 
//          respective position in the outgoing char*

// TODO: depend on libfmt and add support for printing and formatting auc::u8text. 
//          do this via __has_include to optionally define the libfmt formatter 
//          and printer if the user has the libfmt dependency
int main()
{
    std::string msg;
    static std::string sPassMsg = "Passed!";
    static std::string sFailMsg = "Failed :(";

    char utf8_chars[] = u8"߿က$£ह€한𐍈򟿿";
    auc::u8text utf8_text(utf8_chars);
    msg = utf8_text.is_valid() ? sPassMsg : sFailMsg;
    std::cout << "utf8_chars: " << msg << '\n';

    char invalid_chars[] = "\u07FF\u1000\0024\u00A3\u0939\u20AC\uD55C\u10348\u09FFFF";
    auc::u8text invalid_text(invalid_chars);
    msg = !invalid_text.is_valid() ? sPassMsg : sFailMsg;
    std::cout << "invalid_chars: " << msg << '\n';

    char ascii_chars[] = "Hello, World!";
    auc::u8text ascii_text(ascii_chars);
    msg = ascii_text.is_valid() ? sPassMsg : sFailMsg;
    std::cout << "ascii_chars: " << msg << '\n';
}

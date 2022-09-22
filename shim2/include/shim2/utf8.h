#ifndef NOO_UTF8_H
#define NOO_UTF8_H

#include "shim2/main.h"

namespace noo {

namespace util {

SHIM2_EXPORT int utf8_len(std::string text);
SHIM2_EXPORT int utf8_len_bytes(std::string text, int char_count);
SHIM2_EXPORT Uint32 utf8_char_next(std::string text, int &offset);
SHIM2_EXPORT Uint32 utf8_char_offset(std::string text, int o);
SHIM2_EXPORT Uint32 utf8_char(std::string text, int i);
SHIM2_EXPORT std::string utf8_char_to_string(Uint32 ch);
SHIM2_EXPORT std::string utf8_substr(std::string s, int start, int count = -1);

} // End namespace util

}

#endif // NOO_UTF8_H

#pragma once

#include <string_view>
#include <algorithm>

namespace exl::util {

    template<size_t Size>
    inline void CopyString(char (&out)[Size], std::string_view in) {
        size_t length = std::min(Size-1, in.length());
        memcpy(out, in.data(), length);
        out[length] = '\0';
    }
}
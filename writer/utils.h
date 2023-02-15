#pragma once
#include <string>



template <class T>
inline std::string ToHex(T v, size_t hex_len = sizeof(T) << 1)
{

    static const char* digits = "0123456789ABCDEF";
    std::string res(hex_len + 2, '0');
    const char* str_pref = "0x";
    memcpy((void*)res.c_str(), str_pref, 2);

    for (size_t i = 2, j = (hex_len - 1) * 4; i < hex_len + 2; i++, j -= 4)
        res[i] = digits[(v >> j) & 0x0f];

    return res;

}
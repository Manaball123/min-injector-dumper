

#pragma once
#include "framework.h"


#define TITLE "DLL Dumper"
#define USE_CONSOLE 


//uncomment if u also want a log for the function calls
#define WRITE_LOG


//file io
//uncomment to specify an abseloute path for the dumps
#define USE_ABS_PATH
#define ABS_PATH L"D:/Dumps/"

namespace Logging
{


    void Setup();
    void Close();
    //void Write(const std_char* str);

    

    void Log(std::string str);
    void WLog(std::wstring str);
    inline void Log(const char* str) { Log(std::string(str)); };

    BOOL WriteBufferToDisk(LPCVOID buffer, size_t size, std::wstring appended_name);
    /*
    void Log(const wchar_t* str);
    void Log(std::wstring str);
    */

    //lazy lol

    template <class T>
    inline void Log(T v)
    {
        Log(std::to_string(v));
    };



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
    template <class T>
    inline std::string HexDump(T v)
    {
       

        char* vb = reinterpret_cast<char*>(&v);
        size_t bytes_n = sizeof(T);
        std::string res(bytes_n * 3, 0x20);
        static const char* digits = "0123456789ABCDEF";
        for (size_t i = 0; i < bytes_n; i++)
        {
            char cur_byte = vb[i];
           
            size_t offset = i * 3;
            //char 1: high 4 bits
            res[offset] = digits[(cur_byte & 0xf0) >> 4];
            //char 2: low 4 bits
            res[offset + 1] = digits[cur_byte & 0x0f];
            //char 3: space
            res[offset + 2] = 0x20;
        }
        return res;
    }
    template<class T>
    inline std::string ArgDump(T v)
    {
        return HexDump(v);
    }
    
    inline std::string ArgDump(HANDLE v)
    {
        return ToHex((size_t)v);
    }
    inline std::string ArgDump(size_t v)
    {
        return ToHex(v);
    }
    
    inline std::string ArgDump(unsigned long v)
    {
        return ToHex(v);
    }
    void Err(DWORD err);

    void SetupConsole();

    //all credits 2 this guy i have no clue what this is
    //https://bitwizeshift.github.io/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
    template <std::size_t...Idxs>
    constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>)
    {
        return std::array{ str[Idxs]..., '\n' };
    }

    template <typename T>
    constexpr auto type_name_array()
    {
#if defined(__clang__)
        constexpr auto prefix = std::string_view{ "[T = " };
        constexpr auto suffix = std::string_view{ "]" };
        constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif defined(__GNUC__)
        constexpr auto prefix = std::string_view{ "with T = " };
        constexpr auto suffix = std::string_view{ "]" };
        constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif defined(_MSC_VER)
        constexpr auto prefix = std::string_view{ "type_name_array<" };
        constexpr auto suffix = std::string_view{ ">(void)" };
        constexpr auto function = std::string_view{ __FUNCSIG__ };
#else
# error Unsupported compiler
#endif

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);

        static_assert(start < end);

        constexpr auto name = function.substr(start, (end - start));
        return substring_as_array(name, std::make_index_sequence<name.size()>{});
    }

    template <typename T>
    struct type_name_holder {
        static inline constexpr auto value = type_name_array<T>();
    };

    template <typename T>
    constexpr auto type_name() -> std::string_view
    {
        constexpr auto& value = type_name_holder<T>::value;
        return std::string_view{ value.data(), value.size() };
    }

    
#define LogArg(arg)\
Log(std::string("Value of ") + std::string(#arg) + std::string(" is: ") + ArgDump(arg) + std::string(", and type is: ") + std::string(type_name<decltype(arg)>()))
#define LogCall(fn)\
Log(std::string("----------FUNCTION CALL: ") + std::string(#fn) + std::string(" ----------"))
}


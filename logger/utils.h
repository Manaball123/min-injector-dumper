

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


    void Err(DWORD err);

    void SetupConsole();



}

BOOL WriteBufferToDisk(LPCVOID buffer, size_t size, std::wstring appended_name);
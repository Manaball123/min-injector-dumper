
/*
#include "../include/framework.h"
#include "../include/utils.h"
*/
#include "pch.h"
#define TITLE "DLL Dumper"
using namespace Logging;
namespace fs = std::filesystem;



void SetupConsole();

namespace Logging
{
    //vars
    std::wstring basePath;
    //fs::path fs_basePath;

    std::ofstream logStream;
    std::wofstream log_wstream;


    //Setup logging
    void Logging::Setup()
    {

#ifdef USE_CONSOLE

        SetupConsole();
#endif

#ifdef USE_ABS_PATH
        basePath.append(ABS_PATH);
#endif // USE_ABS_PATH

        basePath.append(std::to_wstring(static_cast<long>(time(0))));
        basePath.append(L"/");
        //create base directory for logs/dumps
        fs::create_directories(basePath);
        //fs_basePath = fs::path(basePath);

#ifdef WRITE_LOG
    //Create log file
        std::wstring fname = basePath;
        fname.append(TEXT("log.txt"));



        //fs::create_directories(fname); //add directories based on the object path (without this line it will not work)

        logStream.open(fname);



#endif
    }
    void Close()
    {
        logStream.close();
    }


    //
    void Write(std::string str)
    {
        logStream << str << std::endl;
    }
    void WWrite(std::wstring str)
    {
        log_wstream << str << std::endl;
    }
    /*
    void Log(const std_char_t* str)
    {
#ifdef USE_CONSOLE
        std::wcout << str << std::endl;

#else
        MessageBox(nullptr, str, TITLE, MB_OK);
#endif // USE_CONSOLE

#ifdef WRITE_LOG
        Write(str);
#endif

    }

    void Log(const char* str)
    {

        int len = strlen(str);
        wchar_t* buffer = new wchar_t[len + 1];
        MultiByteToWideChar(CP_ACP, 0, str, len + 1, buffer, len);
        buffer[len] = *const_cast<wchar_t*>(L"\0");
        const wchar_t* const_str = const_cast<const wchar_t*>(buffer);
        Log(const_str);
        delete[] buffer;


    }
*/
    void Log(std::string str)
    {
        Write(str);
        std::cout << str << std::endl;
    }

    void WLog(std::wstring str)
    {
        WWrite(str);
        std::wcout << str << std::endl;
    }

    /*
    void Log(std::wstring str)
    {
        Log(str);
    }
*/

    void Err(DWORD err)
    {
        Log("Error Thrown: ");
        Log(ToHex(err));
    }
    void SetupConsole()
    {
        if (!AllocConsole())
            return;

        FILE* fDummy;
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
    }


}









BOOL Logging::WriteBufferToDisk(LPCVOID buffer, size_t size, std::wstring appended_name)
{

    std::wstring fname = Logging::basePath;

    fname.append(appended_name);


    HANDLE hFile = CreateFile(fname.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD bytesWritten;

    Log("Writing buffer...");
    Log(fname);
    //write dumped memory to file
    if (!WriteFile(hFile, buffer, size, &bytesWritten, nullptr))
    {
        Err(GetLastError());
        return STAT_ERR;
    };
    Log("Success!");

    CloseHandle(hFile);

    return STAT_OK;
}

std::string Logging::WStrToStr(const wchar_t* in)
{
    std::string out;
    unsigned int codepoint = 0;
    for (in; *in != 0; ++in)
    {
        if (*in >= 0xd800 && *in <= 0xdbff)
            codepoint = ((*in - 0xd800) << 10) + 0x10000;
        else
        {
            if (*in >= 0xdc00 && *in <= 0xdfff)
                codepoint |= *in - 0xdc00;
            else
                codepoint = *in;

            if (codepoint <= 0x7f)
                out.append(1, static_cast<char>(codepoint));
            else if (codepoint <= 0x7ff)
            {
                out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else if (codepoint <= 0xffff)
            {
                out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else
            {
                out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            codepoint = 0;
        }
    }
    return out;
}



void SetupConsole()
{
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

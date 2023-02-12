#include "pch.h"
using namespace Logging;

HANDLE WINAPI API::Hook_OpenProcess(DWORD access, BOOL inheritHandle, DWORD pid)
{
    return orig_OpenProcess(access, inheritHandle, pid);
}

BOOL WINAPI API::Hook_ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    
    static int call_ctr = 0;
    Log("ReadProcessMemory is called.");
    Log("Process is Reading from address: ");
    Log(ToHex((size_t)lpBaseAddress));
    Log("Total size wrote: ");
    Log(nSize);


    std::wstring fname = L"RPM";
    fname.append(std::to_wstring(call_ctr++));
    fname.append(L".bin");
    call_ctr++;
    WriteBufferToDisk(lpBuffer, nSize, fname);
    //create file for dumping


    return orig_ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);

}

BOOL WINAPI API::Hook_WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten)
{
    using namespace Logging;
    static int call_ctr = 0;
    Log("WriteProcessMemory is called.");
    Log("Process is Writing to address: ");
    Log(ToHex((size_t)lpBaseAddress));
    Log("Total size wrote: ");
    Log(nSize);


    std::wstring fname = L"WPM";
    fname.append(std::to_wstring(call_ctr++));
    fname.append(L".bin");
    call_ctr++;
    WriteBufferToDisk(lpBuffer, nSize, fname);
    //create file for dumping


    return orig_WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);

}

HMODULE WINAPI API::Hook_LoadLibraryA(LPCSTR name)
{
    HMODULE handle = orig_LoadLibraryA(name);
    Log("LoadLibraryA called.\n File name:" + std::string(name));
    Log("Returned handle value of " + ToHex((uint64)handle));
    return handle;
}
HMODULE WINAPI API::Hook_LoadLibraryW(LPCWSTR name)
{
    HMODULE handle = orig_LoadLibraryW(name);
    Log("LoadLibraryW called.\n File name:");
    WLog(std::wstring(name));
    Log("Returned handle value of " + ToHex((uint64)handle));
    return handle;
}

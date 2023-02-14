#include "pch.h"
using namespace Logging;

HANDLE WINAPI API::Hook_OpenProcess(DWORD access, BOOL inheritHandle, DWORD pid)
{
    return orig_OpenProcess(access, inheritHandle, pid);
}

BOOL WINAPI API::Hook_ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    
    static int call_ctr = 0;
    LogCall(ReadProcessMemory);
    LogArg(hProcess);
    LogArg(lpBaseAddress);
    LogArg(lpBuffer);
    Log(nSize);


    std::wstring fname = L"RPM";
    fname.append(std::to_wstring(call_ctr++));
    fname.append(L".bin");
    call_ctr++;

    BOOL res = orig_ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
    WriteBufferToDisk(lpBuffer, nSize, fname);
    //create file for dumping


    return res;

}

BOOL WINAPI API::Hook_WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten)
{
    using namespace Logging;
    static int call_ctr = 0;
    LogCall(WriteProcessMemory);
    LogArg(hProcess);
    LogArg(lpBaseAddress);
    LogArg(lpBuffer);

    LogArg(nSize);


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

HMODULE WINAPI API::Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{

    return orig_LoadLibraryExW(lpLibFileName, hFile, dwFlags);
}



LPVOID WINAPI API::Hook_VirtualAllocEx(
    _In_ HANDLE hProcess,
    _In_opt_ LPVOID lpAddress,
    _In_ SIZE_T dwSize,
    _In_ DWORD flAllocationType,
    _In_ DWORD flProtect
)
{
    LogCall(VirtualAllocEx);
    LogArg(lpAddress);
    LogArg(dwSize);
    LogArg(flAllocationType);
    return orig_VirtualAllocEx(hProcess, lpAddress, dwSize, flAllocationType, flProtect);
}


HANDLE WINAPI API::Hook_CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
    
    return orig_CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);

}


BOOL WINAPI API::Hook_CloseHandle(HANDLE handle)
{
    //Log("Closing handle with value " + HexDump(handle));
    return orig_CloseHandle(handle);
}


NTSTATUS NTAPI API::Hook_NtCreateThreadEx(
    OUT PHANDLE hThread,
    IN ACCESS_MASK DesiredAccess,
    IN PVOID ObjectAttributes,
    IN HANDLE ProcessHandle,
    IN PVOID lpStartAddress,
    IN PVOID lpParameter,
    IN ULONG Flags,
    IN SIZE_T StackZeroBits,
    IN SIZE_T SizeOfStackCommit,
    IN SIZE_T SizeOfStackReserve,
    OUT PVOID lpBytesBuffer
)
{
   
    
    NTSTATUS res = orig_NtCreateThreadEx(hThread, DesiredAccess, ObjectAttributes, ProcessHandle, lpStartAddress, lpParameter, Flags, StackZeroBits, SizeOfStackCommit, SizeOfStackReserve, lpBytesBuffer);
    if (ProcessHandle == INVALID_HANDLE_VALUE)
        return res;

    LogCall(NtCreateThreadEx);
    LogArg(*hThread);
    LogArg(ProcessHandle);
    LogArg(lpStartAddress);
   
    

    return res;
}


NTSTATUS NTAPI API::Hook_NtOpenProcess(

    OUT PHANDLE             ProcessHandle,
    IN ACCESS_MASK          AccessMask,
    IN POBJECT_ATTRIBUTES   ObjectAttributes,
    IN PCLIENT_ID           ClientId)
{
    
    
    //make sure that shit is null terminated
    
    
    NTSTATUS res = orig_NtOpenProcess(ProcessHandle, AccessMask, ObjectAttributes, ClientId);
    //only log succeeded calls(luckycharmz only prob)
    if (res != 0)
        return res;
    LogCall(NtOpenProcess);
       
    LogArg(*ProcessHandle);
    LogArg(AccessMask);
    //LogArg(*ObjectAttributes);
    //LogArg(ObjectAttributes->ObjectName);
    LogArg(ClientId->UniqueProcess);

    
    
    return res;
    
}


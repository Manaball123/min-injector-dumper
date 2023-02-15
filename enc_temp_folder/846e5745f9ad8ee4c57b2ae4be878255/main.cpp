// writer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <unordered_map>
#include "instruction.h"
#include <fstream>
#include <TlHelp32.h>
//thanx logger
NTSTATUS NTAPI NtCreateThreadEx(
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
);


struct MappingData
{
    LPVOID pBase;
    LPVOID pDllMain;
    LPVOID addr1;
    LPVOID addr2;
    DWORD reserved_maybe[2] = { 0x764912A0 ,0x7648FB80 };

};

//int a = sizeof(MappingData);

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







int main()
{
    int a;
    //maybe u need to unset the hex idk
    std::vector<char*> buffers;
    buffers.reserve(9);
    for (int i = 0; i < 9; i++)
    {
        
        std::string fname = std::string("buffers/WPM") + std::to_string(i) + std::string(".bin");
        std::ifstream file(fname, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        char* buffer = (char*)malloc(size);
        if (file.read(buffer, size))
        {
            std::cout << fname << " is now at address " << ToHex((size_t)buffer) << " with size " << ToHex((size_t)size) << "\n";
        }
        buffers.push_back(buffer);



    }
    /*
    while (1)
    {





        std::string fname;
        std::cout << "Enter function name: ";
        std::cin >> fname;
        std::cout << "\n";
        auto map_it = Exec::instructions_table.find(fname);

        if (map_it != Exec::instructions_table.end())
        {
            std::cout << "Enter args: \n";
            Exec::instructions_table[fname]->Callback();
            
        }
        else
        {
            std::cout << "Invalid Function name!\n";
        }
        
    }
    */

    using namespace std;
    //fuck making the api, gonna just write a temporary one
    HANDLE hProcess = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (lstrcmpW(entry.szExeFile, L"csgo.exe") == 0)
            {
                hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);


               
            }
        }
    }
    CloseHandle(snapshot);
    
    
    //hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 34504);
    if (hProcess == INVALID_HANDLE_VALUE)
    {
        cout << "L"; 
        return 0;
    }
        

    BYTE* baseAddress = (BYTE*)VirtualAllocEx(hProcess, NULL, 0x011EC000,MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    int ctr = 0;
#define WriteMem(addr, size) WriteProcessMemory(hProcess, addr, buffers[ctr], size, nullptr); std::cout << "written buffer" << ctr << std::endl; ctr++; 
    WriteMem(baseAddress, 0x00000400);
    WriteMem(baseAddress + 0x00001000, 0x0006F000);
    WriteMem(baseAddress + 0x00070000, 0x0001E400);
    WriteMem(baseAddress + 0x0008F000, 0x00001400);
    WriteMem(baseAddress + 0x011E0000, 0x00002800);
    WriteMem(baseAddress + 0x011E3000, 0x00000200);
    WriteMem(baseAddress + 0x011E4000, 0x00007800);


    BYTE* scAddress = (BYTE*)VirtualAllocEx(hProcess, NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    MappingData data = { 0 };
    data.pBase = baseAddress    + 0x00000000;
    data.pDllMain = baseAddress + 0x00000108;
    data.addr1 = baseAddress    + 0x011E4000;
    data.addr2 = baseAddress    + 0x0008C408;


    WriteProcessMemory(hProcess, scAddress, &data, 0x18, NULL);
    
    ctr++;
    WriteMem(scAddress + 0x18, 0x100);

    using PTR_NtCreateThreadEx = decltype(NtCreateThreadEx)*;
    PTR_NtCreateThreadEx pNtCreateThreadEx = (PTR_NtCreateThreadEx)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtCreateThreadEx");

    HANDLE hThread;
    DWORD arg;
    

    NTSTATUS status = pNtCreateThreadEx(
        &hThread,
        0x1FFFFF,
        NULL,
        hProcess,
        (LPTHREAD_START_ROUTINE)(scAddress + 0x18),
        //this may not be null
        scAddress,
        FALSE, //start instantly
        NULL,
        NULL,
        NULL,
        NULL
    );
    //credits: https://github.com/3gstudent/Inject-dll-by-APC/blob/master/NtCreateThreadEx.cpp line 90

    if (hThread == NULL)
    {
        CloseHandle(hProcess);
        printf("[!]ThreadHandle error\n");
        return FALSE;
    }
    if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
    {
        printf("[!]WaitForSingleObject error\n");
        return FALSE;
    }
    printf("Thread executed prolly");
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return TRUE;



    
}

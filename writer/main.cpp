// writer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <unordered_map>
#include "instruction.h"
#include <fstream>
#include <TlHelp32.h>
//
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
    LPVOID pPEH;
    LPVOID addr1;
    LPVOID addr2;
    LPVOID pLoadLibraryA;
    LPVOID pGetProcAddress;

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






using namespace std;
int main()
{

    LPVOID kek = (LPVOID)LoadLibraryA;
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

    //std::cout << std::hex << (size_t)LoadLibraryA;
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
        
    /*
    //shit below is NOT called
#define ImageBase 0x4EA90000
//#define ImageSize 0x00090400
#define ImageSize 0x011EC000
    BYTE* buf = new BYTE[ImageSize];
    LPVOID pBase = (LPVOID)ImageBase;
    cout << "helloz";
    //VirtualProtectEx(hProcess, ImageBase, )
    cout << ReadProcessMemory(hProcess, pBase, buf, ImageSize, NULL) << endl;
    cout << GetLastError();
    ofstream ofs("lukicharmz.dll");
    ofs.write((const char*)(buf), ImageSize);
    return 0;
    */


    BYTE* baseAddress = (BYTE*)VirtualAllocEx(hProcess, NULL, 0x011EC000,MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    int ctr = 0;
#define WriteMem(addr, size) std::cout << "WPM returned: " << WriteProcessMemory(hProcess, addr, buffers[ctr], size, nullptr); std::cout << "\n written buffer" << ctr << std::endl; ctr++; 
    WriteMem(baseAddress, 0x00000400);
    WriteMem(baseAddress + 0x00001000, 0x0006F000);
    WriteMem(baseAddress + 0x00070000, 0x0001E400);
    WriteMem(baseAddress + 0x0008F000, 0x00001400);
    WriteMem(baseAddress + 0x011E0000, 0x00002800);
    WriteMem(baseAddress + 0x011E3000, 0x00000200);
    WriteMem(baseAddress + 0x011E4000, 0x00007800);


    BYTE* scAddress = (BYTE*)VirtualAllocEx(hProcess, NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    DWORD oldp;
    //VirtualProtectEx(hProcess, scAddress, 0x1000, PAGE_EXECUTE_READWRITE, &oldp);
    MappingData data = { 0 };
    data.pBase = baseAddress    + 0x00000000;
    data.pPEH = baseAddress + 0x00000108;
    data.addr1 = baseAddress    + 0x011E4000;
    data.addr2 = baseAddress    + 0x0008C408;
    data.pLoadLibraryA = LoadLibraryA;
    data.pGetProcAddress = GetProcAddress;


    WriteProcessMemory(hProcess, scAddress, &data, sizeof(MappingData), NULL);
    
    ctr++;
    WriteMem(scAddress + 0x18, 0x100);

    using PTR_NtCreateThreadEx = decltype(NtCreateThreadEx)*;
    PTR_NtCreateThreadEx pNtCreateThreadEx = (PTR_NtCreateThreadEx)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtCreateThreadEx");
    using PTR_DllMain = BOOL (__stdcall*)(HMODULE, DWORD, LPVOID);

    
    HANDLE hThread;
    
    BYTE* shellCodeStart = scAddress + 0x18;
    NTSTATUS status = pNtCreateThreadEx(
        &hThread,                                   
        0x1FFFFF,
        NULL,
        hProcess,
        (LPTHREAD_START_ROUTINE)shellCodeStart,
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


using pSCStart = DWORD(WINAPI*)(MappingData*);
struct SCData
{
    MappingData* mapData;
    pSCStart start;
};


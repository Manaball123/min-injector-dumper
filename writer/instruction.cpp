#include "instruction.h"
#include <iostream>
#include "vars.h"
#include "utils.h"

using namespace APICMD;
using namespace std;





/*
BOOL
WINAPI
WriteProcessMemory(
    _In_ HANDLE hProcess,
    _In_ LPVOID lpBaseAddress,
    _In_reads_bytes_(nSize) LPCVOID lpBuffer,
    _In_ SIZE_T nSize,
    _Out_opt_ SIZE_T* lpNumberOfBytesWritten
    );
*/
void CMDWriteProcessMemory::Callback()
{
    HANDLE hProcess;
    LPVOID lpBaseAddress;
    LPCVOID buffer;
    SIZE_T nSize;
    SIZE_T* lpNumberOfBytesWritten;

    size_t args[5];
    cin >> hex >> args[0] >> args[1] >> args[2] >> args[3] >> args[4];

    WriteProcessMemory((HANDLE)args[0], (LPVOID)args[1], (LPCVOID)args[2], (SIZE_T)args[3], (SIZE_T*)args[4]);


    
}

void CMDCreateBuffer::Callback()
{
    std::string name;
    size_t buf_size;
    cin >> name >> hex >> buf_size;
    void* buf = malloc(buf_size);
    Vars::buffers.insert(std::pair<std::string, void*>(name,buf));
    cout << "Buffer Address: " << ToHex((size_t)buf);
   


    
}

void CMDCreateRemoteThreadEx::Callback()
{

    cout << "Returned Value: " << ToHex((size_t)CreateRemoteThreadEx((HANDLE)args[0],
        NULL,
        args[1],
        (LPTHREAD_START_ROUTINE)args[2],
        NULL,
        args[3],
        NULL,
        NULL
        ));
}

void CMDOpenProcess::Callback()
{
    
    cout << "Returned value: " << ToHex((size_t)OpenProcess(GENERIC_ALL, FALSE, args[0]));
    
}

void CMDVirtualAllocEx::Callback()
{
    size_t args[2];
    for (auto& i : args)
    {
        cin >> hex >> i;
    }
    cout << "Returned value: " << ToHex((size_t)VirtualAllocEx(
        (HANDLE)args[0],
        NULL,
        args[1],
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE


    ));
}
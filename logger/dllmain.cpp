// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
using namespace Logging;
void CloseThread(HMODULE hModule)
{
    Log("Closing Thread\n ");
    FreeLibraryAndExitThread(hModule, 0);
}


DWORD WINAPI MainThread(HMODULE hModule)
{

  //  MessageBoxA(nullptr, "Thread started", "WH", MB_ICONEXCLAMATION | MB_OK);
    Logging::Setup();

    if (MH_Initialize() != MH_OK)
    {
        Log("Failed to initialize MH\n");
        CloseThread(hModule);
        return -1;
    }
    Log("Hooking functions...");
    if (Hooking::InitAll() != STAT_OK)

    {
        Log("Failed to initialize hooks");
        return -1;
    }

    Log("Enabling all hooks...");
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        Log("MH_EnableHook(MH_ALL_HOOKS) failed.\n");
        CloseThread(hModule);
        return -1;
    }


    //std::cout << "Hooks set.\n Press enter to terminate thread... \n";
    Log("Hooks set.\nPress 'END' to terminate thread... \n");

    while (!GetAsyncKeyState(VK_END))
    {

        Sleep(100);
    }

    Log("Thread terminated.\nExiting... \n");
    CloseThread(hModule);
    return 0;

}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        
        
        auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hModule, 0, nullptr);
        if (thread)
        {
            CloseHandle(thread);
        }
        
        

    }

    return TRUE;
}


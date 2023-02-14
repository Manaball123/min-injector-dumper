#pragma once
#include "framework.h"


struct APIFunction
{
	void* fn;
	void** orig;
	HMODULE h_module;
	const char* proc_name;
	size_t callbacks_len;
	std::vector<void*> callbacks;
};

#define DeclFn(n)\
using fn_##n = decltype(n);\
inline decltype(n)* orig_##n;\
fn_##n Hook_##n;


#define N_FUNCS 0
#define INCREMENT_N_FUNCS(N)
#define AddFn(n,mod) APIFunction{(void*)&Hook_##n, (void**)&orig_##n, mod, #n},

#define AddKernel32(n) AddFn(n, kernel32)
#define AddNtDll(n) AddFn(n, ntdll)


namespace API
{
	inline HMODULE kernel32 = LoadLibrary(L"kernel32.dll");
	inline HMODULE ntdll = LoadLibrary(L"ntdll.dll");

	//kernel32
	DeclFn(OpenProcess);
	DeclFn(ReadProcessMemory);
	DeclFn(WriteProcessMemory);

	DeclFn(LoadLibraryA);
	DeclFn(LoadLibraryW);
	DeclFn(LoadLibraryExA);
	DeclFn(LoadLibraryExW);
	DeclFn(CreateThread);
	DeclFn(CloseHandle);
	DeclFn(VirtualAllocEx);
	DeclFn(VirtualFreeEx);
	DeclFn(VirtualProtectEx);


	//ntdll
	//needs to be manually declared

	DeclFn(NtCreateThreadEx);
	DeclFn(NtOpenProcess);


	/*
	template<typename T>
	class ecksdee;

	template<typename R, typename...Args>
	class ecksdee<std::function<R(Args...)>>
	{
	public:
		using args_pack = Args;
	};
	using args_LoadLibraryA = ecksdee<decltype(LoadLibraryA)>;
	*/

	

	inline APIFunction funcs[] = {
		AddKernel32(OpenProcess)
		AddKernel32(ReadProcessMemory)
		AddKernel32(WriteProcessMemory)
		AddKernel32(LoadLibraryExW)
		//AddKernel32(LoadLibraryA)
		//AddKernel32(LoadLibraryW)
		//AddKernel32(CreateThread)
		//AddKernel32(CloseHandle)
		AddKernel32(VirtualAllocEx)
		AddNtDll(NtCreateThreadEx)
		AddNtDll(NtOpenProcess)
	};

}


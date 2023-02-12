#pragma once
#include "framework.h"


struct APIFunction
{
	void* fn;
	void** orig;
	HMODULE h_module;
	const char* proc_name;
};

#define DeclFn(n)\
using fn_##n = decltype(n);\
inline decltype(n)* orig_##n;\
fn_##n Hook_##n;


#define N_FUNCS 0
#define INCREMENT_N_FUNCS(N)
#define AddFn(n,mod) APIFunction{(void*)&Hook_##n, (void**)&orig_##n, mod, #n},

#define AddKernel32(n) AddFn(n, kernel32)


namespace API
{
	inline HMODULE kernel32 = LoadLibrary(L"kernel32.dll");
	DeclFn(OpenProcess);
	DeclFn(ReadProcessMemory);
	DeclFn(WriteProcessMemory);

	DeclFn(LoadLibraryA);
	DeclFn(LoadLibraryW);

	

	inline APIFunction funcs[] = {
		AddKernel32(OpenProcess)
		AddKernel32(ReadProcessMemory)
		AddKernel32(WriteProcessMemory)
		AddKernel32(LoadLibraryA)
		AddKernel32(LoadLibraryW)
	};

}


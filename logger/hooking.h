
#pragma once
#include "framework.h"
#include "api-typedefs.h"

namespace Hooking
{

	inline HMODULE kernel32_dll = LoadLibrary(L"kernel32.dll");


	template <typename T1, typename T2>
	int InitHook(HMODULE from_module, const char* proc_name, T1 detour, T2 ppOrig)
	{


		void* address = GetProcAddress(from_module, proc_name);

		if (address == nullptr)
		{
			Logging::Log(std::string("Cannot find function: ") + proc_name);
			return STAT_ERR;
		}
		if (MH_CreateHook(address, reinterpret_cast<void*>(detour), reinterpret_cast<void**>(ppOrig)) != MH_OK)
		{
			Logging::Log(std::string("Failed to hook function: ") + proc_name);
			return STAT_ERR;
		}
		Logging::Log(std::string("Hooked function: ") + proc_name);
		return STAT_OK;
	}
	template <typename T1, typename T2>
	int InitKernel32(const char* proc_name, T1 detour, T2 ppOrig)
	{
		InitHook(kernel32_dll, proc_name, detour, ppOrig);
	}

	inline int InitAll()
	{
		constexpr int arr_size = (sizeof(API::funcs) / sizeof(APIFunction));
		for (int i = 0; i < arr_size; i++)
		{
			APIFunction& func = API::funcs[i];
			InitHook(func.h_module, func.proc_name, func.fn, func.orig);
		}
		return STAT_OK;
	}
}


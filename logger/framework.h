#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "decls.h"

#include <functional>
#include <string>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <array>
#include <utility>
#include <iostream>

//#include <ntddkbd.h>
//minhook library
#if _WIN64
#pragma comment(lib, "lib/minhook/libMinHook.x64.lib")
#else 
#pragma comment(lib, "lib/minhook/libMinHook.x86.lib")
#endif
#include "lib/minhook/MinHook.h"
#include "utils.h"
using uint64 = unsigned __int64;
using uint32 = unsigned __int32;
using int64 = __int64;
constexpr int STAT_OK = 1;
constexpr int STAT_ERR = 0;
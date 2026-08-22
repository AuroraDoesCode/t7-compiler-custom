#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <istream>
#include <iostream>
#include <strsafe.h>

#define SystemHandleInformation 16
#include <Windows.h>
#include <iostream>
#include <Winternl.h>

#include <windows.h>
#include <stdio.h>

#define NT_SUCCESS(x) ((x) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004

#define SystemHandleInformation 16
#define ObjectBasicInformation 0
#define ObjectNameInformation 1
#define ObjectTypeInformation 2
#define EXPORT extern "C" __declspec(dllexport)

constexpr uint32_t fnv_base_32 = 0x4B9ACE2F;

inline uint32_t fnv1a(const char* key) {

	const char* data = key;
	uint32_t hash = 0x4B9ACE2F;
	while (*data)
	{
		hash ^= *data;
		hash *= 0x1000193;
		data++;
	}
	hash *= 0x1000193; // bo3 wtf lol
	return hash;

}

template <typename T> void chgmem(__int64 addy, T copy)
{
	DWORD oldprotect;
	VirtualProtect((void*)addy, sizeof(T), PAGE_EXECUTE_READWRITE, &oldprotect);
	*(T*)addy = copy;
	VirtualProtect((void*)addy, sizeof(T), oldprotect, &oldprotect);
}

// Supported Bo3 versions
enum class Bo3Version
{
    Steam2023, // Downpatched Steam
    Steam2026, // Current Steam
    MSStore // Bo3 Enhanced
};

extern Bo3Version g_Bo3Version;

void chgmem(__int64 addy, __int32 size, void* copy);

extern const char MSELECT[];



#define OFFSET_S(off) (*(uint64_t*)((uint64_t)(NtCurrentTeb()->ProcessEnvironmentBlock) + 0x10) + (uint64_t)off)

inline uint64_t SelectOffset(uint64_t off2023, uint64_t off2026, uint64_t offMS)
{
	switch (g_Bo3Version)
	{
	case Bo3Version::Steam2023: return OFFSET_S(off2023);
	case Bo3Version::Steam2026: return OFFSET_S(off2026);
	case Bo3Version::MSStore:   return OFFSET_S(offMS);
	default:                    return OFFSET_S(off2026);
	}
}

#define REBASE(steam2023, steam2026, msstore) SelectOffset(steam2023, steam2026, msstore)

// Opcional: mantener compatibilidad con IS_WINSTORE
#define IS_WINSTORE (g_Bo3Version == Bo3Version::MSStore)
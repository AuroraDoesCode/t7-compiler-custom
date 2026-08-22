#include "framework.h"
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#include <fstream>
#include <iostream>

constexpr auto BO3_MSSTORE = "72c8a21763adbfac9e1b2bcd6f93b05ecf437610e16430d99a1680ea0f827c17";
constexpr auto BO3_STEAM_2023 = "66b95eb4667bd5b3b3d230e7bed1d29ccd261d48ca2699f01216c863be24ff44";
constexpr auto BO3_STEAM_2026 = "9ba98dba41e18ef47de6c63937340f8eae7cb251f8fbc2e78d70047b64aa15b5";


Bo3Version g_Bo3Version = Bo3Version::Steam2026;  // Default is Steam2026

// Hash the bo3.exe to detect the game version
std::string Sha256File(const wchar_t* path)
{
	HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return "";

	BCRYPT_ALG_HANDLE hAlg = NULL;
	BCRYPT_HASH_HANDLE hHash = NULL;
	UCHAR hash[32];
	DWORD hashLen = 32;

	BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
	BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);

	BYTE buffer[4096];
	DWORD bytesRead = 0;
	while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0)
	{
		BCryptHashData(hHash, buffer, bytesRead, 0);
	}

	BCryptFinishHash(hHash, hash, hashLen, 0);
	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	CloseHandle(hFile);

	char hex[65];
	for (int i = 0; i < 32; i++)
		sprintf_s(hex + i * 2, 3, "%02x", hash[i]);
	hex[64] = 0;

	return std::string(hex);
}

// Get the file path from process handle and hash it
std::string GetModuleFilePath(HMODULE hMod)
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(hMod, path, MAX_PATH);
	return Sha256File(path);
}

Bo3Version DetectBo3Version()
{
	auto base = (uint8_t*)GetModuleHandle(NULL);

	std::string hash = GetModuleFilePath(GetModuleHandle(NULL));

	char buf[128];
	sprintf_s(buf, "Bo3 EXE SHA-256: %s\n", hash.c_str());

    // Debug, useful in case Bo3 gets another update
	//std::ofstream log("t7logfile.txt", std::ios_base::app | std::ios_base::out);
	//log << "\nGame Version:" << "";

	Bo3Version version;

	// Get game version by hash
	if (hash == BO3_MSSTORE ){
		version = Bo3Version::MSStore;
		//log << "Microsoft Store" << buf;
	}
	else if (hash == BO3_STEAM_2023 ){
		version = Bo3Version::Steam2023;
		//log << "Steam 2023" << buf;
	}
	else if (hash == BO3_STEAM_2026 ){
		version = Bo3Version::Steam2026;
		//log << "Steam 2026" << buf;
	}
	// If not found, assume latest Steam Version
	else {
		version = Bo3Version::Steam2026;
		//log << "Couldnt find Game version for sha256" << buf;
	}

	//log << "\n";
	//log.close();
	return version;
}


#pragma optimize("",off)
bool is_tls_initialized = false;
void NTAPI tls_callback(PVOID DllHandle, DWORD dwReason, PVOID)
{
	if (is_tls_initialized)
	{
		return;
	}
	is_tls_initialized = true;

	// Get the Bo3 version and store it in the global variable
	g_Bo3Version = DetectBo3Version();
}

#pragma optimize("",on)

#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:tls_callback_func") 
#pragma const_seg(".CRT$XLF")
EXTERN_C const
PIMAGE_TLS_CALLBACK tls_callback_func = tls_callback;
#pragma const_seg()

void chgmem(__int64 addy, __int32 size, void* copy)
{
	DWORD oldprotect;
	VirtualProtect((void*)addy, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy((void*)addy, copy, size);
	VirtualProtect((void*)addy, size, oldprotect, &oldprotect);
}
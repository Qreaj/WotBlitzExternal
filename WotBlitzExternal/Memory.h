#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

namespace Memory {
	struct moduleInfo {
    uintptr_t baseAddr,size;
};
	DWORD getPID(const wchar_t* procName);
	 moduleInfo GetModuleInfo(DWORD processId, const wchar_t* moduleName);
	uintptr_t FindPattern(HANDLE handle,uintptr_t baseAddr, size_t size, string pattern);
		template <typename T>
	constexpr T RPM(HANDLE hProc, const uintptr_t& address) {
		T value = {};
		::ReadProcessMemory(hProc, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}
	template <typename T>
	constexpr void  WPM(HANDLE hProc, const uintptr_t& address, const T& value) {
		::WriteProcessMemory(hProc, (BYTE*)address, &value, sizeof(T), 0);

	}
	template <typename T>
void RPM(HANDLE hProc, const uintptr_t& address, T* buffer, size_t size) {
    ::ReadProcessMemory(hProc, reinterpret_cast<const void*>(address), buffer, size, NULL);
}
}
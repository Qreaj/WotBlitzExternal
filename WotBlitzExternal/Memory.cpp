#include "Memory.h"


DWORD Memory::getPID(const wchar_t* processName) {
	int procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry)) {
			do
			{
				if (!_wcsicmp(processName, procEntry.szExeFile)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

Memory::moduleInfo Memory::GetModuleInfo(DWORD processId, const wchar_t* moduleName) {
	uintptr_t moduleBase = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry; modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do
			{
				if (!_wcsicmp(moduleName, modEntry.szModule)) {
						return {
						(uintptr_t)modEntry.modBaseAddr,
						(uintptr_t)modEntry.modBaseSize
							};
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return {};
}

uintptr_t Memory::FindPattern(HANDLE handle,uintptr_t baseAddr, size_t size, string pattern) {
    std::istringstream iss(pattern);
    std::vector<uint8_t> signature;
    std::string token;

    while (iss >> token) {
        if (token == "?") {
            signature.push_back(0xFF);
        } else {
            try {
                int value = std::stoi(token, 0, 16);
                signature.push_back(static_cast<uint8_t>(value & 0xFF));
            } catch (std::invalid_argument& e) {
                std::cerr << "Invalid hex character in pattern: " << e.what() << std::endl;
                return 0;
            }
        }
    }

    std::vector<uint8_t> memBuffer(size);
    if (!ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(baseAddr), memBuffer.data(), size, NULL)) {
        std::cerr << "Failed to read process memory." << std::endl;
        return 0;
    }

    for (size_t i = 0; i < size - signature.size(); ++i) {
        bool found = true;
        for (size_t j = 0; j < signature.size(); ++j) {
            if (signature[j] != 0xFF && signature[j] != memBuffer[i + j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddr + i;
        }
    }
    return 0; 
}
#include <iostream>
#include "Memory.h"
#include <cstdint>

bool IsHandleValid(HANDLE h) {
    DWORD exitCode;
    return GetExitCodeProcess(h, &exitCode) && exitCode == STILL_ACTIVE;
}
int main() {
	int pid = Memory::getPID(L"wotblitz.exe");
	if (pid == 0) {
		cout << "Game not found\n";
		system("pause");
		return -1;
	}
	cout << "Cheat made by qreaj\n";
	cout << "https://github.com/Qreaj/WotBlitzExternal\n";
	cout << "Press F1 to enable/disable esp\n";
	cout << "Press F2 to enable/disable nospread\n";
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS,0,pid);
	Memory::moduleInfo minfo = Memory::GetModuleInfo(pid,L"wotblitz.exe");

	uintptr_t espAddr = Memory::FindPattern(handle,minfo.baseAddr,(uintptr_t)minfo.size,"8a 41 ? c3 cc cc cc cc cc cc cc cc cc cc cc cc 8d 41");
	uintptr_t noSpreadAddr = Memory::FindPattern(handle,minfo.baseAddr,(uintptr_t)minfo.size,"F3 0F ? ? ? ? ? ? F3 0F ? ? ? F2 0F ? ? ? ? ? ? F2 0F ? ? ? 0F 57 C0 F3 0F ? ? ? F3 0F ? ? ? F3 0F ? ? ? 0F");

	byte espAssembly[6];
	byte spreadAssembly[8];

	Memory::RPM(handle,espAddr,espAssembly,6);
	Memory::RPM(handle,noSpreadAddr,spreadAssembly,8);

	bool esp,nospread;
	esp = true;
	nospread = true;
	while(IsHandleValid(handle)) {
		if (GetAsyncKeyState(VK_F1) & 1) esp = !esp;
		if (GetAsyncKeyState(VK_F2) & 1) nospread = !nospread;

		if (esp) {
			// mov eax, 1
			Memory::WPM<byte[6]>(handle, espAddr,{ 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3 }); 
		} else {
			// mov    al,BYTE PTR [ecx+0x11]
			Memory::WPM<byte[6]>(handle, espAddr, espAssembly);
		}
        if (nospread) {
            // xorps  xmm0,xmm0 
			Memory::WPM<byte[8]>(handle, noSpreadAddr,{  0x0F, 0x57, 0xC0, 0x90, 0x90, 0x90 ,0x90 ,0x90 }); 
		} else {
			// movss xmm0,[edi+00000484]
			Memory::WPM<byte[8]>(handle, noSpreadAddr,spreadAssembly); 
		}
		

		Sleep(5);
	}
return 0;
}
#include "hooks.h"
#include <Windows.h>

DWORD manaJmpBackAddy;

void __declspec(naked) manaOnFunc() {
	__asm {
		nop
		nop
		mov[esi + 0xBC], eax
		jmp[manaJmpBackAddy]
	}
}

void __declspec(naked) manaOffFunc() {
	__asm {
		sub eax,edx
		mov[esi + 0xBC], eax
		jmp[manaJmpBackAddy]
	}
}

DWORD oneHitJmpBackAddy;

void __declspec(naked) oneHitOnFunc() {
	__asm {
		mov eax,0x2711
		sub [edi+0x30],eax
		jmp[oneHitJmpBackAddy]
	}
}

void __declspec(naked) oneHitOffFunc() {
	__asm {
		mov eax,[ebp+0x10]
		sub[edi + 0x30],eax
		jmp[oneHitJmpBackAddy]
	}
}



bool Hook(void* toHook, void* ourFunct, int len) {
	if (len < 5) {
		return false;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}

void funcHook(const char* hookCall, DWORD glLoc) {
	if (hookCall == "manaOn"){
		DWORD manaHookAddr = glLoc + 0x525C5;

		int manaHookLen = 8;
		manaJmpBackAddy = manaHookAddr + manaHookLen;

		Hook((void*)manaHookAddr, manaOnFunc, manaHookLen);
	}
	if (hookCall == "oneHitOn") {
		DWORD oneHitAddr = glLoc + 0x20C2;

		int oneHitLen = 6;
		oneHitJmpBackAddy = oneHitAddr + oneHitLen;

		Hook((void*)oneHitAddr, oneHitOnFunc, oneHitLen);
	}
	if (hookCall == "manaOff") {
		DWORD manaHookAddr = glLoc + 0x525C5;

		int manaHookLen = 8;
		manaJmpBackAddy = manaHookAddr + manaHookLen;

		Hook((void*)manaHookAddr, manaOffFunc, manaHookLen);
	}
	if (hookCall == "oneHitOff") {
		DWORD oneHitAddr = glLoc + 0x20C2;

		int oneHitLen = 6;
		oneHitJmpBackAddy = oneHitAddr + oneHitLen;

		Hook((void*)oneHitAddr, oneHitOffFunc, oneHitLen);
	}
};
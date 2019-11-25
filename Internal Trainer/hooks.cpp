#include "hooks.h"
#include <Windows.h>

DWORD manaJmpBackAddy;

void __declspec(naked) manaFunc() {
	__asm {
		nop
		nop
		mov[esi + 0xBC], eax
		jmp[manaJmpBackAddy]
	}
};

DWORD magmaJmpBackAddy;

void __declspec(naked) magmaFunc() {
	__asm {
		mov edi,02
		mov ebx,ecx
		jmp[magmaJmpBackAddy]
	}
}

DWORD oneHitJmpBackAddy;

void __declspec(naked) oneHitFunc() {
	__asm {
		mov eax,0x2711
		sub [edi+0x30],eax
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
	if (hookCall == "mana"){
		DWORD manaHookAddr = glLoc + 0x525C5;

		int manaHookLen = 8;
		manaJmpBackAddy = manaHookAddr + manaHookLen;

		Hook((void*)manaHookAddr, manaFunc, manaHookLen);
	}
	if (hookCall == "magma") {
		DWORD magmaHookAddr = glLoc + 0x3CD48;

		int magmaHookLen = 5;
		magmaJmpBackAddy = magmaHookAddr + magmaHookLen;

		Hook((void*)magmaHookAddr, magmaFunc, magmaHookLen);
	}
	if (hookCall == "oneHit") {
		DWORD oneHitAddr = glLoc + 0x20C2;

		int oneHitLen = 6;
		oneHitJmpBackAddy = oneHitAddr + oneHitLen;

		Hook((void*)oneHitAddr, oneHitFunc, oneHitLen);
	}
};
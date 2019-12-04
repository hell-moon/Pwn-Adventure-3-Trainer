#include "Functions.h"

DWORD EntlistJmpBack = 0x0;
bool alreadyThere = false;

__declspec(naked) void entityhook()
{
	__asm
	{
		mov [edi + 0x30], 0x2
		jmp[EntlistJmpBack]
	}

}
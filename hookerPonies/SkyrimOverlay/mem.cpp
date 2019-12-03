#include "pch.h"
#include "mem.h"

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;	//this holds the previous permissions after we change them with VirtualProtectEx, so we can revert back, later
	// each page of memory has access permissions.  We want to set permissions to execute, read and write
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);		//ASIDE: anti-cheats can detect if certain permissions are tripped, and can detect writing to memory
	memcpy(dst, src, size);							//write to memory
	VirtualProtect(dst, size, oldprotect, &oldprotect);					//restore previous permissions
}

void mem::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;	//this holds the previous permissions after we change them with VirtualProtectEx, so we can revert back, later
	// each page of memory has access permissions.  We want to set permissions to execute, read and write
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);		//ASIDE: anti-cheats can detect if certain permissions are tripped, and can detect writing to memory
	memset(dst, 0x90, size);							//set the destination to nop instruction
	VirtualProtect(dst, size, oldprotect, &oldprotect);					//restore previous permissions
}

uintptr_t mem::FindDMAAddy( uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}
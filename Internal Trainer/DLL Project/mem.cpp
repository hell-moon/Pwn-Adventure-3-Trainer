#include "pch.h"
#include "mem.h"

/*

//External functions

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;	//this holds the previous permissions after we change them with VirtualProtectEx, so we can revert back, later
	// each page of memory has access permissions.  We want to set permissions to execute, read and write
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);		//ASIDE: anti-cheats can detect if certain permissions are tripped, and can detect writing to memory
	WriteProcessMemory(hProcess, dst, src, size, nullptr);							//write to memory
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);					//restore previous permissions
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];			//allocate new BYTE array on the heap, assign the address of it to the pointer, nopArray
	memset(nopArray, 0x90, size);				//set each byte in array to 0x90, the {no operation} instruction

	PatchEx(dst, nopArray, size, hProcess);		//patch the specified destination with the nopArray
	delete[] nopArray;							//deallocate the memory
}

uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}
*/

//Internal functions
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
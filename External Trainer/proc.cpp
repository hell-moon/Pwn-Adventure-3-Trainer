#include "stdafx.h"
#include "proc.h"

DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		// return handle to snapshot of all heaps, modules, threads in system
	if (hSnap != INVALID_HANDLE_VALUE)									
	{
		PROCESSENTRY32 procEntry;										// describes an entry from a list of processes in system address space when snapshot taken
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))							// TRUE if first entry of process list is copied to buffer, else FALSE
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))			// wide character string compare name of process to passed procName
				{
					procId = procEntry.th32ProcessID;					// if name matches, get process ID
					break;
				}
			} while (Process32Next(hSnap, &procEntry));					// if name doesn't match, load next process in the snapshot

		}
	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);	// return handle to snapshot of heaps, modules, threads used by process
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;											// describes an entry from a list of the modules belonging to process
		modEntry.dwSize = sizeof(modEntry);								// A process module represents a.dll or .exe file that is loaded into a particular process
		if (Module32First(hSnap, &modEntry))							// TRUE if first entry of module list is copied to buffer, else FALSE
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))				// wide character string compare name of module to passed modName
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;		// if name matches, get the base address of the module in the context of the owning process
					break;
				}
			} while (Module32Next(hSnap, &modEntry));					// if name doesn't match, load next module in the snapshot
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)		
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);		// read value at address (pointer)
		addr += offsets[i];													// add offset to address
	}																		// repeat for all offsets
	return addr;
}
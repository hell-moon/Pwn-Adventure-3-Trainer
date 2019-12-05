/*
Hacking Beavers Capstone Team
Tyler Ball
Xiao Kuang
Brock Neidert
This project builds a program that injects the specified library name into the specified process
In this case, inject the internal trainer .dll into the Pwn Adventure 3 process
*/

#include <windows.h> 
#include <tlhelp32.h> 
#include <stdio.h> 
#include <iostream>

using std::cout;
using std::endl;

BOOL Inject(DWORD pID, const char* DLL_NAME);
DWORD GetProcId(const wchar_t* ProcName);

const wchar_t* processName = L"PwnAdventure3-Win32-Shipping.exe";
const char* dllName = "Pwn Adventure 3 Internal Trainer.dll";

int main()
{
	// Retrieve process ID 
	DWORD pID = GetProcId(processName);

	// Get the dll's full path name 
	char buffer[MAX_PATH] = { 0 };
	GetFullPathNameA(dllName, MAX_PATH, buffer, NULL);
	printf(buffer);
	printf("\n");

	// Inject our main dll 
	if (!Inject(pID, buffer))
	{
		printf("DLL Not Loaded!");
		getchar();
	}
	//else {
	//	printf("DLL Loaded!");
	//}

	////getchar();
	return 0;
}

BOOL Inject(DWORD pID, const char* DLL_NAME)
{
	HANDLE Proc;
	//HMODULE hLib;
	char buffer[50] = { 0 };
	LPVOID RemoteString, loadLibraryAddr;

	if (!pID)
		return false;

	Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	if (!Proc)
	{
		sprintf_s(buffer, "OpenProcess() failed: %d", GetLastError());
		//MessageBox(NULL, buffer, "Loader", MB_OK); 
		printf(buffer);
		return false;
	}

	loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

	// Allocate space in the process for our DLL 
	RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(DLL_NAME), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// Write the string name of our DLL in the memory allocated 
	WriteProcessMemory(Proc, (LPVOID)RemoteString, DLL_NAME, strlen(DLL_NAME), NULL);

	// Load our DLL 
	CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibraryAddr, (LPVOID)RemoteString, NULL, NULL);

	CloseHandle(Proc);
	return true;
}

DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		// return handle to snapshot of all heaps, modules, threads in system
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;										// describes an entry from a list of processes in system address space when snapshot taken
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))							// TRUE if first entry of process list is copied to bufferfer, else FALSE
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


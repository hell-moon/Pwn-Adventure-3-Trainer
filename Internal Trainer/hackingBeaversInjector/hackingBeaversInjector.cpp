//// hackingBeaversInjector.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include <iostream>
//
//#include <string>
//#include <windows.h>
//using std::cout;
//using std::endl;
//
//#define MAXWAIT 10000
//
//bool insertDll(DWORD procID, std::string dll)
//{
//	//Find the address of the LoadLibrary api, luckily for us, it is loaded in the same address for every process
//	HMODULE hLocKernel32 = GetModuleHandle("Kernel32");
//	FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryA");
//
//	//Adjust token privileges to open system processes
//	HANDLE hToken;
//	TOKEN_PRIVILEGES tkp;
//	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
//	{
//		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
//		tkp.PrivilegeCount = 1;
//		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//		AdjustTokenPrivileges(hToken, 0, &tkp, sizeof(tkp), NULL, NULL);
//	}
//
//	//Open the process with all access
//	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
//
//	//Allocate memory to hold the path to the Dll File in the process's memory
//	dll += '\0';
//	LPVOID hRemoteMem = VirtualAllocEx(hProc, NULL, dll.size(), MEM_COMMIT, PAGE_READWRITE);
//
//	//Write the path to the Dll File in the location just created
//	DWORD numBytesWritten;
//	WriteProcessMemory(hProc, hRemoteMem, dll.c_str(), dll.size(), &numBytesWritten);
//
//	//Create a remote thread that starts begins at the LoadLibrary function and is passed our memory pointer
//	HANDLE hRemoteThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)hLocLoadLibrary, hRemoteMem, 0, NULL);
//
//	cout << hRemoteThread << endl;
//
//	//Wait for the thread to finish
//	bool res = false;
//	if (hRemoteThread)
//		res = (bool)WaitForSingleObject(hRemoteThread, MAXWAIT) != WAIT_TIMEOUT;
//
//	//Free the memory created on the other process
//	VirtualFreeEx(hProc, hRemoteMem, dll.size(), MEM_RELEASE);
//
//	//Release the handle to the other process
//	CloseHandle(hProc);
//
//	return res;
//}
//
//
//int main()
//{
//    std::cout << "Hello World!\n";
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started: 
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

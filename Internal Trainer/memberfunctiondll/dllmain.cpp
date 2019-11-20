// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "mem.h"
#include "proc.h"
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;


typedef void(__thiscall* _Chat)(void* playervftblptr, const char* text);
_Chat Chat;

typedef void*(__thiscall* _GetItemByName)(void* gamePtr, const char* name);
_GetItemByName GetItemByName;

typedef bool(__thiscall* _AddItem)(void* playervftblptr, void* IItemPtr, unsigned int count, bool allowPartial);
_AddItem AddItem;




DWORD WINAPI HackThread(HMODULE hModule)
{
	// Create Console, initialize stdin, stdout, stderr handles for new console
	// active console screen buffer, CONOUT$
	AllocConsole();
	FILE* fConsole;
	// reassign stdout to fConsole, with path handle CONOUT$
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/freopen-s-wfreopen-s?view=vs-2019
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle(L"GameLogic.dll");
	uintptr_t firstLevelPtrAddr = gameLogicAddr + 0x97D7C;			
	
	uintptr_t addItemFuncOff = 0x51BA0;
	uintptr_t getItemByNameFuncOff = 0x1DE20;
	uintptr_t chatFuncOff = 0x551A0;
	const char* sGreatBallsOfFire = "GreatBallsOfFire";
	const char* sROPChainGun = "ROPChainGun";
	const char* sGoldMaster = "GoldenMaster";
	const char* sRemoteExploit = "RemoteExploit";
	const char* sOPFireball = "CharStar";
	const char* sHeapSpray = "HeapSpray";

	
	// various offsets list
	vector<unsigned int> playerPointerAddressOffset = { 0x1c, 0x6c };
	vector<unsigned int> playerVftableAddr = { 0x1c, 0x6c, 0x0 };
	//*******************************************************************************************
	//*******************************************************************************************


	// player pointer
	void* pPlayer = (void*)mem::FindDMAAddy(firstLevelPtrAddr, playerPointerAddressOffset);
	void* pPlayervftable = (void*)mem::FindDMAAddy(firstLevelPtrAddr, playerVftableAddr);
	// game pointer
	void* pGame = (void*)(gameLogicAddr + 0x9780);
	//*******************************************************************************************
	//*******************************************************************************************



	//print for debugging
	cout << "gameLogic.dll: " << std::hex << gameLogicAddr << endl;
	cout << "player pointer: " << std::hex << mem::FindDMAAddy(firstLevelPtrAddr, playerPointerAddressOffset) << endl;
	cout << "player vftable: " << std::hex << mem::FindDMAAddy(firstLevelPtrAddr, playerVftableAddr) << endl;
	cout << "game pointer: " << std::hex << pGame << endl;
	//*******************************************************************************************
	//*******************************************************************************************




	// create instance of chat function
	Chat = (_Chat)(gameLogicAddr + chatFuncOff);
	// create instance of getItemByName function
	GetItemByName = (_GetItemByName)(gameLogicAddr + getItemByNameFuncOff);
	// create instance of addItem function
	AddItem = (_AddItem)(gameLogicAddr + addItemFuncOff);
	//*******************************************************************************************
	//*******************************************************************************************

	Sleep(500);
	Chat(pPlayervftable, "Hacking Beavers Internal Trainer Loaded");


	while (!GetAsyncKeyState(VK_DELETE))
	{
		if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		{
			//nada
		}
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			// hold pointer to item
			void* pGBF = GetItemByName(pGame, sGreatBallsOfFire);
			cout << sGreatBallsOfFire << " address: " << std::hex << pGBF << endl;
			AddItem(pPlayervftable, pGBF, 1, 0);

			void* pROPGun = GetItemByName(pGame, sROPChainGun);
			cout << sROPChainGun << " address: " << std::hex << pROPGun << endl;
			AddItem(pPlayervftable, pROPGun, 1, 0);

			void* pGoldMaster = GetItemByName(pGame, sGoldMaster);
			cout << sGoldMaster << " address: " << std::hex << pGoldMaster << endl;
			AddItem(pPlayervftable, pGoldMaster, 1, 0);

			void* pRemoteExploit = GetItemByName(pGame, sRemoteExploit);
			cout << sRemoteExploit << " address: " << std::hex << pRemoteExploit << endl;
			AddItem(pPlayervftable, pRemoteExploit, 1, 0);
			
			void* pOPFireball = GetItemByName(pGame, sOPFireball);
			cout << sOPFireball << " address: " << std::hex << pOPFireball << endl;
			AddItem(pPlayervftable, pOPFireball, 1, 0);
		}
		//if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		//{
		//	void* pROPGun = NULL;
		//	pROPGun = GetItemByName(pGame, ROPChainGun);
		//	cout << ROPChainGun << " address: " << std::hex << pROPGun << endl;
		//	AddItem(pPlayervftable, pROPGun, 1, 0);

		//}
		//if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		//{
		//	void* pGoldMaster = NULL;
		//	pGoldMaster = GetItemByName(pGame, GoldMaster);
		//	cout << GoldMaster << " address: " << std::hex << pGoldMaster << endl;
		//	AddItem(pPlayervftable, pGoldMaster, 1, 0);

		//}

	}

	// clean up and eject dll
	fclose(fConsole);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


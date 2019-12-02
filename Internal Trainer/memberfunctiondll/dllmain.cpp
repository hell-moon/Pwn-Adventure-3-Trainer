// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "mem.h"
#include "proc.h"
#include <vector>
#include <iostream>
#include "functioncalls.h"

using std::cout;
using std::endl;
using std::vector;


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
	// create instance of giveAll function
	GiveAll = (_GiveAll)(gameLogicAddr + giveAllFuncOff);
	// create instance of getPlayerInterface function
	GetPlayerInterface = (_GetPlayerInterface)(gameLogicAddr + 0x4FEB0);
	// create instance of fastTravel function
	FastTravel = (_FastTravel)(gameLogicAddr + fastTravelFuncOff);
	// create instance of teleport function
	Teleport = (_Teleport)(gameLogicAddr + teleportFuncOff);

	//*******************************************************************************************
	//*******************************************************************************************

	Sleep(500);
	Chat(pPlayervftable, "Hacking Beavers Internal Trainer Loaded2");


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

			void* pOPFireball = GetItemByName(pGame, sOPFireball);
			cout << sOPFireball << " address: " << std::hex << pOPFireball << endl;
			AddItem(pPlayervftable, pOPFireball, 1, 0);

			void* pROPGun = GetItemByName(pGame, sROPChainGun);
			cout << sROPChainGun << " address: " << std::hex << pROPGun << endl;
			AddItem(pPlayervftable, pROPGun, 1, 0);

			void* pGoldMaster = GetItemByName(pGame, sGoldMaster);
			cout << sGoldMaster << " address: " << std::hex << pGoldMaster << endl;
			AddItem(pPlayervftable, pGoldMaster, 1, 0);

			void* pRemoteExploit = GetItemByName(pGame, sRemoteExploit);
			cout << sRemoteExploit << " address: " << std::hex << pRemoteExploit << endl;
			AddItem(pPlayervftable, pRemoteExploit, 1, 0);

			void* pHeapSpray = GetItemByName(pGame, sHeapSpray);
			cout << sHeapSpray << " address: " << std::hex << pHeapSpray << endl;
			AddItem(pPlayervftable, pHeapSpray, 1, 0);

			void* pHandCannon = GetItemByName(pGame, sHandCannon);
			cout << sHandCannon << " address: " << std::hex << pHandCannon << endl;
			AddItem(pPlayervftable, pHandCannon, 1, 0);

			void* pPistolAmmo = GetItemByName(pGame, sPistolAmmo);
			cout << sPistolAmmo << " address: " << std::hex << pPistolAmmo << endl;
			AddItem(pPlayervftable, pPistolAmmo, 9999, 1);

			void* pShotgunAmmo = GetItemByName(pGame, sShotgunAmmo);
			cout << sShotgunAmmo << " address: " << std::hex << pShotgunAmmo << endl;
			AddItem(pPlayervftable, pShotgunAmmo, 9999, 1);

			void* pRifleAmmo = GetItemByName(pGame, sRifleAmmo);
			cout << sRifleAmmo << " address: " << std::hex << pRifleAmmo << endl;
			AddItem(pPlayervftable, pRifleAmmo, 9999, 1);

			void* pSniperAmmo = GetItemByName(pGame, sSniperAmmo);
			cout << sSniperAmmo << " address: " << std::hex << pSniperAmmo << endl;
			AddItem(pPlayervftable, pSniperAmmo, 9999, 1);

			void* pRevolverAmmo = GetItemByName(pGame, sRevolverAmmo);
			cout << sRevolverAmmo << " address: " << std::hex << pRevolverAmmo << endl;
			AddItem(pPlayervftable, pRevolverAmmo, 9999, 1);

			void* pPwnCoin = GetItemByName(pGame, sPwnCoin);
			cout << sPwnCoin << " address: " << std::hex << pPwnCoin << endl;
			AddItem(pPlayervftable, pPwnCoin, 9999, 1);

		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			Teleport(pPlayervftable, "TailMountains");
			cout << "GiveAll Activated" << endl;
			GiveAll(pGame, pPlayervftable);
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			cout << "GiveAll Activated" << endl;
			GiveAll(pGame, pPlayervftable);
		}

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

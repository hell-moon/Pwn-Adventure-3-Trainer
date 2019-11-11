// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include "mem.h"



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

	// make sure correct address for game logic module
	std::cout << std::hex <<gameLogicAddr << std::endl;

	uintptr_t firstLevelPtr, manaAddr, healthAddr, speedAddr, jumpAddr = 0;
	bool bMana = false, bHealth = false, bSpeed = false, bJump = false, bLoop = true;

	const int newMana = 1337;
	const int newHealth = 420;
	const int defaultMana = 100;
	const int defaultHealth = 100;
	const float newSpeed = 1000;
	const float defaultSpeed = 200;
	const float newJump = 2000;
	const float defaultJump = 420;


	std::vector<unsigned int> manaOffsets = { 0x1c, 0x6c, 0xbc };
	std::vector<unsigned int> healthOffsets = { 0x1c, 0x6c, 0x00 };  // need to subtract 0x40 to get health addr
	std::vector<unsigned int> speedOffsets = { 0x1c, 0x6c, 0x120 };
	std::vector<unsigned int> jumpOffsets = { 0x1c, 0x6c, 0x124 };

	firstLevelPtr = gameLogicAddr + 0x97D7C;
	manaAddr = mem::FindDMAAddy(firstLevelPtr, manaOffsets);
	healthAddr = mem::FindDMAAddy(firstLevelPtr, healthOffsets) - 0x40;
	speedAddr = mem::FindDMAAddy(firstLevelPtr, speedOffsets);
	jumpAddr = mem::FindDMAAddy(firstLevelPtr, jumpOffsets);

	// hack loop
	while (bLoop)
	{
		if (GetAsyncKeyState(VK_DELETE) & 1)								// listen for hotkey presses, set flags
		{
			bLoop = false;
		}
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)								// listen for hotkey presses, set flags
		{
			bMana = !bMana;
			if (bMana)
			{
				std::cout << "Mana <ON>\n";
			}
			else
			{
				std::cout << "Mana <OFF>\n";
				*(int*)manaAddr = defaultMana;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bHealth = !bHealth;
			if (bHealth)
			{
				std::cout << "Health <ON>\n";
			}
			else
			{
				std::cout << "Health <OFF>\n";
				*(int*)healthAddr = defaultHealth;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bSpeed = !bSpeed;
			if (bSpeed)
			{
				std::cout << "Speed <ON>\n";
				*(float*)speedAddr = newSpeed;
			}
			else
			{
				std::cout << "Speed <OFF>\n";
				*(float*)speedAddr = defaultSpeed;
			}

		}
		
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			bJump = !bJump;
			if (bJump)
			{
				std::cout << "Jump <ON>\n";
				*(float*)jumpAddr = newJump;
			}
			else
			{
				std::cout << "Jump <OFF>\n";
				*(float*)jumpAddr = defaultSpeed;
			}
		}

		if (manaAddr)
		{
			if (bMana)
			{
				*(int*)manaAddr = newMana;
			}
			if (bHealth)
			{
				*(int*)healthAddr = newHealth;
			}
		}
	}
	
	// clean up and eject dll
	fclose(fConsole);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
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


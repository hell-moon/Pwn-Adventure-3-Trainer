// PwnAdventure3Ex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"
#include <thread>
#include "menuDialogue.h"

#include <cstdlib>
#include <string>
#include <ctime>
#include <numeric>
#include <cmath>
#include <sstream>
#include <chrono>
#include <ctime>
#include <mutex>



//void cheatMana(HANDLE);
void cheatHealth(HANDLE, uintptr_t, int);


int main()
{
	// retrieve the process
	HANDLE hProcess = 0;

	uintptr_t moduleBase, firstLevelPtr, manaAddr, healthAddr, speedAddr, jumpAddr = 0;
	bool bMana = false, bHealth = false, bSpeed = false, bJump = false;

	const int newMana = 1337;
	const int newHealth = 420;
	const int defaultMana = 100;
	const int defaultHealth = 100;
	const float newSpeed = 1000;
	const float defaultSpeed = 200;
	const float newJump = 2000;
	const float defaultJump = 420;

	DWORD procId = GetProcId(L"PwnAdventure3-Win32-Shipping.exe");

	if (procId)
	{
		//get the base address of process PwnAdventure3-Win32-Shipping.exe
		moduleBase = GetModuleBaseAddress(procId, L"GameLogic.dll");

		//add relative offset to get the first level pointer
		firstLevelPtr = moduleBase + 0x00097D7C;

		//need to use PROCESS_ALL_ACCESS for GetExitCodeProcess
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		// determine dynamic addresses based off of static pointers
		manaAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0xbc });
		healthAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x00 }) - 0x40;
		speedAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x120 });
		jumpAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x124 });

	}
	else
	{
		//if pwn adventure process is not found, exit
		std::cout << "[1] Process not found, press enter to exit\n";
		return 0;
	}


	// welcome the user
	welcome();

	// prompt the user
	int userSelection;

	do
	{
		// if user selects mana
		userSelection = mainMenuOptions();
		system("CLS");

		switch (userSelection)
		{
		// toggle mana
		case 1:
			bMana = !bMana;
			if (bMana)
			{
				std::cout << "Mana <ON>\n";
			}
			else
			{
				std::cout << "Mana <OFF>\n";
				mem::PatchEx((BYTE*)manaAddr, (BYTE*)&defaultMana, sizeof(defaultMana), hProcess);
			}
			break;
		// toggle health
		case 2:
			bHealth = !bHealth;
			if (bHealth)
			{
				std::thread superHealthy(cheatHealth, hProcess, healthAddr, newHealth);
				superHealthy.join();
				std::cout << "Health <ON>\n";
			}
			else
			{
				std::cout << "Health <OFF>\n";
				//mem::PatchEx((BYTE*)healthAddr, (BYTE*)&defaultHealth, sizeof(defaultHealth), hProcess);
			}
			break;
		case 3:
			bSpeed = !bSpeed;
			if (bSpeed)
			{
				std::cout << "Speed <ON>\n";
				mem::PatchEx((BYTE*)speedAddr, (BYTE*)&newSpeed, sizeof(newSpeed), hProcess);
			}
			else
			{
				std::cout << "Speed <OFF>\n";
				mem::PatchEx((BYTE*)speedAddr, (BYTE*)&defaultSpeed, sizeof(newSpeed), hProcess);
			}
			break;
		case 4:
			bJump = !bJump;
			if (bJump)
			{
				std::cout << "Jump <ON>\n";
				mem::PatchEx((BYTE*)jumpAddr, (BYTE*)&newJump, sizeof(newJump), hProcess);
			}
			else
			{
				std::cout << "Jump <OFF>\n";
				mem::PatchEx((BYTE*)jumpAddr, (BYTE*)&defaultJump, sizeof(newJump), hProcess);
			}
			break;
		default:
			break;
		}

	} while (userSelection != 0);

	return 0;
}


void cheatHealth(HANDLE hProcess, uintptr_t healthAddr, int newHealth)
{
	
	//if process has not terminated and function succeeds, status returned is STILL_ACTIVE
	int test = 4;
	while (test == 4) 
	{
		// flag is set, write to health address every 10 ms
		mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newHealth, sizeof(newHealth), hProcess);
		
		Sleep(500);
	}
}

// PwnAdventure3Ex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"
#include <omp.h>


int main()
{

#ifndef _OPENMP
	fprintf(stderr, "OpenMP is not available\n");
	return 1;
#endif

	HANDLE hProcess = 0;

	uintptr_t moduleBase, firstLevelPtr, manaAddr, healthAddr, speedAddr, jumpAddr, fireAddr = 0;
	bool bMana = false, bHealth = false, bSpeed = false, bJump = false, bFire = false, activeTrainer = false;

	const int newMana = 1337;
	const int newHealth = 420;
	const int defaultMana = 100;
	const int defaultHealth = 100;
	const float newSpeed = 1000;
	const float defaultSpeed = 200;
	const float newJump = 2000;
	const float defaultJump = 420;
	char newFire[] = { 0xB8, 0xDC, 0x05, 0x00, 0x00 };
	char defaultFire[] = { 0xB8, 0x19, 0x00, 0x00, 0x00 };

	DWORD procId = GetProcId(L"PwnAdventure3-Win32-Shipping.exe");

	if (procId)
	{
		// get the base address of process PwnAdventure3-Win32-Shipping.exe
		moduleBase = GetModuleBaseAddress(procId, L"GameLogic.dll");                            

		// add relative offset to get the first level pointer
		firstLevelPtr = moduleBase + 0x00097D7C;  
		fireAddr = moduleBase + 0x0000D7E0;

		// need to use PROCESS_ALL_ACCESS for GetExitCodeProcess
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);                                

		// determine dynamic addresses based off of static pointers
		manaAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0xbc });                    
		healthAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x00 }) - 0x40;
		speedAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x120 });
		jumpAddr = FindDMAAddy(hProcess, firstLevelPtr, { 0x1c, 0x6c, 0x124 });

		activeTrainer = true;
	}
	else
	{
		// if pwn adventure process is not found, exit
		std::cout << "[1] Process not found, press enter to exit\n";								
		getchar();
		return 0;
	}

	
	omp_set_num_threads(2);

	// divide the code into sections
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			while (activeTrainer == true)
			{
				// listen for hotkey presses, set flags
				if (GetAsyncKeyState(VK_NUMPAD1) & 1)
				{
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
						mem::PatchEx((BYTE*)healthAddr, (BYTE*)&defaultHealth, sizeof(defaultHealth), hProcess);
					}
				}
				// continuous write or freeze
				if (bMana)
				{
					// flag is set, write to mana address every 10 ms
					mem::PatchEx((BYTE*)manaAddr, (BYTE*)&newMana, sizeof(newMana), hProcess);
				}
				if (bHealth)
				{
					// flag is set, write to health address every 10 ms
					mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newHealth, sizeof(newHealth), hProcess);
				}

				Sleep(20);
			}
		}

		#pragma omp section
		{
			while (activeTrainer == true)
			{
				if (GetAsyncKeyState(VK_NUMPAD3) & 1)
				{
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

				}
				if (GetAsyncKeyState(VK_NUMPAD4) & 1)
				{
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
				}
				if (GetAsyncKeyState(VK_NUMPAD6) & 1)
				{
					bFire = !bFire;
					if (bFire)
					{
						std::cout << "Fireball <ON>\n";
						mem::PatchEx((BYTE*)fireAddr, (BYTE*)&newFire, sizeof(newFire), hProcess);
					}
					else
					{
						std::cout << "Fireball <OFF>\n";
						mem::PatchEx((BYTE*)fireAddr, (BYTE*)&defaultFire, sizeof(newFire), hProcess);
					}
				}
				Sleep(20);
			}
		}
	}
	
	std::cout << "[2] Process not found, press enter to exit\n";
	getchar();

	return 0;

}

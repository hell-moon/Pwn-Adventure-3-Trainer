#include "DirectX.h"
#include "framework.h"
#include "functioncalls.h"
#include "hooks.h"
#include "modulemgr.h"
#include "main.h"
#include "pch.h"
#include "mem.h"
#include "proc.h"
#include <Psapi.h>
#include <vector>
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

using std::cout;
using std::endl;
using std::vector;

// initiate variables for the menu
directx_t DirectX;
ID3DXFont* pFont;
LPD3DXFONT font_interface = NULL;
IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;
RECT rc;

//bool bInitialized = false;

// determine the state of keys when the trainer is launched
bool bKeys[255];
bool bKeyPrev[255];

// is the menu hidden or visible?
bool menuVisible = true;

// how many cheats does out trainer have?
#define NUMCHEATS 6
// create an array that has a int flag to denote the state of each cheat
int currentCheatSetting[NUMCHEATS];
// which cheat has the player selected?
int highlightedCheat = 0;
// create an array of wide strings to serve as a list in the menu
wchar_t cheatMenuEntries[NUMCHEATS][255] =
{
	L"Mana      ",
	L"Health    ",
	L"Speed     ",
	L"Jump      ",
	L"Guns      ",
	//L"Teleport  ",
	L"God Mode  "
};
// create an array that stores the number of settings for each cheat
int numCheatSettings[NUMCHEATS] = { 2,2,3,3,2,2 };

// has the legendary weapons cheat been used?
int legendaryMode = 0;
// is the god mode cheat on?
int godFlag = 0;
// store the currently committed setting in the cheat options
int lastCommittedSetting = 0;
// are there uncommitted changes in the menu?
int uncommittedChanges = 0;

// create an array of the cheat options
wchar_t ourCheats[NUMCHEATS][3][255] =
{
	{ L"                      Default", L"                       Infinite" },
	{ L"                      Default", L"                   Invincible", },
	{ L"                     Default", L"                       Faster", L"                     Fastest" },
	{ L"                      Default", L"                       Higher", L"                     Highest" },
	{ L"                            Off", L"                 Legendary" },
	{ L"                         Off", L"                         On" }
};

// initialize DirectX settings
void DirectxFunctions::DirectXInit(HWND hwnd)
{
	// did the Direct3DCreate9 object get created?
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX.Object)))
		exit(1);

	// our cheat menu will run in a small window
	ZeroMemory(&DirectX.Param, sizeof(DirectX.Param));
	DirectX.Param.Windowed = true;
	DirectX.Param.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX.Param.BackBufferHeight = Overlay.Height;
	DirectX.Param.BackBufferWidth = Overlay.Width;
	DirectX.Param.EnableAutoDepthStencil = true;
	DirectX.Param.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX.Param.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX.Param.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// were the objects created?
	if (FAILED(DirectX.Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX.Param, 0, &DirectX.Device)))
		exit(1);

	// set up text settings
	D3DXCreateFont(DirectX.Device, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &DirectX.Font);
	D3DXCreateFont(DirectX.Device, 13, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &DirectX.espFont);
	if (!DirectX.Line)
		D3DXCreateLine(DirectX.Device, &DirectX.Line);
}


void DirectxFunctions::RenderDirectX()
{
	// get address of gamelogic.dll
	uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
	// from gamelogic.dll, use offset to get address of player structure
	uintptr_t firstLevelPtr = gameLogicAddr + 0x97D7C;
	DirectX.Device->BeginScene();
	
	// make sure any pressed keys at time of launch don't interfere with the menu
	if (GetForegroundWindow() == Target.Window)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GetAsyncKeyState(i) != 0)
			{
				if (bKeyPrev[i] == false)
				{
					bKeyPrev[i] = true;
					bKeys[i] = true;
				}
				else
				{
					bKeys[i] = false;
				}
			}
			else
			{
				bKeys[i] = false;
				bKeyPrev[i] = false;
			}
		}

		// pressing subtract toggles menu visibility 
		if (bKeys[VK_SUBTRACT])
		{
			menuVisible = !menuVisible;
		}

		// if the menu is visible, draw it
		if (menuVisible)
		{
			// position the menu
			RECT pos;
			pos.right = 1001;
			pos.bottom = 1001;
			pos.left = 20;
			pos.top = 20;


			// background, xpos, ypos, width, height, color
			Drawing::FilledRect(18, 20, 250, 161, D3DCOLOR_ARGB(255, 5, 5, 5));
			// border, xpos, ypos, width, height, color
			Drawing::BorderedRect(17, 19, 250, 161, 1, 1, 1, 1, D3DCOLOR_ARGB(255, 255, 125, 000));
			// header rectangle, xpos, ypos, width, height, color
			Drawing::FilledRect(17, 19, 250, 19, D3DCOLOR_ARGB(255, 255, 125, 000));
			// header text
			DirectX.Font->DrawTextW(NULL, L"Beaver Trainer v1.1", -1, &pos, 0, D3DCOLOR_ARGB(255, 5, 5, 5));
			pos.top += 20;
			// footer rectangle, xpos, ypos, width, height
			Drawing::FilledRect(17, 161, 250, 19, D3DCOLOR_ARGB(255, 255, 125, 000));

			wchar_t cheatName[255];
			wchar_t cheatSetting[255];

			// fill the menu with text
			for (int i = 0; i < NUMCHEATS; i++)
			{
				D3DCOLOR color;
				if (highlightedCheat == i)
					color = D3DCOLOR_ARGB(255, 255, 125, 000);
				else
					color = D3DCOLOR_ARGB(225, 255, 250, 250);
				swprintf(cheatName, cheatMenuEntries[i]);

				// update the char array with the current setting
				swprintf(cheatSetting, ourCheats[i][currentCheatSetting[i]]);				

				// concatenate print it to the menu
				wcscat(cheatName, cheatSetting);
				DirectX.Font->DrawTextW(NULL, cheatName, -1, &pos, 0, color);
				pos.top += 17;
			}
			pos.top += 19.5;
			// footer text
			DirectX.Font->DrawTextW(NULL, L"Press NUMPAD- to hide menu", -1, &pos, 0, D3DCOLOR_ARGB(255, 5, 5, 5));

			// move up
			if (bKeys[VK_NUMPAD8])
			{
				// overwrite any uncommitted change
				if (uncommittedChanges == 1)
				{
					currentCheatSetting[highlightedCheat] = lastCommittedSetting;
					uncommittedChanges = 0;
				}
				// move highlight
				if (highlightedCheat > 0)
				{
					highlightedCheat--;
				}
			}
			// move down
			if (bKeys[VK_NUMPAD2])
			{
				// overwrite any uncommitted change
				if (uncommittedChanges == 1)
				{
					currentCheatSetting[highlightedCheat] = lastCommittedSetting;
					uncommittedChanges = 0;
				}
				// move highlight
				if (highlightedCheat < NUMCHEATS - 1)
				{
					highlightedCheat++;
				}
			}
			// get previous setting
			if (bKeys[VK_NUMPAD4])
			{
				// store the last committed setting
				if (uncommittedChanges == 0)
				{
					lastCommittedSetting = currentCheatSetting[highlightedCheat];
					uncommittedChanges = 1;
				}
				// peak left
				if (currentCheatSetting[highlightedCheat] > 0)
					currentCheatSetting[highlightedCheat]--;
	
			}
			// get next setting
			if (bKeys[VK_NUMPAD6])
			{
				// store a the last committed setting
				if (uncommittedChanges == 0)
				{
					lastCommittedSetting = currentCheatSetting[highlightedCheat];
					uncommittedChanges = 1;
				}
				// peak right
				if ( currentCheatSetting[highlightedCheat] < numCheatSettings[highlightedCheat] - 1)
					currentCheatSetting[highlightedCheat]++;

			}
			// commit selected change
			if (bKeys[VK_NUMPAD0])
			{
				// commit any changes
				lastCommittedSetting = currentCheatSetting[highlightedCheat];
				uncommittedChanges = 0;

				// Mana Boost
				// Hacking Method:  MemPatch / Direct memory write to function that controls player mana
				if (highlightedCheat == 0)
				{
					// don't alter health if god mode is on
					if (godFlag == 0)
					{
						if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
							currentCheatSetting[highlightedCheat] = 0;

						// no-op the subtract command if the cheat is on, otherwise use default settings
						if (currentCheatSetting[highlightedCheat] > 0)
						{
							mem::Patch((BYTE*)(gameLogicAddr + 0x525C5), (BYTE*)"\x90\x090", 2);
						}
						else
						{
							mem::Patch((BYTE*)(gameLogicAddr + 0x525C5), (BYTE*)"\x2B\xC2", 2);
						}
					}
					else
					{
						currentCheatSetting[highlightedCheat] = 0;
					}
				}

				// Health Boost Cheat
				// Hacking Method:  MemPatch / Direct memory write to function that controls player health
				if (highlightedCheat == 1)
				{
					if (godFlag == 0)
					{
						if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
							currentCheatSetting[highlightedCheat] = 0;

						//if (bMenuItems[iSelectedItem] == true)
						if (currentCheatSetting[highlightedCheat] > 0)
						{
							// overwrites assembly instruction to jump if equal, bypassing health subtract
							mem::Patch((BYTE*)(gameLogicAddr + 0x51176), (BYTE*)"\x0F\x84\x9C\x00\x00\x00", 6);
						}
						else
						{
							// rewrites assembly instruction to jump if not equal, resulting in health subtract
							mem::Patch((BYTE*)(gameLogicAddr + 0x51176), (BYTE*)"\x0F\x85\x9C\x00\x00\x00", 6);
						}
					}
					else
					{
						currentCheatSetting[highlightedCheat] = 0;
					}
				}

				// Speed Boost
				if (highlightedCheat == 2)
				{
					if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
						currentCheatSetting[highlightedCheat] = 0;

					// initialize speed values
					uintptr_t speedAddr;
					const float oneSpeed = 1000;
					const float twoSpeed = 3000;
					const float defaultSpeed = 200;

					// locate the address containing speed
					std::vector<unsigned int> speedOffsets = { 0x1c, 0x6c, 0x120 };
					firstLevelPtr = gameLogicAddr + 0x97D7C;
					speedAddr = mem::FindDMAAddy(firstLevelPtr, speedOffsets);

					// alter the value depending on the user's selection
					if (currentCheatSetting[highlightedCheat] == 2)
					{
						*(float*)speedAddr = twoSpeed;
					}
					else if (currentCheatSetting[highlightedCheat] == 1)
					{
						*(float*)speedAddr = oneSpeed;
					}
					else
					{
						*(float*)speedAddr = defaultSpeed;
					}
				}

				// Jump Boost
				if (highlightedCheat == 3)
				{
					if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
						currentCheatSetting[highlightedCheat] = 0;

					// initialize jump values
					uintptr_t jumpAddr;
					const float oneJump = 1800;
					const float twoJump = 5000;
					const float defaultJump = 420;

					// locate address containing jump
					std::vector<unsigned int> jumpOffsets = { 0x1c, 0x6c, 0x124 };
					firstLevelPtr = gameLogicAddr + 0x97D7C;
					jumpAddr = mem::FindDMAAddy(firstLevelPtr, jumpOffsets);


					// alter the value depending on the user's selection
					if (currentCheatSetting[highlightedCheat] == 2)
					{
						*(float*)jumpAddr = twoJump;
					}
					else if (currentCheatSetting[highlightedCheat] == 1)
					{
						*(float*)jumpAddr = oneJump;
					}
					else
					{
						*(float*)jumpAddr = defaultJump;
					}
				}

				// Give Legendary Items
				if (highlightedCheat == 4)
				{
					// only run this if the user doesn't already have the weapons
					if (legendaryMode == 0 && currentCheatSetting[highlightedCheat] == 1)
					{
						gameFunc("getWeapons");
						legendaryMode = 1;
					}
					else if (legendaryMode == 0)
					{
						currentCheatSetting[highlightedCheat] = 0;
					}
					else
					{
						currentCheatSetting[highlightedCheat] = 1;
					}

				}

				// Teleport
				//if (highlightedCheat == 5)
				//{
				//	if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
				//		currentCheatSetting[highlightedCheat] = 0;

				//	if (currentCheatSetting[highlightedCheat] == 0)
				//		//teleport to town
				//		gameFunc("toTown");
				//	
				//	else if (currentCheatSetting[highlightedCheat] == 1)
				//		//teleport to tails mountain
				//		gameFunc("toTail");

				//	else if (currentCheatSetting[highlightedCheat] == 2)
				//		//teleport to pirate bay
				//		gameFunc("toPirate");
				//	
				//	else if (currentCheatSetting[highlightedCheat] == 3)
				//		//teleport to gold farm
				//		gameFunc("toGold");
				//	
				//	else if (currentCheatSetting[highlightedCheat] == 4)
				//		//teleport to ballmer peak
				//		gameFunc("toBallmer");
				//	
				//	else if (currentCheatSetting[highlightedCheat] == 5)
				//		//teleport to unbearable woods
				//		gameFunc("toUnbearable");
				//	
				//	else if (currentCheatSetting[highlightedCheat] == 6)
				//		//teleport to sewer
				//		gameFunc("toSewer");
				//	
				//	else if (currentCheatSetting[highlightedCheat] == 7)
				//		//teleport to lost cave
				//		gameFunc("toLost");
				//}
				// God Mode
				if (highlightedCheat == 5)
				{
					if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
						currentCheatSetting[highlightedCheat] = 0;

					if (currentCheatSetting[highlightedCheat] > 0)
					{
						godFlag = 1;
						// turn off heatlth and mana cheat
						currentCheatSetting[0] = 0;
						currentCheatSetting[1] = 0;
						//unlimited mana function hook
						funcHook("manaOn");
						//damage increase function hook
						funcHook("oneHitOn");
						//health function patch to bypass damage taken by player
						mem::Patch((BYTE*)(gameLogicAddr + 0x51176), (BYTE*)"\x0F\x84\x9C\x00\x00\x00", 6);

					}
					else
					{
						godFlag = 0;
						//restore original game code for mana
						funcHook("manaOff");
						//restore original game code for player damage output
						funcHook("oneHitOff");
						//restore original game code to allow player to take damage
						mem::Patch((BYTE*)(gameLogicAddr + 0x51176), (BYTE*)"\x0F\x85\x9C\x00\x00\x00", 6);

					}
				}
			}
		}
	}
	DirectX.Device->EndScene();
	DirectX.Device->PresentEx(0, 0, 0, 0, 0);
	DirectX.Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
}


overlay_t Overlay;
target_t Target;

LRESULT CALLBACK WinProcedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		DirectxFunctions::RenderDirectX();
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		break;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}


DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	ProcessFunctions::CheckDWM();
	OverlayFunctions::GetTargetWindow();
	OverlayFunctions::CreateClass(WinProcedure, "overlay");
	OverlayFunctions::CreateWindowOverlay();
	DirectxFunctions::DirectXInit(Overlay.Window);

	for (;;)
	{
		if (PeekMessage(&Overlay.Message, Overlay.Window, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Overlay.Message);
			TranslateMessage(&Overlay.Message);
		}
		Sleep(1);
		OverlayFunctions::GetTargetWindow();
	}
	return 0;
}

// built in windows function, this program will create a DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		hInstance = hModule;
		CreateThread(0, NULL, ThreadProc, (LPVOID)L"X", NULL, NULL);
	}
	return TRUE;
}



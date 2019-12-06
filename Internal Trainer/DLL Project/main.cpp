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

directx_t DirectX;
ID3DXFont* pFont;
LPD3DXFONT font_interface = NULL;
IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;
RECT rc;

bool bInitialized = false;

bool bKeys[255];
bool bKeyPrev[255];

bool menuVisible = true;

// how many cheats does out trainer have?
#define NUMCHEATS 7
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
	L"Teleport  ",
	L"God Mode  "
};
// create an array that stores the number of settings for each cheat
int numCheatSettings[7] = { 2,2,3,3,2,8,2 };

int legendaryMode = 0;
int godFlag = 0;
int lastCommittedSetting = 0;
int uncommittedChanges = 0;

wchar_t manaOptions[2][255] =   { L"                      Default", 
								  L"                       Infinite" };
wchar_t healthOptions[2][255] = { L"                      Default", 
                                  L"                   Invincible" };
wchar_t speedOptions[3][255] =  { L"                     Default", 
								  L"                        Faster", 
								  L"                     Fastest" };
wchar_t jumpOptions[3][255] =   { L"                      Default",	
								  L"                       Higher", 
							      L"                     Highest" };
wchar_t gunOptions[2][255] =    { L"                            Off",	
								  L"                 Legendary" };
wchar_t teleOptions[8][255] =   { L"                         Town", 
								  L"           Tail Mountains", 
								  L"                 Pirate Bay", 
								  L"                 Gold Farm", 
								  L"             Ballmer Peak", 
				                  L"    Unbearable Woods", 
								  L"                       Sewer", 
								  L"                  Lost Cave" };
wchar_t godOptions[2][255] =    { L"                         Off", 
								  L"                         On" };


void DirectxFunctions::DirectXInit(HWND hwnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX.Object)))
		exit(1);

	ZeroMemory(&DirectX.Param, sizeof(DirectX.Param));
	DirectX.Param.Windowed = true;
	DirectX.Param.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX.Param.BackBufferHeight = Overlay.Height;
	DirectX.Param.BackBufferWidth = Overlay.Width;
	DirectX.Param.EnableAutoDepthStencil = true;
	DirectX.Param.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX.Param.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX.Param.SwapEffect = D3DSWAPEFFECT_DISCARD;


	if (FAILED(DirectX.Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX.Param, 0, &DirectX.Device)))
		exit(1);


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
		if (bKeys[VK_SUBTRACT])
		{
			menuVisible = !menuVisible;
		}

		if (menuVisible)
		{
			RECT pos;
			pos.right = 1001;
			pos.bottom = 1001;

			pos.left = 20;
			pos.top = 20;


			// background, xpos, ypos, width, height
			Drawing::FilledRect(18, 20, 250, 161, D3DCOLOR_ARGB(255, 5, 5, 5));
			// border, xpos, ypos, width, height 
			Drawing::BorderedRect(17, 19, 250, 161, 1, 1, 1, 1, D3DCOLOR_ARGB(255, 255, 125, 000));
			// header rectangle, xpos, ypos, width, height
			Drawing::FilledRect(17, 19, 250, 19, D3DCOLOR_ARGB(255, 255, 125, 000));
			// header text
			DirectX.Font->DrawTextW(NULL, L"Beaver Trainer v1.1", -1, &pos, 0, D3DCOLOR_ARGB(255, 5, 5, 5));
			pos.top += 20;
			// footer rectangle, xpos, ypos, width, height
			Drawing::FilledRect(17, 161, 250, 19, D3DCOLOR_ARGB(255, 255, 125, 000));

			wchar_t swf[255];
			wchar_t swf2[255];

			for (int i = 0; i < NUMCHEATS; i++)
			{
				D3DCOLOR color;
				if (highlightedCheat == i)
					color = D3DCOLOR_ARGB(255, 255, 125, 000);
				else
					color = D3DCOLOR_ARGB(225, 255, 250, 250);
				swprintf(swf, cheatMenuEntries[i]);

				if (i == 0)
					swprintf(swf2, manaOptions[currentCheatSetting[i]]);
				else if (i == 1)
					swprintf(swf2, healthOptions[currentCheatSetting[i]]);
				else if (i == 2)
					swprintf(swf2, speedOptions[currentCheatSetting[i]]);
				else if (i == 3)
					swprintf(swf2, jumpOptions[currentCheatSetting[i]]);
				else if (i == 4)
					swprintf(swf2, gunOptions[currentCheatSetting[i]]);
				else if (i == 5)
					swprintf(swf2, teleOptions[currentCheatSetting[i]]);
				else if (i == 6)
					swprintf(swf2, godOptions[currentCheatSetting[i]]);

				wcscat(swf, swf2);
				DirectX.Font->DrawTextW(NULL, swf, -1, &pos, 0, color);

				pos.top += 17;
			}
			pos.top += 2.5;
			// footer text
			DirectX.Font->DrawTextW(NULL, L"Press END to hide menu", -1, &pos, 0, D3DCOLOR_ARGB(255, 5, 5, 5));

			if (bKeys[VK_NUMPAD8])
			{
				// overwrite any uncommitted change
				if (uncommittedChanges == 1)
				{
					currentCheatSetting[highlightedCheat] = lastCommittedSetting;
					uncommittedChanges = 0;
				}
				if (highlightedCheat > 0)
				{
					highlightedCheat--;
				}
			}
			if (bKeys[VK_NUMPAD2])
			{
				// overwrite any uncommitted change
				if (uncommittedChanges == 1)
				{
					currentCheatSetting[highlightedCheat] = lastCommittedSetting;
					uncommittedChanges = 0;
				}
				if (highlightedCheat < NUMCHEATS - 1)
				{
					highlightedCheat++;
				}
			}
			if (bKeys[VK_NUMPAD4])
			{
				if (uncommittedChanges == 0)
				{
					lastCommittedSetting = currentCheatSetting[highlightedCheat];
					uncommittedChanges = 1;
				}
				// peak left
				if (currentCheatSetting[highlightedCheat] > 0)
					currentCheatSetting[highlightedCheat]--;
	
			}
			if (bKeys[VK_NUMPAD6])
			{
				// store a the last committed change if this is the first time the user tries to browse optons
				if (uncommittedChanges == 0)
				{
					lastCommittedSetting = currentCheatSetting[highlightedCheat];
					uncommittedChanges = 1;
				}
				// check to see that temp is different
				// peak right
				if ( currentCheatSetting[highlightedCheat] < numCheatSettings[highlightedCheat] - 1)
					currentCheatSetting[highlightedCheat]++;

			}


			if (bKeys[VK_NUMPAD0])
			{
				// commit any changes
				lastCommittedSetting = currentCheatSetting[highlightedCheat];
				uncommittedChanges = 0;

				// Mana Boost
				// Hacking Method:  MemPatch / Direct memory write to function that controls player mana
				if (highlightedCheat == 0)
				{
					if (godFlag == 0)
					{
						if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
							currentCheatSetting[highlightedCheat] = 0;

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

					uintptr_t speedAddr;
					const float oneSpeed = 1000;
					const float twoSpeed = 3000;
					const float defaultSpeed = 200;
					std::vector<unsigned int> speedOffsets = { 0x1c, 0x6c, 0x120 };
					firstLevelPtr = gameLogicAddr + 0x97D7C;
					speedAddr = mem::FindDMAAddy(firstLevelPtr, speedOffsets);

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

					uintptr_t jumpAddr;
					const float oneJump = 1800;
					const float twoJump = 5000;
					const float defaultJump = 420;
					std::vector<unsigned int> jumpOffsets = { 0x1c, 0x6c, 0x124 };
					firstLevelPtr = gameLogicAddr + 0x97D7C;
					jumpAddr = mem::FindDMAAddy(firstLevelPtr, jumpOffsets);

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
				if (highlightedCheat == 5)
				{
					if (currentCheatSetting[highlightedCheat] == numCheatSettings[highlightedCheat])
						currentCheatSetting[highlightedCheat] = 0;

					if (currentCheatSetting[highlightedCheat] == 0)
						//teleport to town
						gameFunc("toTown");
					
					else if (currentCheatSetting[highlightedCheat] == 1)
						//teleport to tails mountain
						gameFunc("toTail");

					else if (currentCheatSetting[highlightedCheat] == 2)
						//teleport to pirate bay
						gameFunc("toPirate");
					
					else if (currentCheatSetting[highlightedCheat] == 3)
						//teleport to gold farm
						gameFunc("toGold");
					
					else if (currentCheatSetting[highlightedCheat] == 4)
						//teleport to ballmer peak
						gameFunc("toBallmer");
					
					else if (currentCheatSetting[highlightedCheat] == 5)
						//teleport to unbearable woods
						gameFunc("toUnbearable");
					
					else if (currentCheatSetting[highlightedCheat] == 6)
						//teleport to sewer
						gameFunc("toSewer");
					
					else if (currentCheatSetting[highlightedCheat] == 7)
						//teleport to lost cave
						gameFunc("toLost");
				}
				// God Mode
				if (highlightedCheat == 6)
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


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		hInstance = hModule;
		CreateThread(0, NULL, ThreadProc, (LPVOID)L"X", NULL, NULL);
	}
	return TRUE;
}



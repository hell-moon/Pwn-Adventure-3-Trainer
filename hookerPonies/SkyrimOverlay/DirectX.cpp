#include "DirectX.h"
#include "framework.h"
#include "functioncalls.h"
#include "modulemgr.h"
#include "main.h"
#include "Hooks_reclass.h"
#include "pch.h"
#include "mem.h"
#include "proc.h"
#include <Psapi.h>
#include <vector>
#include <iostream>





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
#define NUMCHEATS 6
// create an array that has a int flag to denote the state of each cheat
int currentCheatSetting[NUMCHEATS];
// which cheat has the player selected?
int highlightedCheat = 0;
// create an array of wide strings to serve as a list in the menu
wchar_t cheatMenuEntries[NUMCHEATS][255] =
{
	L"Mana     ",
	L"Health   ",
	L"Speed    ",
	L"Jump     ",
	L"Guns     ",
	L"Teleport "
};
// create an array that stores the number of settings for each cheat
int numCheatSettings[6] = { 2,2,3,3,2,3 };


int legendaryMode = 0;
int lastCommittedSetting = 0;
int uncommittedChanges = 0;

wchar_t manaOptions[2][255] = {	L"default",	L"infinite" };
wchar_t healthOptions[2][255] = { L"default", L"working" };
wchar_t speedOptions[3][255] = { L"default", L"light jog", L"hauling ass" };
wchar_t jumpOptions[3][255] = {	L"default",	L"big hops", L"bigger hops" };
wchar_t gunOptions[2][255] = {	L"default",	L"legendary" };
wchar_t teleOptions[3][255] = { L"pirate_bay", L"tails_mountain", L"hidden_island" };


void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

bool WorldToScreen(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

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
		if (bKeys[VK_END])
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
			Drawing::FilledRect(18, 20, 205, 120, D3DCOLOR_ARGB(5, 255, 0, 0));
			// border, xpos, ypos, width, height 
			Drawing::BorderedRect(17, 19, 205, 120, 1, 1, 1, 1, D3DCOLOR_ARGB(255, 255, 255, 255));
			// header rectangle, xpos, ypos, width, height
			Drawing::FilledRect(17, 19, 205, 19, D3DCOLOR_ARGB(255, 255, 255, 255));
			// header text
			DirectX.Font->DrawTextW(NULL, L"Beaver Trainer v1.1", -1, &pos, 0, D3DCOLOR_ARGB(255, 5, 5, 5));
			pos.top += 20;


			wchar_t swf[255];
			wchar_t swf2[255];

			for (int i = 0; i < NUMCHEATS; i++)
			{
				D3DCOLOR color;
				if (highlightedCheat == i)
					color = D3DCOLOR_ARGB(225, 15, 250, 15);
				else
					color = D3DCOLOR_ARGB(225, 15, 250, 250);
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

				wcscat(swf, swf2);
				DirectX.Font->DrawTextW(NULL, swf, -1, &pos, 0, color);

				pos.top += 16;
			}

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
				if (highlightedCheat == 0)
				{
					int MAXOPTIONS = 2;
					if (currentCheatSetting[highlightedCheat] == MAXOPTIONS)
						currentCheatSetting[highlightedCheat] = 0;

					uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");

					if (currentCheatSetting[highlightedCheat] > 0)
					{
						mem::Patch((BYTE*)(gameLogicAddr + 0x525C5), (BYTE*)"\x90\x090", 2);
					}
					else
					{
						mem::Patch((BYTE*)(gameLogicAddr + 0x525C5), (BYTE*)"\x2B\xC2", 2);
					}
				}

				// Health Boost
				if (highlightedCheat == 1)
				{
					int MAXOPTIONS = 2;
					if (currentCheatSetting[highlightedCheat] == MAXOPTIONS)
						currentCheatSetting[highlightedCheat] = 0;

					uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
					uintptr_t firstLevelPtr, healthAddr;
					const int newHealth = 999999;
					const int defaultHealth = 100;
					std::vector<unsigned int> healthOffsets = { 0x1c, 0x6c, 0x00 };
					firstLevelPtr = gameLogicAddr + 0x97D7C;
					healthAddr = mem::FindDMAAddy(firstLevelPtr, healthOffsets) - 0x40;

					//if (bMenuItems[iSelectedItem] == true)
					if (currentCheatSetting[highlightedCheat] > 0)
					{
						*(int*)healthAddr = newHealth;
					}
					else
					{
						*(int*)healthAddr = defaultHealth;
					}
				}

				// Speed Boost
				if (highlightedCheat == 2)
				{

					int MAXOPTIONS = 3;
					if (currentCheatSetting[highlightedCheat] == MAXOPTIONS)
						currentCheatSetting[highlightedCheat] = 0;

					uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
					uintptr_t firstLevelPtr, speedAddr;
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
					int MAXOPTIONS = 3;
					if (currentCheatSetting[highlightedCheat] == MAXOPTIONS)
						currentCheatSetting[highlightedCheat] = 0;

					uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
					uintptr_t firstLevelPtr, jumpAddr;
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
					if (legendaryMode == 0)
					{

						uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
						uintptr_t firstLevelPtrAddr = gameLogicAddr + 0x97D7C;
						uintptr_t getItemByNameFuncOff = 0x1DE20;
						uintptr_t addItemFuncOff = 0x51BA0;

						// various offsets list
						vector<unsigned int> playerPointerAddressOffset = { 0x1c, 0x6c };
						vector<unsigned int> playerVftableAddr = { 0x1c, 0x6c, 0x0 };

						//// player pointer
						void* pPlayer = (void*)mem::FindDMAAddy(firstLevelPtrAddr, playerPointerAddressOffset);
						void* pPlayervftable = (void*)mem::FindDMAAddy(firstLevelPtrAddr, playerVftableAddr);
						// game pointer
						void* pGame = (void*)(gameLogicAddr + 0x9780);					

						// create instance of getItemByName function
						GetItemByName = (_GetItemByName)(gameLogicAddr + getItemByNameFuncOff);

						// create instance of addItem function
						AddItem = (_AddItem)(gameLogicAddr + addItemFuncOff);

						// hold pointer to item
						void* pGBF = GetItemByName(pGame, sGreatBallsOfFire);
						//cout << sGreatBallsOfFire << " address: " << std::hex << pGBF << endl;
						AddItem(pPlayervftable, pGBF, 1, 0);

						void* pOPFireball = GetItemByName(pGame, sOPFireball);
						//cout << sOPFireball << " address: " << std::hex << pOPFireball << endl;
						AddItem(pPlayervftable, pOPFireball, 1, 0);

						void* pROPGun = GetItemByName(pGame, sROPChainGun);
						//cout << sROPChainGun << " address: " << std::hex << pROPGun << endl;
						AddItem(pPlayervftable, pROPGun, 1, 0);

						void* pGoldMaster = GetItemByName(pGame, sGoldMaster);
						//cout << sGoldMaster << " address: " << std::hex << pGoldMaster << endl;
						AddItem(pPlayervftable, pGoldMaster, 1, 0);

						void* pRemoteExploit = GetItemByName(pGame, sRemoteExploit);
						//cout << sRemoteExploit << " address: " << std::hex << pRemoteExploit << endl;
						AddItem(pPlayervftable, pRemoteExploit, 1, 0);

						void* pHeapSpray = GetItemByName(pGame, sHeapSpray);
						//cout << sHeapSpray << " address: " << std::hex << pHeapSpray << endl;
						AddItem(pPlayervftable, pHeapSpray, 1, 0);

						void* pHandCannon = GetItemByName(pGame, sHandCannon);
						//cout << sHandCannon << " address: " << std::hex << pHandCannon << endl;
						AddItem(pPlayervftable, pHandCannon, 1, 0);

						void* pPistolAmmo = GetItemByName(pGame, sPistolAmmo);
						//cout << sPistolAmmo << " address: " << std::hex << pPistolAmmo << endl;
						AddItem(pPlayervftable, pPistolAmmo, 9999, 1);

						void* pShotgunAmmo = GetItemByName(pGame, sShotgunAmmo);
						//cout << sShotgunAmmo << " address: " << std::hex << pShotgunAmmo << endl;
						AddItem(pPlayervftable, pShotgunAmmo, 9999, 1);

						void* pRifleAmmo = GetItemByName(pGame, sRifleAmmo);
						//cout << sRifleAmmo << " address: " << std::hex << pRifleAmmo << endl;
						AddItem(pPlayervftable, pRifleAmmo, 9999, 1);

						void* pSniperAmmo = GetItemByName(pGame, sSniperAmmo);
						//cout << sSniperAmmo << " address: " << std::hex << pSniperAmmo << endl;
						AddItem(pPlayervftable, pSniperAmmo, 9999, 1);

						void* pRevolverAmmo = GetItemByName(pGame, sRevolverAmmo);
						//cout << sRevolverAmmo << " address: " << std::hex << pRevolverAmmo << endl;
						AddItem(pPlayervftable, pRevolverAmmo, 9999, 1);

						void* pPwnCoin = GetItemByName(pGame, sPwnCoin);
						//cout << sPwnCoin << " address: " << std::hex << pPwnCoin << endl;
						AddItem(pPlayervftable, pPwnCoin, 9999, 1);

						legendaryMode = 1;
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

					uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
					uintptr_t firstLevelPtrAddr = gameLogicAddr + 0x97D7C;
					Teleport = (_Teleport)(gameLogicAddr + teleportFuncOff);
					vector<unsigned int> playerVftableAddr = { 0x1c, 0x6c, 0x0 };
					void* pPlayervftable = (void*)mem::FindDMAAddy(firstLevelPtrAddr, playerVftableAddr);


					if (currentCheatSetting[highlightedCheat] == 2)
					{
						//teleport to hidden island
						Teleport(pPlayervftable, "CowLevel");
					}
					else if (currentCheatSetting[highlightedCheat] == 1)
					{
						//teleport to tails mountain
						Teleport(pPlayervftable, "TailMountains");

					}
					else if (currentCheatSetting[highlightedCheat] == 0)
					{
						Teleport(pPlayervftable, "PirateBay");
					}
				}
			}
		}
	}
	DirectX.Device->EndScene();
	DirectX.Device->PresentEx(0, 0, 0, 0, 0);
	DirectX.Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
}

//Print our pattern scan results if necessary
void MsgBoxAddy(DWORD addy)
{
	char szBuffer[1024];
	sprintf(szBuffer, "%02x", addy);
	MessageBox(NULL, szBuffer, "Title", MB_OK);
}

#pragma region Mid Function Hook/Code cave
/*Credits to InSaNe on MPGH for the original function*/
//We make Length at the end optional as most jumps will be 5 or less bytes
void PlaceJMP(BYTE* Address, DWORD jumpTo, DWORD length = 5)
{
	DWORD dwOldProtect, dwBkup, dwRelAddr;

	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect(Address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// Calculate the "distance" we're gonna have to jump - the size of the JMP instruction
	dwRelAddr = (DWORD)(jumpTo - (DWORD)Address) - 5;

	// Write the JMP opcode @ our jump position...
	*Address = 0xE9;

	// Write the offset to where we're gonna jump
	//The instruction will then become JMP ff002123 for example
	*((DWORD*)(Address + 0x1)) = dwRelAddr;

	// Overwrite the rest of the bytes with NOPs
	//ensuring no instruction is Half overwritten(To prevent any crashes)
	for (DWORD x = 0x5; x < length; x++)
		* (Address + x) = 0x90;

	// Restore the default permissions
	VirtualProtect(Address, length, dwOldProtect, &dwBkup);
}

#pragma region PATTERN SCANNING
//Get all module related info, this will include the base DLL. 
//and the size of the module
MODULEINFO GetModuleInfo(char* szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}

DWORD FindPattern(char* module, char* pattern, char* mask)
{
	//Get all module related information
	MODULEINFO mInfo = GetModuleInfo(module);

	//Assign our base and module size
	//Having the values right is ESSENTIAL, this makes sure
	//that we don't scan unwanted memory and leading our game to crash
	DWORD base = (DWORD)mInfo.lpBaseOfDll;

	DWORD size = (DWORD)mInfo.SizeOfImage;

	//Get length for our mask, this will allow us to loop through our array
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			//if we have a ? in our mask then we have true by default, 
			//or if the bytes match then we keep searching until finding it or not
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		//found = true, our entire pattern was found
		//return the memory addy so we can write to it
		if (found)
		{
			return base + i;
		}
	}

	return NULL;
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

DWORD WINAPI initiateHook(LPVOID param)
{

	// entityhook as per x32Dbg
	/*DWORD entityAddy = FindPattern("PwnAdventure3-Win32-Shipping.exe",
		"\xC7\x47\x00\x00\x00\x00\x00\x85\xF6", "xx?????xx");*/


		// entityhook with specifics added
		/*DWORD entityAddy = FindPattern("PwnAdventure3-Win32-Shipping.exe",
			"\xC7\x47\x30\x7D\x00\x00\x00\x85\xF6", "xxxxxxxxx");*/

			// entityhook with offsets
	uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");

	uintptr_t bearInitPtr, bearHealthLoad = 0;
	bearInitPtr = gameLogicAddr + 0x6170;
	bearHealthLoad = bearInitPtr + 0x3E;
	DWORD entityAddy = (DWORD)bearHealthLoad;

	// need to go back to BearInit + 0x45 when we're done

	// MsgBoxAddy(entityAddy);
	EntlistJmpBack = entityAddy + 0x7;
	PlaceJMP((BYTE*)entityAddy, (DWORD)entityhook, 7);

	return NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		hInstance = hModule;
		CreateThread(0, NULL, ThreadProc, (LPVOID)L"X", NULL, NULL);
		CreateThread(0, 0, initiateHook, 0, 0, 0);
	}
	return TRUE;
}



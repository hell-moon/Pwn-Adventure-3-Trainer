#pragma once
#include <Windows.h>
typedef void(__thiscall* _Chat)(void* playervftblptr, const char* text);
_Chat Chat;

typedef void* (__thiscall* _GetItemByName)(void* gamePtr, const char* name);
_GetItemByName GetItemByName;

typedef bool(__thiscall* _AddItem)(void* playervftblptr, void* IItemPtr, unsigned int count, bool allowPartial);
_AddItem AddItem;

typedef void(__thiscall* _GiveAll)(void* gamePtr, void* playerPtr);
_GiveAll GiveAll;

typedef void* (__thiscall* _GetPlayerInterface)(void* playervftblptr);
_GetPlayerInterface GetPlayerInterface;

typedef void(__thiscall* _FastTravel)(void* playervftableptr, const char* origin, const char* dest);
_FastTravel FastTravel;

typedef void(__thiscall* _Teleport)(void* playervftableptr, const char* location);
_Teleport Teleport;

uintptr_t addItemFuncOff = 0x51BA0;
uintptr_t getItemByNameFuncOff = 0x1DE20;
uintptr_t chatFuncOff = 0x551A0;
uintptr_t giveAllFuncOff = 0x1E640;
uintptr_t fastTravelFuncOff = 0x55AE0;
uintptr_t teleportFuncOff = 0x54E50;

// item names
const char* sGreatBallsOfFire = "GreatBallsOfFire";
const char* sROPChainGun = "ROPChainGun";
const char* sGoldMaster = "GoldenMaster";
const char* sRemoteExploit = "RemoteExploit";
const char* sOPFireball = "CharStar";
const char* sHeapSpray = "HeapSpray";
const char* sHandCannon = "HandCannon";
const char* sPwnCoin = "Coin";

// ammo names
const char* sPistolAmmo = "PistolAmmo";
const char* sShotgunAmmo = "ShotgunAmmo";
const char* sSniperAmmo = "SniperAmmo";
const char* sRevolverAmmo = "RevolverAmmo";
const char* sRifleAmmo = "RifleAmmo";


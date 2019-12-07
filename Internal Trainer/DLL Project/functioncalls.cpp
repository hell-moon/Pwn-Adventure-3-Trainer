#include <Windows.h>
#include "mem.h"
#include "functioncalls.h"
#include<vector>
using std::vector;

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


// get address of gamelogic.dll
uintptr_t gameLogicAddr = (uintptr_t)GetModuleHandle("GameLogic.dll");
// from gamelogic.dll, use offset to get address of player structure
uintptr_t firstLevelPtr = gameLogicAddr + 0x97D7C;

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

// various offsets list
vector <unsigned int> playerPointerAddressOffset = { 0x1c, 0x6c };
vector <unsigned int> playerVftableAddr = { 0x1c, 0x6c, 0x0 };

// player/ playervftable pointers
void* pPlayer = (void*)mem::FindDMAAddy(firstLevelPtr, playerPointerAddressOffset);
void* pPlayervftable = (void*)mem::FindDMAAddy(firstLevelPtr, playerVftableAddr);
// game pointer
void* pGame = (void*)(gameLogicAddr + 0x9780);

void gameFunc(const char* funcName){
    // create instance of getItemByName function
    GetItemByName = (_GetItemByName)(gameLogicAddr + getItemByNameFuncOff);

    // create instance of addItem function
    AddItem = (_AddItem)(gameLogicAddr + addItemFuncOff);

    // create instance of teleport function
    Teleport = (_Teleport)(gameLogicAddr + teleportFuncOff);

    if(funcName == "getWeapons"){
        // hold pointer to item
        void *pGBF = GetItemByName(pGame, sGreatBallsOfFire);
        //cout << sGreatBallsOfFire << " address: " << std::hex << pGBF << endl;
        AddItem(pPlayervftable, pGBF, 1, 0);

        void *pOPFireball = GetItemByName(pGame, sOPFireball);
        //cout << sOPFireball << " address: " << std::hex << pOPFireball << endl;
        AddItem(pPlayervftable, pOPFireball, 1, 0);

        void *pROPGun = GetItemByName(pGame, sROPChainGun);
        //cout << sROPChainGun << " address: " << std::hex << pROPGun << endl;
        AddItem(pPlayervftable, pROPGun, 1, 0);

        void *pGoldMaster = GetItemByName(pGame, sGoldMaster);
        //cout << sGoldMaster << " address: " << std::hex << pGoldMaster << endl;
        AddItem(pPlayervftable, pGoldMaster, 1, 0);

        void *pRemoteExploit = GetItemByName(pGame, sRemoteExploit);
        //cout << sRemoteExploit << " address: " << std::hex << pRemoteExploit << endl;
        AddItem(pPlayervftable, pRemoteExploit, 1, 0);

        void *pHeapSpray = GetItemByName(pGame, sHeapSpray);
        //cout << sHeapSpray << " address: " << std::hex << pHeapSpray << endl;
        AddItem(pPlayervftable, pHeapSpray, 1, 0);

        void *pHandCannon = GetItemByName(pGame, sHandCannon);
        //cout << sHandCannon << " address: " << std::hex << pHandCannon << endl;
        AddItem(pPlayervftable, pHandCannon, 1, 0);

        void *pPistolAmmo = GetItemByName(pGame, sPistolAmmo);
        //cout << sPistolAmmo << " address: " << std::hex << pPistolAmmo << endl;
        AddItem(pPlayervftable, pPistolAmmo, 9999, 1);

        void *pShotgunAmmo = GetItemByName(pGame, sShotgunAmmo);
        //cout << sShotgunAmmo << " address: " << std::hex << pShotgunAmmo << endl;
        AddItem(pPlayervftable, pShotgunAmmo, 9999, 1);

        void *pRifleAmmo = GetItemByName(pGame, sRifleAmmo);
        //cout << sRifleAmmo << " address: " << std::hex << pRifleAmmo << endl;
        AddItem(pPlayervftable, pRifleAmmo, 9999, 1);

        void *pSniperAmmo = GetItemByName(pGame, sSniperAmmo);
        //cout << sSniperAmmo << " address: " << std::hex << pSniperAmmo << endl;
        AddItem(pPlayervftable, pSniperAmmo, 9999, 1);

        void *pRevolverAmmo = GetItemByName(pGame, sRevolverAmmo);
        //cout << sRevolverAmmo << " address: " << std::hex << pRevolverAmmo << endl;
        AddItem(pPlayervftable, pRevolverAmmo, 9999, 1);

        void *pPwnCoin = GetItemByName(pGame, sPwnCoin);
        //cout << sPwnCoin << " address: " << std::hex << pPwnCoin << endl;
        AddItem(pPlayervftable, pPwnCoin, 9999, 1);
    }
    if (funcName == "toTown"){
        Teleport(pPlayervftable, "Town");
    }
    if (funcName == "toTail"){
        Teleport(pPlayervftable, "TailMountains");
    }
	if (funcName == "toPirate") {
		Teleport(pPlayervftable, "PirateBay");
	}
	if (funcName == "toGold") {
		Teleport(pPlayervftable, "GoldFarm");
	}
	if (funcName == "toBallmer") {
		Teleport(pPlayervftable, "BallmerPeak");
	}
	if (funcName == "toUnbearable") {
		Teleport(pPlayervftable, "UnbearableWoods");
	}
	if (funcName == "toSewer") {
		Teleport(pPlayervftable, "Sewer");
	}
	if (funcName == "toLost") {
		Teleport(pPlayervftable, "LostCave");
	}
}

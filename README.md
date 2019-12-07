CAPSTONE USAGE INSTRUCTIONS

Game Installation:

The target of our Trainer (a  utility for implementing game cheats) is Pwn Adventure 3. To run
our project, the game will need to be installed. It can be downloaded here:
https://www.pwnadventure.com/

Run the executable which will install the game on your computer. Once installation is 
complete, select 'Offline Play' from the main menu and create a character. 


Implementing our project:

Our program is a Dynamic Linked Library (DLL), which will be injected into the game’s process. 
Inside the "Internal Trainer / Binaries - Final" folder you will find two files:
“hackingBeaversInjector.exe” and “Pwn Adventure 3 Internal Trainer.dll”.
Running the exe will inject our cheat into the game.

Note on security:

Our project involves injecting code via an exe. This is likely to be flagged by any 
anti-virus software running on your computer. Anti-virus software may need to be temporarily
disabled while using this program, or exclude the injector from anti-virus scans.

Using the Trainer:

![Image of In-Game Menu](https://i.imgur.com/vvqX6b2.png)

Once the exe is run, a menu will appear within the upper left corner of the screen,
The user is able to use the directional arrows on the NUMPAD to navigate the menu:
- Up and down (8 and 2) will move the orange highlight between the available cheats
- Left and right (4 and 6) will cycle through the available settings for each cheat
- Pressing 0 on the NUMPAD will confirm the user’s selection and engage the cheat
- NUMPAD_SUBTRACT will toggle the visibility of the menu

Description of cheats available:

Mana
Off: the game plays with the default settings, spells drain mana as the game creators intended
Infinite: spells no longer drain the player’s mana

Health
Off: the game plays with the default settings, enemy attacks drain the player’s health as the 
game creators intended
Invincible: enemy attacks no longer detract from the player’s HP

Speed:
Normal: the player moves at the default speed of 200 units
Faster: the player moves at 1000 units
Fastest: the player moves at 3000 units

ump:
Normal: the player jumps to a default height of 420 units
Higher: the player jumps to a height of 1800 units
Highest: the player jumps to a height of 5000 units

Guns
Off: the player starts with no items and must acquire them through gameplay as the creators intended
Legendary: the player is given several legendary guns, ammunition, and 9999 of the in-game currency

Teleport (patched out of final version due to potential bugs, available in "Binaries - Teleport") 
the player is transported to one of the following locations
Town
Tail Mountains
Pirate Bay
Gold Farm
Ballmer Peak
Unbearable Woods
Sewer
Lost Cave

God Mode - enables Health and Mana cheats with the addition of dropping enemies in one hit
Off:  default values for health, mana, and damage output
On:  the player takes no damage, has unlimited mana and damage output increased significantly.
All enemies in the game are able to be defeated with one hit from the players selected weapon/spell.  

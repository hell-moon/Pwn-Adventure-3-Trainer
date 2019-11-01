#include "menuDialogue.h"
#include <iostream>


// welcome the user
void welcome()
{
	std::cout << "Welcome to the Hacking Beaver's Pwn Island 3 Cheat Trainer!\n";
}


// present hack options
int mainMenuOptions()
{
	std::cout << "Use the number pad to toggle a cheat from the menu below:\n";
	std::cout << "1. Infinite Mana\n2. Invincibility\n3. Super speed\n4. Super Jump\n0. Exit Menu\n";

	int userChoice;

	// need to add an integer min/max validation function
	std::cin >> userChoice;

	//std::cout << "You picked " << userChoice << "\n";

	return userChoice;
}
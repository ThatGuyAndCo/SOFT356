#include <iostream>
#include <string>
#include <regex>
#include "ProcGen.h"
#include "ProcGenStructure.h"
using namespace std;


procGenStructure procGenProcess::generateStructure(bool regen) {
	if (!regen) {
		cout << "Get user input for number of rooms...";
		bool validValue = false;
		string inputValue;
		cmatch match;
		while (!validValue) {
			cin >> inputValue; //Min 2, max of maybe 100, depends on performance.

			std::regex ex("[0-9]{1,3}");
			if (std::regex_match(inputValue.c_str(), match, ex)) {
				numberOfRooms = stoi(match[0]);
				if (numberOfRooms < 2) {
					numberOfRooms = 2;
					std::cout << "\nMinimum of 2 rooms, setting rooms to 2.\n";
				}
				validValue = true;
			}
			else {
				std::cout << "\nInvalid value entered, please enter a valid integer.\n";
			}
		}
	}
	currentStructure.clearStructure();
	currentStructure.startGeneration(numberOfRooms);
	return currentStructure;
}
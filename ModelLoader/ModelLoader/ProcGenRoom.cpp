#include <string>
#include <iostream>
#include "ProcGenRoom.h"
using namespace std;

	void procGenRoom::calculateVariant() {
		if (numberOfDoors == 1) { //No variants, just rotation
			modelNumber = "1_1_" + to_string(roomType);
			calculateOneAngle();
		}
		else if (numberOfDoors == 2) { //4 rotations for variant 1, 2 rotations for variant 2
			if ((doorPositions[0] && doorPositions[2]) || (doorPositions[1] && doorPositions[3])) { //Doors are opposite
				modelNumber = "2_2_" + to_string(roomType);
				calculateTwoTwoAngle();
			}
			else {
				modelNumber = "2_1_" + to_string(roomType);
				calculateTwoOneAngle();
			}
		}
		else if (numberOfDoors == 3) {//No variants, just rotation
			modelNumber = "3_1_" + to_string(roomType);
			calculateThreeAngle();
		}
		else if (numberOfDoors == 4) { //No variants or rotation
			modelNumber = "4_1_" + to_string(roomType);
		}
		else {
			cout << "\n\nUhoh, something went very wrong at calculateVariant!\n";
			cout << "Number of doors should be 1/2/3/4 but was actually: " << numberOfDoors << " for room at Coordinates: " << xCoord << ", " << yCoord << "\n\n";
		}
	}

	//Model defaults to north
	void procGenRoom::calculateOneAngle() {
		for (unsigned int i = 0; i < 4; i++) {
			if (doorPositions[i] == true) {
				return;
			}
			else {
				rotationForDoorMatch = rotationForDoorMatch + 90.0f;
			}
		}
		cout << "\n\nUhoh, something went very wrong at calculateOneAngle!\n";
		cout << "doorPositions should have 1 true value, but didn't for room at Coordinates: " << xCoord << ", " << yCoord << "\n\n";
	}

	//Model default to north/east
	void procGenRoom::calculateTwoOneAngle() {
		for (unsigned int i = 0; i < 4; i++) {
			if (i != 3 && doorPositions[i] == true && doorPositions[i + 1] == true) {
				return;
			}
			else if (i == 3 && doorPositions[i] == true && doorPositions[0] == true) {
				return;
			}
			else {
				rotationForDoorMatch = rotationForDoorMatch + 90.0f;
			}
		}
		cout << "\n\nUhoh, something went very wrong at calculateTwoOneAngle!\n";
		cout << "doorPositions should have at 2 true values, but didn't for room at Coordinates: " << xCoord << ", " << yCoord << "\n";
		cout << "doorPositions are as follows: " << doorPositions[0] << ", " << doorPositions[1] << ", " << doorPositions[2] << ", " << doorPositions[3] << "\n\n";
	}

	//Model defaults to north/south
	void procGenRoom::calculateTwoTwoAngle() {
		for (unsigned int i = 0; i < 2; i++) {
			if (doorPositions[i] == true && doorPositions[i + 2] == true) {
				return;
			}
			else {
				rotationForDoorMatch = rotationForDoorMatch + 90.0f;
			}
		}
		cout << "\n\nUhoh, something went very wrong at calculateThreeAngle!\n";
		cout << "doorPositions should have at 3 true values, but didn't for room at Coordinates: " << xCoord << ", " << yCoord << "\n";
		cout << "doorPositions are as follows: " << doorPositions[0] << ", " << doorPositions[1] << ", " << doorPositions[2] << ", " << doorPositions[3] << "\n\n";
	}

	//Model defaults to north/east/south
	void procGenRoom::calculateThreeAngle() {
		for (unsigned int i = 0; i < 4; i++) {
			if (i < 2 && doorPositions[i] == true && doorPositions[i + 1] == true && doorPositions[i + 2] == true) {
				return;
			}
			else if (i == 2 && doorPositions[i] == true && doorPositions[i + 1] == true && doorPositions[0] == true) {
				return;
			}
			else if (i == 3 && doorPositions[i] == true && doorPositions[0] == true && doorPositions[1] == true) {
				return;
			}
			else {
				rotationForDoorMatch = rotationForDoorMatch + 90.0f;
			}
		}
		cout << "\n\nUhoh, something went very wrong at calculateThreeAngle!\n";
		cout << "doorPositions should have at 3 true values, but didn't for room at Coordinates: " << xCoord << ", " << yCoord << "\n";
		cout << "doorPositions are as follows: " << doorPositions[0] << ", " << doorPositions[1] << ", " << doorPositions[2] << ", " << doorPositions[3] << "\n\n";
	}

	string procGenRoom::getModelNumber() {
		return modelNumber;
	}

	float procGenRoom::getRotationDegrees() {
		return rotationForDoorMatch;
	}

	bool procGenRoom::getIsRoomUsed() {
		return isRoomUsed;
	}

	bool procGenRoom::getIsGoalRoom() {
		if (roomType == 1)
			return true;
		else
			return false;
	}

	unsigned int procGenRoom::getXCoord() {
		return xCoord;
	}

	unsigned int procGenRoom::getYCoord() {
		return yCoord;
	}

	void procGenRoom::enableStartRoom(unsigned int xCoord, unsigned int yCoord) {
		isRoomUsed = true;
		this->xCoord = xCoord;
		this->yCoord = yCoord;
		roomType = 0;
	}

	bool procGenRoom::enableRoom(unsigned int xCoord, unsigned int yCoord, unsigned int isGoalRoom, bool goalRoomExists, bool lastRoom) {
		isRoomUsed = true;
		this->xCoord = xCoord;
		this->yCoord = yCoord;
		if (!goalRoomExists && (isGoalRoom > 95 || lastRoom)) {
			roomType = 1;
			return true;
		}
		else {
			roomType = 0;
			return goalRoomExists;
		}
	}

	void procGenRoom::updateDoors(unsigned int index, bool value) {
		doorPositions[index] = value;
	}

	void procGenRoom::calculateModel() {
		numberOfDoors = 0;
		for (int i = 0; i < 4; i++) {
			if (doorPositions[i] == true) {
				numberOfDoors++;
			}
		}

		calculateVariant();
	}
	
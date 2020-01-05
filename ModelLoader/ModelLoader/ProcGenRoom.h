#ifndef PROCGENROOM_H
#define PROCGENROOM_H

#include <string>
using namespace std;

class procGenRoom {
private:
	bool isRoomUsed = false;
	unsigned int xCoord = 0;
	unsigned int yCoord = 0;
	unsigned int roomType = 0; // 0  = standard, 1 = goal
	unsigned int numberOfDoors = 1; //1 door for each room adjacent to it on the structure
	bool doorPositions[4] = { false, false, false, false }; //0 = north, 1 = east, 2 = south, 3 = west; true = door, false = no door

	//Only the below variables will be used for the model loader pipeline to display the created rooms. 

	//Model number based on following structure: (# of doors)_(variant)_(room_type)
	//i.e. 2_2_0 would be a 2 door room, with doors opposite each other, and the standard room model
	string modelNumber = "1_1_0";
	//To reduce number of models needed, rotate existing models into the correct positions 90 degrees at a time.
	//This is much less performance efficient as it requires looping through and testing 4 different rotations of various model types to discover which model is appropriate for the connected rooms.
	float rotationForDoorMatch = 0.0f;

	void procGenRoom::calculateVariant();
	//Model defaults to north
	void calculateOneAngle();
	//Model default to north/east
	void calculateTwoOneAngle();
	//Model defaults to north/south
	void calculateTwoTwoAngle();
	//Model defaults to north/east/south
	void calculateThreeAngle();

public:
	std::string getModelNumber();
	float getRotationDegrees();
	bool getIsRoomUsed();
	bool getIsGoalRoom();
	unsigned int getXCoord();
	unsigned int getYCoord();
	void enableStartRoom(unsigned int xCoord, unsigned int yCoord);
	bool enableRoom(unsigned int xCoord, unsigned int yCoord, unsigned int isGoalRoom, bool goalRoomExists, bool lastRoom);
	void updateDoors(unsigned int index, bool value);
	void calculateModel();
};

#endif
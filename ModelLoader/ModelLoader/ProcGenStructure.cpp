#include <vector>
#include <iostream>
#include <random>
#include "ProcGenStructure.h"
#include "ProcGenRoom.h"
using namespace std;

procGenRoom procGenStructure::getSpawnRoom() {
	return spawnRoom;
}
procGenRoom procGenStructure::getGoalRoom() {
	return goalRoom;
}
vector<procGenRoom> procGenStructure::getRoomList() {
	return roomList;
}

void procGenStructure::clearStructure() {
	roomList.clear();
}

void procGenStructure::startGeneration(unsigned int numberOfRooms) {
	vector< vector<procGenRoom> > roomGenerationList;

	//Generate structure for creating random numbers (as rand must be manually seeded else it will produce the same results each
	//time the program is run)
	random_device r;
	default_random_engine e1(r());
	uniform_int_distribution<int> uniform_dist(1, 100);
	cout << "\nPreallocating:";
	//Preallocate the nested vectors to fill it with "empty cells"
	for (unsigned int x = 0; x < numberOfRooms; x++) {
		vector<procGenRoom> tempRow;
		for (unsigned int y = 0; y < numberOfRooms; y++) {
			procGenRoom pgr;
			if (x == numberOfRooms / 2 && y == numberOfRooms / 2) {
				pgr.enableStartRoom(x, y);
				spawnRoom = pgr;
			}
			tempRow.push_back(pgr);
		}
		roomGenerationList.push_back(tempRow);
	}

	cout << "\nGenerating:";
	//Generate a structure of rooms branching from already existing rooms. Loop until user provided number of rooms exist.
	//Each time a room is available to be 'generated' it has a 60% chance to actually be generated, and a 5% chance of being the goal room
	//The more doors an existing room has, the lower the chance of a the room being generated (this helps make maps crawl out further)
	//If no goal room exists when the final room is created, that room will be the goal room
	unsigned int currentNumOfRooms = 1; //+1 for starting room
	bool lastRoom = false;
	bool goalRoomMade = false;
	while (numberOfRooms > currentNumOfRooms) {
		for (unsigned int x = 0; x < numberOfRooms; x++) {
			for (unsigned int y = 0; y < numberOfRooms; y++) {
				if (currentNumOfRooms == numberOfRooms - 1) //Account for a 2-cell level
					lastRoom = true;
				procGenRoom pgr = roomGenerationList[x][y];
				if (pgr.getIsRoomUsed()) {
					unsigned int spawnChance = 0;
					
					if (y + 1 != numberOfRooms && roomGenerationList[x][y + 1].getIsRoomUsed() == false){
						unsigned int numAdjacentRooms = 0; //Count the adjacent rooms to calculate an smaller chance for a room to spawn for each room thats connected already
						if (y + 2 != numberOfRooms && roomGenerationList[x][y + 2].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (x + 1 != numberOfRooms && y + 1 != numberOfRooms && roomGenerationList[x + 1][y+1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (y + 1 != numberOfRooms && roomGenerationList[x][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (x != 0 && y + 1 != numberOfRooms && roomGenerationList[x - 1][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						//cout << "\n\nAdjRooms: " << numAdjacentRooms;
						spawnChance = (unsigned int)((numAdjacentRooms * 15) + 1) * uniform_dist(e1);
						//cout << "spawnChance: " << spawnChance;
						if (spawnChance < 80) {
							//cout << "\nspawning";
							goalRoomMade = roomGenerationList[x][y + 1].enableRoom(x, y + 1, uniform_dist(e1), goalRoomMade, lastRoom);
							if (goalRoomMade)
								goalRoom = roomGenerationList[x][y + 1];
							currentNumOfRooms++;
							if (currentNumOfRooms == numberOfRooms - 1)
								lastRoom = true;
							else if (currentNumOfRooms == numberOfRooms)
								break;
						}
					}

					if (x + 1 != numberOfRooms && roomGenerationList[x + 1][y].getIsRoomUsed() == false) {
						unsigned int numAdjacentRooms = 0; //Count the adjacent rooms to calculate an smaller chance for a room to spawn for each room thats connected already
						if (y + 1 != numberOfRooms && x + 1 != numberOfRooms && roomGenerationList[x+1][y + 1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (x + 2 != numberOfRooms && roomGenerationList[x + 2][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (y != 0 && x + 1 != numberOfRooms && roomGenerationList[x+1][y - 1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (roomGenerationList[x][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						//cout << "\n\nAdjRooms: " << numAdjacentRooms;
						spawnChance = (unsigned int)((numAdjacentRooms * 25) + 1) * uniform_dist(e1);
						//cout << "spawnChance: " << spawnChance;
						if (spawnChance < 50) {
							//cout << "\nspawning";
							goalRoomMade = roomGenerationList[x + 1][y].enableRoom(x + 1, y, uniform_dist(e1), goalRoomMade, lastRoom);
							if (goalRoomMade)
								goalRoom = roomGenerationList[x + 1][y];
							currentNumOfRooms++;
							if (currentNumOfRooms == numberOfRooms - 1)
								lastRoom = true;
							else if (currentNumOfRooms == numberOfRooms)
								break;
						}
					}

					if (y != 0 && roomGenerationList[x][y - 1].getIsRoomUsed() == false) {
						unsigned int numAdjacentRooms = 0; //Count the adjacent rooms to calculate an smaller chance for a room to spawn for each room thats connected already
						if (roomGenerationList[x][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (x + 1 != numberOfRooms && y != 0 && roomGenerationList[x + 1][y-1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (y != 1 && roomGenerationList[x][y - 2].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (x != 0 && y != 0 && roomGenerationList[x - 1][y - 1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						//cout << "\n\nAdjRooms: " << numAdjacentRooms;
						spawnChance = (unsigned int)((numAdjacentRooms * 15) + 1) * uniform_dist(e1);
						//cout << "spawnChance: " << spawnChance;
						if (spawnChance < 80) {
							//cout << "\nspawning";
							goalRoomMade = roomGenerationList[x][y - 1].enableRoom(x, y - 1, uniform_dist(e1), goalRoomMade, lastRoom);
							if (goalRoomMade)
								goalRoom = roomGenerationList[x][y - 1];
							currentNumOfRooms++;
							if (currentNumOfRooms == numberOfRooms - 1)
								lastRoom = true;
							else if (currentNumOfRooms == numberOfRooms)
								break;
						}
					}

					if (x != 0 && roomGenerationList[x - 1][y].getIsRoomUsed() == false) {
						unsigned int numAdjacentRooms = 0; //Count the adjacent rooms to calculate an smaller chance for a room to spawn for each room thats connected already
						if (y + 1 != numberOfRooms && x != 1 && roomGenerationList[x-1][y + 1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (roomGenerationList[x][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (y != 0 && x != 1 && roomGenerationList[x-1][y - 1].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						if (x != 1 && roomGenerationList[x - 2][y].getIsRoomUsed() == true) {
							numAdjacentRooms = numAdjacentRooms + 1;
						}
						//cout << "\n\nAdjRooms: " << numAdjacentRooms;
						spawnChance = (unsigned int)((numAdjacentRooms * 25) + 1) * uniform_dist(e1);
						//cout << "spawnChance: " << spawnChance;
						if (spawnChance < 50) {
							//cout << "\nspawning";
							goalRoomMade = roomGenerationList[x - 1][y].enableRoom(x - 1, y, uniform_dist(e1), goalRoomMade, lastRoom);
							if (goalRoomMade)
								goalRoom = roomGenerationList[x - 1][y];
							currentNumOfRooms++;
							if (currentNumOfRooms == numberOfRooms - 1)
								lastRoom = true;
							else if (currentNumOfRooms == numberOfRooms)
								break;
						}
					}
				}
			}
			if (currentNumOfRooms == numberOfRooms) //This is for breaking the for loop, not the while loop (which will break automatically)
				break;
		}
	}

	cout << "\nLinking:";
	//For used rooms, update doors of adjacent used rooms (link up rooms next to each other)
	//We would need to update the doors here to link adjacent rooms that werent created from one-another, so the update doors is called
	//in bulk here rather than when each room is created
	//Then, calculate which model and rotation is required, and write used rooms to a list
	for (unsigned int x = 0; x < numberOfRooms; x++) {
		for (unsigned int y = 0; y < numberOfRooms; y++) {
			procGenRoom pgr = roomGenerationList[x][y];
			if (pgr.getIsRoomUsed()) {
				if (y + 1 != numberOfRooms && roomGenerationList[x][y + 1].getIsRoomUsed() == true) { //South
					pgr.updateDoors(2, true);
				}
				if (x + 1 != numberOfRooms && roomGenerationList[x + 1][y].getIsRoomUsed() == true) { //West
					pgr.updateDoors(3, true);
				}
				if (y != 0 && roomGenerationList[x][y - 1].getIsRoomUsed() == true) { //North
					pgr.updateDoors(0, true);
				}
				if (x != 0 && roomGenerationList[x - 1][y].getIsRoomUsed() == true) { //East
					pgr.updateDoors(1, true);
				}

				pgr.calculateModel();
				roomList.push_back(pgr);
			}
		}
	}

	cout << "\nCleaning:";
	//Delete the nested vectors to free space
	for (unsigned int x = 0; x < numberOfRooms; x++) {
		roomGenerationList[x].clear();
	}
	roomGenerationList.clear();
}
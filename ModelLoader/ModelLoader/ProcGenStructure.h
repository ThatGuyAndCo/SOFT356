#ifndef PROCGENSTRUCTURE_H
#define PROCGENSTRUCTURE_H

#include <vector>
#include "ProcGenRoom.h"
using namespace std;

class procGenStructure {
private:
	procGenRoom spawnRoom;
	procGenRoom goalRoom;
	vector<procGenRoom> roomList; 
	unsigned int numberOfAdjacentRooms(vector< vector<procGenRoom> > roomGenerationList, unsigned int numberOfRooms, unsigned int x, unsigned int y);
public:
	procGenRoom getSpawnRoom();
	procGenRoom getGoalRoom();
	std::vector<procGenRoom> getRoomList();
	void clearStructure();
	void startGeneration(unsigned int numberOfRooms);
};

#endif
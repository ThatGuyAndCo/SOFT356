#ifndef PROCGEN_H
#define PROCGEN_H

#include "ProcGenStructure.h"

class procGenProcess {
private:
	procGenStructure currentStructure;
	unsigned int numberOfRooms;

public:
	procGenStructure generateStructure(bool regen);
};

#endif
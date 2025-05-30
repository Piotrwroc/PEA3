#ifndef SIMULATION_H
#define SIMULATION_H

#include "Menu.h"

class Simulation
{

public:
	void simulation_mode(int algnum);
	const char* alg_name(int algnum);
	const char* TS_mode_name(int algnum);
	const char* SA_mode_name(int algnum);
};

#endif SIMULATION_H
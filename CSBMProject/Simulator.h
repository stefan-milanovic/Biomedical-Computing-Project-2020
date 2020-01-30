#ifndef _SIMULATOR_H_

#define _SIMULATOR_H_

#include "Configuration.h"
#include "SimulationInfo.h"
#include <chrono>

class Simulator {

public:
	Simulator(Configuration& conf) : config(conf) {}

	void simulate();
private:

	long maximumTime;
	Configuration config;

	vector<SimulationInfo> simulationInfos;

	std::chrono::steady_clock::time_point startTime, endTime;

};

#endif
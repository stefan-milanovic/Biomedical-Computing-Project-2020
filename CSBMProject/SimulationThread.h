#ifndef _SIMULATIONTHREAD_H_

#define _SIMULATIONTHREAD_H_

#include "SimulationInfo.h"

class SimulationThread {
public:

	void operator()(SimulationInfo simulationInfo);
};

#endif
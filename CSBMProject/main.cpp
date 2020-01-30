#include <iostream>
#include <string>

#include "Configuration.h"
#include "Simulator.h"

using namespace std;

int main() {

	const string CONFIG_FILENAME = "config.conf";

	// 1) Parse the configuration file and create a Configuration object which holds the parameters for the simulation.
	Configuration config(CONFIG_FILENAME);

	// 2) Create the simulator object.
	Simulator simulator(config);

	// 3) Perform the simulation. This object will use threads to execute concurrent simulations and store the results.
	//	  This function internally waits for all working threads to finish their execution.
	simulator.simulate();

	// 4) Results will be routed to files by the worker threads.

	return 0;
}
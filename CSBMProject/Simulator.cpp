#include "Simulator.h"
#include "SimulationInfo.h"
#include <chrono>
#include <string>

void Simulator::simulate() {

	// Measure time.
	startTime = std::chrono::steady_clock::now();
	
	// Each SimulationInfo calculates a random set of populations and parameters internally.
	for (int i = 0; i < config.getNumberOfSimulations(); i++) {
		simulationInfos.push_back(SimulationInfo(config));
	}

	// Create the otuput directory (if it doesn't exist).
	system("mkdir output_files");

	// For each simulation info.
	for (unsigned i = 0; i < simulationInfos.size(); i++) {

		double currentSimulatedTime = 0;
		SimulationInfo& simulationInfo = simulationInfos[i];

		// Save simulation info with time = 0.
		simulationInfo.saveIteration(currentSimulatedTime);

		while (config.getMaximumDuration() == 0 ? (currentSimulatedTime < config.getMaximumDuration() && simulationInfo.getInfectousCount() > 0) : simulationInfo.getInfectousCount() > 0) {

			simulationInfo.updateProbabilities();
			currentSimulatedTime += simulationInfo.getTimeOfNextEvent();
			simulationInfo.selectProcess();
			simulationInfo.checkEvents(currentSimulatedTime);

			// Save iteration results for file output at the end of the simulation.
			simulationInfo.saveIteration(currentSimulatedTime);
		}

		simulationInfo.outputToFile();
	}
	

	// Stop measuring time.
	endTime = std::chrono::steady_clock::now();

	// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
}

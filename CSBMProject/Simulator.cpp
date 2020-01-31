#include "Simulator.h"
#include "SimulationInfo.h"
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <omp.h>

void Simulator::simulate() {

	// Measure time.
	startTime = std::chrono::steady_clock::now();
	
	// Each SimulationInfo calculates a random set of populations and parameters internally.
	for (int i = 0; i < config.getNumberOfSimulations(); i++) {
		simulationInfos.push_back(SimulationInfo(config));
	}

	// Create the otuput directory (if it doesn't exist).
	system("mkdir output_files");

	// Create threads.
	
#pragma omp parallel for num_threads(config.GetThreadCount())
	// For each simulation info.
	for (int i = 0; i < (int)simulationInfos.size(); i++) {

		double currentSimulatedTime = 0;
		SimulationInfo& simulationInfo = simulationInfos[i];

		// Save simulation info with time = 0.
		simulationInfo.saveIteration(currentSimulatedTime);

		while (config.getMaximumDuration() != 0 ? (currentSimulatedTime < config.getMaximumDuration() && simulationInfo.getInfectousCount() > 0) : simulationInfo.getInfectousCount() > 0) {

			simulationInfo.updateProbabilities();
			currentSimulatedTime += simulationInfo.getTimeOfNextEvent();
			simulationInfo.selectProcess();
			simulationInfo.checkEvents(currentSimulatedTime);

			// Save iteration results for file output at the end of the simulation.
			simulationInfo.saveIteration(currentSimulatedTime);

			// End simulations lasting longer than two years.
			if (currentSimulatedTime > 730) {
				break;
			}
		}

		simulationInfo.outputToFile(config.getOutputFormat());
	}


	// Stop measuring time.
	endTime = std::chrono::steady_clock::now();

	// Output aggreggated data.
	outputAggreggatedData();

	// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
}

void Simulator::outputAggreggatedData() {
	string filename = "output_files/output_simulations_all.csv";

	ofstream cout;

	cout.open(filename);

	cout << "Epidemic End,Mortality Rate, Infected Mortality Rate, Recovery Rate, Incubation Period, Infection Rate" << endl;
	for (SimulationInfo simulation : simulationInfos) {
		cout << simulation.getSimulationData()[simulation.getSimulationData().size() - 1].timestamp << ",";
		cout << simulation.getMortalityRate() << ",";
		cout << simulation.getInfectedMortalityRate() << ",";
		cout << simulation.getRecoveryRate() << ",";
		cout << simulation.getIncubationPeriod() << ",";
		cout << simulation.getInfectionRate() << endl;
	}
	
	cout.close();
}

void Simulator::outputEnsembleData() {
	string filename = "output_files/ensemble.csv";

	ofstream cout;

	cout.open(filename);

	cout << "Susceptible,Exposed,Infected,Recovered" << endl;
	for (SimulationInfo simulation : simulationInfos) {
		if (simulation.getSimulationData()[simulation.getSimulationData().size() - 1].timestamp >= 29.95) {
			cout << simulation.getSimulationData()[simulation.getSimulationData().size() - 1].susceptible << ",";
			cout << simulation.getSimulationData()[simulation.getSimulationData().size() - 1].exposed << ",";
			cout << simulation.getSimulationData()[simulation.getSimulationData().size() - 1].infected << ",";
			cout << simulation.getSimulationData()[simulation.getSimulationData().size() - 1].recovered << endl;
		}
	}

	cout.close();
}
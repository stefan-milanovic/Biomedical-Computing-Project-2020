#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <iostream>
#include <vector>

using namespace std;

class Configuration {

public:

	enum SimulationType {
		SIR,
		SEIR,
		SEIR_simplified
	};

	Configuration(string configFilename);

	void setType(SimulationType simulationType) { type = simulationType; }
	void setMaximumDuration(double maxDuration) { maximumDuration = maxDuration; }

	void setNumberOfSimulations(int simulationCount) {
		numberOfSimulations = simulationCount;
	}

	void setNumberOfThreads(int numberOfThreads) { threadCount = numberOfThreads; }

	void addPopulationBoundary(vector<int> boundaries) {
		populationBoundaries.push_back(boundaries);
	}

	void addParameterBoundary(vector<double> boundaries) {
		parameterBoundaries.push_back(boundaries);
	}

	void addVaccinationTimestampBoundary(double boundary) {
		vaccinationTimestampBoundaries.push_back(boundary);
	}

	void setVaccinationEfficiency(double efficiency) { vaccinationEfficiency = efficiency; }

	void setRevaccinationEfficiency(double efficiency) { revaccinationEfficiency = efficiency; }

	void addEvent(bool event) {
		events.push_back(event);
	}

	SimulationType getType() { return type; }
	double getMaximumDuration() { return maximumDuration; }
	int getNumberOfSimulations() { return numberOfSimulations; }

	int GetThreadCount() { return threadCount; }

	vector<vector<int>> getPopulationBoundaries() { return populationBoundaries; }
	vector<vector<double>> getParameterBoundaries() { return parameterBoundaries; }
	vector<bool> getEvents() { return events; }

	vector<double> getVaccinationTimestampBoundaries() { return vaccinationTimestampBoundaries; }
	double getVaccinationEfficiency() { return vaccinationEfficiency; }
	double getRevaccinationEfficiency() { return revaccinationEfficiency; }

private:

	SimulationType type;
	double maximumDuration;

	int numberOfSimulations;

	int threadCount;

	vector<vector<int>> populationBoundaries;
	vector<vector<double>> parameterBoundaries;


	vector<bool> events;
	vector<double> vaccinationTimestampBoundaries;
	double vaccinationEfficiency;
	double revaccinationEfficiency;

};

#endif
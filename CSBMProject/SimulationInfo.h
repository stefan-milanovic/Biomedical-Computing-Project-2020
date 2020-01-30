#ifndef _SIMULATIONINFO_H_

#define _SIMULATIONINFO_H_

#include <vector>
#include <string>

#include "Configuration.h"

using namespace std;

struct RecordedData {
	double timestamp;
	int susceptible;
	int exposed;
	int infected;
	int recovered;
	int total;
	int births;
	int deathsSuspectible;
	int deathsInfected;
	int deathsRecovered;
	int deathsDueToInfection;
	int deathsTotal;

	RecordedData(double time, int S, int E, int I, int R, int N, int born, int diedS, int diedI, int diedR, int diedToInf, int diedTotal) :
	timestamp(time), susceptible(S), exposed(E), infected(I), recovered(R), total(N), births(born), deathsSuspectible(diedS),
	deathsInfected(diedI), deathsRecovered(diedR), deathsDueToInfection(diedToInf), deathsTotal(diedTotal) {}
};

struct Event {
	bool occurred = false;
	string eventName;
	Event(string name) : eventName(name) {}
};

class SimulationInfo {

public:

	SimulationInfo(Configuration& config);

	// Getters.

	const int getTotalPopulation() { return totalPopulation; }
	const int getInfectedCount() { return infected; }
	const int getSusceptibleCount() { return susceptible; }
	const int getRecoveredCount() { return recovered; }
	const int getExposedCount() { return exposed; }

	const int getInfectousCount() { return infected + exposed; }

	void updateProbabilities();
	double getTimeOfNextEvent();
	void selectProcess();
	void checkEvents(double time);

	const void outputToFile();
	void saveIteration(double currentTime);

private:

	// SIR/SEIR process.
	const double birthChance() {

		if (simulationType == Configuration::SimulationType::SEIR_simplified) {
			return 0;
		}

		return mortalityRate * totalPopulation;
		// S++
	}

	// SIR/SEIR process.
	const double deathOfSusceptibleChance() {

		if (simulationType == Configuration::SimulationType::SEIR_simplified) {
			return 0;
		}

		return mortalityRate * susceptible;
		// S--
	}

	// SEIR/SEIR_simplified process (not in SIR).
	const double sicknessChance() {

		if (simulationType == Configuration::SimulationType::SIR) {
			return 0;
		}

		return incubationPeriod * exposed;
		// E--, I++
	}

	// SIR/SEIR process.
	const double deathOfInfectedChance() {

		if (simulationType == Configuration::SimulationType::SEIR_simplified) {
			return 0;
		}

		return mortalityRate * infected;
		// I--
	}

	// SIR/SEIR process.
	const double deathOfRecoveredChance() {

		if (simulationType == Configuration::SimulationType::SEIR_simplified) {
			return 0;
		}

		return mortalityRate * recovered;
		// R--
	}

	// All processes.
	const double infectionChance() {
		return infectionRate * susceptible * infected / totalPopulation;

		// if myworkingthread.mode() == Modes.SIR 
		// S--, I++
		// else
		// S--, E++
	}

	// SEIR/SIR process.
	const double deathDueToInfectionChance() {

		if (simulationType == Configuration::SimulationType::SEIR_simplified) {
			return 0;
		}

		return infectedMortalityRate * infected;
		// I--
	}

	// All processes.
	const double recoveryChance() {
		return recoveryRate * infected;
		// I--, R++
	}

	const string findFilename() {
		return string("output_files/output_simulation_") + to_string(id) + ".txt";
	}


private:

	int id;
	static int IDGenerator;

	Configuration::SimulationType simulationType;

	// Event list.
	double vaccinationTimestamp;
	double vaccinationEfficiency;
	double revaccinationEfficiency;
	vector<Event> eventList;

	// Fixed parameters during the simulation.
	double mortalityRate;
	double infectionRate;
	double recoveryRate;
	double infectedMortalityRate;
	double incubationPeriod;

	enum ElementaryEvent {
		BIRTH,
		DEATH_OF_SUSCEPTIBLE,
		SICKNESS,
		DEATH_OF_INFECTED,
		DEATH_OF_RECOVERED,
		INFECTION,
		DEATH_DUE_TO_INFECTION,
		RECOVERY
	};

	static const int ELEMENTARY_EVENT_COUNT = 8;
	double elementaryEventChances[ELEMENTARY_EVENT_COUNT];

	void processOccurred(ElementaryEvent elementaryEvent);


	// Changeable populations during the simulation.
	int totalPopulation;
	int infected;
	int susceptible;
	int recovered;
	int exposed;

	// Tracked data.

	int births = 0;

	int diedS = 0;
	int diedI = 0;
	int diedR = 0;
	int diedDueToI = 0;

	int deathsTotal = 0;

	// Recorded data.
	vector<RecordedData> simulationData;
};

#endif
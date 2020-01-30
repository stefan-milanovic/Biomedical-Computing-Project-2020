#include "SimulationInfo.h"

#include <random>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <limits>
#include <iomanip>

int SimulationInfo::IDGenerator = 0;

SimulationInfo::SimulationInfo(Configuration& config) {

	// Generate ID.
	id = IDGenerator++;

	// Set simulation type.
	this->simulationType = config.getType();

	std::mt19937_64 rng;
	// initialize the random number generator with time-dependent seed
	uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
	rng.seed(ss);

	auto populations = config.getPopulationBoundaries();
	// Initialise the populations.
	for (unsigned i = 0; i < populations.size(); i++) {
		std::uniform_int_distribution<int> unif(populations[i][0], populations[i][1]);
		switch (i) {
		case 0:
			susceptible = unif(rng);
			break;
		case 1:
			exposed = unif(rng);
			break;
		case 2:
			infected = unif(rng);
			break;
		case 3:
			recovered = unif(rng);
			break;
		}
	}
	
	totalPopulation = susceptible + exposed + infected + recovered;

	auto parameters = config.getParameterBoundaries();
	// Initialise the parameters.
	for (unsigned i = 0; i < parameters.size(); i++) {
		std::uniform_real_distribution<double> unif(parameters[i][0], parameters[i][1]);
		switch (i) {
		case 0:
			mortalityRate = unif(rng);
			break;
		case 1:
			infectedMortalityRate = unif(rng);
			break;
		case 2:
			recoveryRate = unif(rng);
			break;
		case 3:
			incubationPeriod = unif(rng);
			break;
		case 4:
			infectionRate = unif(rng);
			break;
		}
	}

	auto events = config.getEvents();
	// Initialise events.
	for (unsigned i = 0; i < events.size(); i++) {
		if (events[i] != false) {
			if (i == 0) {
				eventList.push_back(Event("Vaccination"));
				std::uniform_real_distribution<double> unif(config.getVaccinationTimestampBoundaries()[0], config.getVaccinationTimestampBoundaries()[1]);
				vaccinationTimestamp = unif(rng);
				vaccinationEfficiency = config.getVaccinationEfficiency();
			}
			else {
				eventList.push_back(Event("Revaccination"));
				revaccinationEfficiency = config.getRevaccinationEfficiency();
			}
		}
	}

}

void SimulationInfo::updateProbabilities() {
	
	elementaryEventChances[BIRTH] = birthChance();
	elementaryEventChances[DEATH_OF_SUSCEPTIBLE] = deathOfSusceptibleChance();
	elementaryEventChances[SICKNESS] = sicknessChance();
	elementaryEventChances[DEATH_OF_INFECTED] = deathOfInfectedChance();
	elementaryEventChances[DEATH_OF_RECOVERED] = deathOfRecoveredChance();
	elementaryEventChances[INFECTION] = infectionChance();
	elementaryEventChances[DEATH_DUE_TO_INFECTION] = deathDueToInfectionChance();
	elementaryEventChances[RECOVERY] = recoveryChance();

}

double SimulationInfo::getTimeOfNextEvent() {
	double chancesTotal = 0;
	for (int i = 0; i < ELEMENTARY_EVENT_COUNT; i++) {
		chancesTotal += elementaryEventChances[i];
	}

	// Get next time of event with an exponential random number generator.
	std::default_random_engine generator;
	if (chancesTotal <= 0) {
		chancesTotal++; // ping;
	}
	std::exponential_distribution<double> distribution(chancesTotal);

	return distribution(generator);
}

void SimulationInfo::selectProcess() {
	double chancesTotal = 0;
	for (int i = 0; i < ELEMENTARY_EVENT_COUNT; i++) {
		chancesTotal += elementaryEventChances[i];
	}

	// Grab a random number between 0 and 1.
	std::mt19937_64 rng;
	uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
	rng.seed(ss);
	std::uniform_real_distribution<double> unif(0, 1);

	double rand = unif(rng);

	double linePointer = 0;
	for (int i = 0; i < ELEMENTARY_EVENT_COUNT; i++) {
		linePointer += elementaryEventChances[i] / chancesTotal;
		if (rand <= linePointer) {
			// i-th elementary event has occured.
			if (elementaryEventChances[i] != 0) {
				processOccurred((ElementaryEvent)i);
				break;
			}
			
		}
	}
}

void SimulationInfo::processOccurred(ElementaryEvent elementaryEvent) {
	
	switch (elementaryEvent) {

	case BIRTH:
		susceptible++;
		totalPopulation++;

		births++;
		break;
	case DEATH_OF_SUSCEPTIBLE:
		susceptible--;
		totalPopulation--;

		diedS++;
		deathsTotal++;
		break;
	case SICKNESS:
		exposed--;
		infected++;
		break;
	case DEATH_OF_INFECTED:
		infected--;
		totalPopulation--;

		diedI++;
		deathsTotal++;
		break;
	case DEATH_OF_RECOVERED:
		recovered--;
		totalPopulation--;

		diedR++;
		deathsTotal++;
		break;
	case INFECTION:
		susceptible--;
		if (simulationType == Configuration::SimulationType::SIR) {
			infected++;
		}
		else {
			exposed++;
		}
		break;
	case DEATH_DUE_TO_INFECTION:
		infected--;
		totalPopulation--;

		diedDueToI++;
		deathsTotal++;
		break;
	case RECOVERY:
		infected--;
		recovered++;

		break;
	}
}

void SimulationInfo::checkEvents(double time) {
	for (unsigned i = 0; i < eventList.size(); i++) {
		if (eventList[i].occurred == false) {
			if (eventList[i].eventName == "Vaccination") {
				// Check vaccination condition.
				if (time >= vaccinationTimestamp) {

					int curedByVaccination = (int)(vaccinationEfficiency * susceptible);
					susceptible -= curedByVaccination;
					recovered += curedByVaccination;

					eventList[i].occurred = true;
				}
			}
			else {
				// Check revaccination condition.
				if (i == 0) {
					// No vaccination beforehand. This event's conditions can never be met.
					return;
				}

				if (eventList[0].occurred == true && infected > 0.3 * totalPopulation) {
					int curedByRevaccination = (int)(revaccinationEfficiency * susceptible);

					susceptible -= curedByRevaccination;
					recovered += curedByRevaccination;

					eventList[i].occurred = true;
				}

			}
		}
	}
}

const void SimulationInfo::outputToFile() {

	// Calculate filename.
	string filename = findFilename();

	ofstream cout;

	cout.open(filename);

	cout << "Simulation ID-" << id << endl << "===================" << endl << endl;

	cout << "1) General info " << endl << "-------------------" << endl;
	cout << "Simulation type: " << (simulationType == Configuration::SimulationType::SIR ?
		"SIR" : simulationType == Configuration::SimulationType::SEIR ? "SEIR" : "SEIR_simplified") << endl;
	cout << "Thread ID: " << omp_get_thread_num() << endl << endl;
	// cout << "Thread runtime (s): " << endl << endl;

	cout << "2) Initial populations " << endl << "-------------------" << endl;
	cout << "Susceptible: " << simulationData[0].susceptible << endl;
	if (simulationType != Configuration::SimulationType::SIR) {
		cout << "Exposed: " << simulationData[0].exposed << endl;
	}
	cout << "Infected: " << simulationData[0].infected << endl;
	cout << "Recovered: " << simulationData[0].recovered << endl << endl;

	cout << "3) Initial parameters " << endl << "-------------------" << endl;
	cout << "Mortality rate (m): " << mortalityRate << endl;
	cout << "Infected mortality rate (v): " << infectedMortalityRate << endl;
	cout << "Recovery rate (r): " << recoveryRate << endl;
	cout << "Incubation period (I): " << incubationPeriod << endl;
	cout << "Infection rate (b): " << infectionRate << endl << endl;

	cout << "4) Active events " << endl << "-------------------" << endl;
	for (unsigned i = 0; i < eventList.size(); i++) {
		cout << eventList[i].eventName << endl;
	}

	cout << endl;

	cout << "5) Simulation report " << endl << "-------------------" << endl << endl;

	cout << "| Time  | Susceptible |";
	if (simulationType != Configuration::SimulationType::SIR) {
		cout << " Exposed |";
	}
	cout << " Infected | Recovered | Total Population |";
	
	if (simulationType != Configuration::SimulationType::SEIR_simplified) {
		cout << " ||| | Births | Deaths - Susceptible | Deaths - Infected | Deaths - Recovered | Deaths - Due To Infection | Deaths - Total | " << endl;
	}
	else {
		cout << endl;
	}

	cout.fill(' ');
	for (RecordedData data : simulationData) {
		printData(simulationType, data, cout);
	}

	cout << endl << "6) Simulation preview " << endl << "-------------------" << endl << endl;
	cout << "| Time  | Susceptible |";
	if (simulationType != Configuration::SimulationType::SIR) {
		cout << " Exposed |";
	}
	cout << " Infected | Recovered | Total Population |";

	if (simulationType != Configuration::SimulationType::SEIR_simplified) {
		cout << " ||| | Births | Deaths - Susceptible | Deaths - Infected | Deaths - Recovered | Deaths - Due To Infection | Deaths - Total | " << endl;
	}
	else {
		cout << endl;
	}

	printData(simulationType, simulationData[0], cout);
	printData(simulationType, simulationData[simulationData.size() - 1], cout);

	cout.close();

}

void SimulationInfo::saveIteration(double currentTime) {
	simulationData.push_back(RecordedData(currentTime, susceptible, exposed, infected, recovered, totalPopulation, births, diedS, diedI, diedR, diedDueToI, deathsTotal));
}

void SimulationInfo::printData(Configuration::SimulationType simulationType, RecordedData data, ofstream& cout) {
	cout << "|" << fixed << setw(7) << left << setprecision(3) << data.timestamp << "|";
	cout << setw(13) << left << data.susceptible << "|";
	if (simulationType != Configuration::SimulationType::SIR) {
		cout << setw(9) << left << data.exposed << "|";
	}
	cout << setw(10) << left << data.infected << "|";
	cout << setw(11) << left << data.recovered << "|";

	if (simulationType != Configuration::SimulationType::SEIR_simplified) {
		cout << setw(18) << left << data.total << "| ||| |";
		cout << setw(8) << left << data.births << "|";
		cout << setw(22) << left << data.deathsSuspectible << "|";
		cout << setw(19) << left << data.deathsInfected << "|";
		cout << setw(20) << left << data.deathsRecovered << "|";
		cout << setw(27) << left << data.deathsDueToInfection << "|";
		cout << setw(16) << left << data.deathsTotal << "|" << endl;
	}
	else {
		cout << setw(18) << left << data.total << "|" << endl;
	}
}
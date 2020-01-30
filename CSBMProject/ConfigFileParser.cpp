#include "ConfigFileParser.h"
#include <fstream>

using nlohmann::json;

void ConfigFileParser::parse() {

	json configJson;

	ifstream configFile;

	configFile.open(configFilename);

	configFile >> configJson;

	// Parse type.
	if (configJson["general"]["SimulationType"] == "SIR") {
		config->setType(Configuration::SimulationType::SIR);
	}
	else if (configJson["general"]["SimulationType"] == "SEIR") {
		config->setType(Configuration::SimulationType::SEIR);
	}
	else if (configJson["general"]["SimulationType"] == "SEIR_simplified") {
		config->setType(Configuration::SimulationType::SEIR_simplified);
	}
	else {
		cerr << "ERROR: Invalid simulation type in config file." << endl;
		exit(1);
	}

	// Parse duration.
	config->setMaximumDuration(configJson["general"]["SimulationDuration"]["maximum_duration(days)"]);
	config->setNumberOfSimulations(configJson["general"]["NumberOfSimulations"]);

	// Parse number of threads.
	config->setNumberOfThreads(configJson["general"]["NumberOfThreads"]);

	// Parse populations.

	vector<int> susceptibleBoundaries;
	susceptibleBoundaries.push_back(configJson["populations"]["Susceptible"]["lower_bound"]);
	susceptibleBoundaries.push_back(configJson["populations"]["Susceptible"]["upper_bound"]);
	config->addPopulationBoundary(susceptibleBoundaries);

	vector<int> exposedBoundaries;
	exposedBoundaries.push_back(configJson["populations"]["Exposed"]["lower_bound"]);
	exposedBoundaries.push_back(configJson["populations"]["Exposed"]["upper_bound"]);
	config->addPopulationBoundary(exposedBoundaries);

	vector<int> infectedBoundaries;
	infectedBoundaries.push_back(configJson["populations"]["Infected"]["lower_bound"]);
	infectedBoundaries.push_back(configJson["populations"]["Infected"]["upper_bound"]);
	config->addPopulationBoundary(infectedBoundaries);

	vector<int> recoveredBoundaries;
	recoveredBoundaries.push_back(configJson["populations"]["Recovered"]["lower_bound"]);
	recoveredBoundaries.push_back(configJson["populations"]["Recovered"]["upper_bound"]);
	config->addPopulationBoundary(recoveredBoundaries);

	// Parse parameters.

	vector<double> mortalityRateBoundaries;
	mortalityRateBoundaries.push_back(configJson["parameters"]["MortalityRate (m)"]["lower_bound"]);
	mortalityRateBoundaries.push_back(configJson["parameters"]["MortalityRate (m)"]["upper_bound"]);
	config->addParameterBoundary(mortalityRateBoundaries);

	vector<double> infectedMortalityRateBoundaries;
	infectedMortalityRateBoundaries.push_back(configJson["parameters"]["InfectedMortalityRate (v)"]["lower_bound"]);
	infectedMortalityRateBoundaries.push_back(configJson["parameters"]["InfectedMortalityRate (v)"]["upper_bound"]);
	config->addParameterBoundary(infectedMortalityRateBoundaries);

	vector<double> recoveryRateBoundaries;
	recoveryRateBoundaries.push_back(configJson["parameters"]["RecoveryRate (r)"]["lower_bound"]);
	recoveryRateBoundaries.push_back(configJson["parameters"]["RecoveryRate (r)"]["upper_bound"]);
	config->addParameterBoundary(recoveryRateBoundaries);

	vector<double> incubationPeriodBoundaries;
	incubationPeriodBoundaries.push_back(configJson["parameters"]["IncubationPeriod (I)"]["lower_bound"]);
	incubationPeriodBoundaries.push_back(configJson["parameters"]["IncubationPeriod (I)"]["upper_bound"]);
	config->addParameterBoundary(incubationPeriodBoundaries);

	vector<double> infectionRateBoundaries;
	infectionRateBoundaries.push_back(configJson["parameters"]["InfectionRate (b)"]["lower_bound"]);
	infectionRateBoundaries.push_back(configJson["parameters"]["InfectionRate (b)"]["upper_bound"]);
	config->addParameterBoundary(infectionRateBoundaries);


	// Parse events.
	config->addEvent(configJson["events"]["Vaccination"]["used"]);
	config->addEvent(configJson["events"]["Revaccination"]["used"]);

	config->addVaccinationTimestampBoundary(configJson["events"]["Vaccination"]["timestamp_lower_bound"]);
	config->addVaccinationTimestampBoundary(configJson["events"]["Vaccination"]["timestamp_upper_bound"]);

	config->setVaccinationEfficiency(configJson["events"]["Vaccination"]["vaccination_efficiency"]);
	config->setRevaccinationEfficiency(configJson["events"]["Revaccination"]["revaccination_efficiency"]);

	configFile.close();
}
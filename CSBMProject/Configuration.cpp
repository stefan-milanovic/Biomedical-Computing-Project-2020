#include "Configuration.h"
#include "ConfigFileParser.h"

Configuration::Configuration(string configFilename) {
	ConfigFileParser configFileParser(this, configFilename);
	configFileParser.parse();
}
#ifndef _CONFIGFILEPARSER_H_

#define _CONFIGFILEPARSER_H_

#include <iostream>
#include "json.hpp"
#include "Configuration.h"
#include <unordered_map>

using nlohmann::json;
using namespace std;

class ConfigFileParser {
public:

	ConfigFileParser(Configuration* conf, string filename) : config(conf), configFilename(filename) { }

	void parse();

private:
	
	string configFilename;
	Configuration* config;
};

#endif
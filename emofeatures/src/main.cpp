
#include <string>
#include <iostream>

#include "FeaturesProcessor.h"


const std::string configOpt = "--config=";

void printUsage() {
	std::cerr << "Usage: emofeatures\n"
		"\t[--config=\"<features config xml file>\"]\n"
		"\t<input file>\n"
		"\n"
		"Default values:\n"
		"\t--config=\"featuresconfig.xml\"\n"
		"\n";
}

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}


int exec(const std::string &input, const std::string &configFile);

int main(int argc, char *argv[]) {
	std::string configFile = "coreconfig.xml";
	std::string inputName;

	for (int i = 1; i < argc; ++i) {
		if (testArg(configOpt, argv[i])) {
			configFile.assign(argv[i] + configOpt.length());
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || configFile.empty()) {
		printUsage();
		return 0;
	}

	return exec(inputName, configFile);
}


int exec(const std::string &input, const std::string &configFile) {
	std::string msg;
	FeaturesProcessor processor;

	msg = processor.init(configFile);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	msg = processor.processInput(input);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	return 0;
}


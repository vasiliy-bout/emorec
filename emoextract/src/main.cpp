
#include <string>
#include <iostream>

#include "ExtractProcessor.h"


const std::string classesOpt = "--classes=";
const std::string configOpt = "--config=";

void printUsage() {
	std::cerr << "Usage: emoguess\n"
		"\t[--classes=\"<classes xml file>\"]\n"
		"\t[--config=\"<config xml file>\"]\n"
		"\t<input file>\n"
		"\n"
		"Default values:\n"
		"\t--classes=\"classes.xml\"\n"
		"\t--config=\"extractconfig.xml\"\n"
		"\n";
}

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}


int exec(const std::string &input, const std::string &classesFile, const std::string &configFile);

int main(int argc, char *argv[]) {
	std::string classesFile = "classes.xml";
	std::string configFile = "extractconfig.xml";
	std::string inputName;

	for (int i = 1; i < argc; ++i) {
		if (testArg(classesOpt, argv[i])) {
			classesFile.assign(argv[i] + classesOpt.length());
		} else if (testArg(configOpt, argv[i])) {
			configFile.assign(argv[i] + configOpt.length());
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || classesFile.empty() || configFile.empty()) {
		printUsage();
		return 0;
	}

	return exec(inputName, classesFile, configFile);
}


int exec(const std::string &input, const std::string &classesFile, const std::string &configFile) {
	std::string msg;
	ExtractProcessor processor;

	msg = processor.init(classesFile, configFile);
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


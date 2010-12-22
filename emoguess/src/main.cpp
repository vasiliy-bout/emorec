
#include <string>
#include <iostream>

#include "GuessProcessor.h"


const std::string classesOpt = "--classes=";
const std::string coreConfigOpt = "--core-config=";
const std::string guiConfigOpt = "--gui-config=";

void printUsage() {
	std::cerr << "Usage: emoguess\n"
		"\t[--classes=\"<classes xml file>\"]\n"
		"\t[--core-config=\"<core config xml file>\"]\n"
		"\t[--gui-config=\"<gui config xml file>\"]\n"
		"\t<input file>\n" ;
}

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}


int exec(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile);

int main(int argc, char *argv[]) {
	std::string classesFile = "classes.xml";
	std::string coreConfigFile = "coreconfig.xml";
	std::string guiConfigFile = "guiconfig.xml";
	std::string inputName;

	for (int i = 1; i < argc; ++i) {
		if (testArg(classesOpt, argv[i])) {
			classesFile.assign(argv[i] + classesOpt.length());
		} else if (testArg(coreConfigOpt, argv[i])) {
			coreConfigFile.assign(argv[i] + coreConfigOpt.length());
		} else if (testArg(guiConfigOpt, argv[i])) {
			guiConfigFile.assign(argv[i] + guiConfigOpt.length());
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || classesFile.empty() || coreConfigFile.empty() || guiConfigFile.empty()) {
		printUsage();
		return 0;
	}

	return exec(classesFile, coreConfigFile, guiConfigFile);
}


int exec(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile) {
	std::string msg;

	GuessProcessor processor(classesFile, coreConfigFile, guiConfigFile);

	msg = processor.init();
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	return 0;
}


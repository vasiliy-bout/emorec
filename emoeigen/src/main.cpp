
#include <string>
#include <iostream>

#include "EigenProcessor.h"


const std::string pcaConfigOpt = "--pca-config=";

void printUsage() {
	std::cerr << "Usage: emoeigen\n"
		"\t[--pca-config=\"<pca config xml file>\"]\n"
		"\t<input index file>\n"
		"\t<output pca name>\n"
		"\n"
		"Default values:\n"
		"\t--pca-config=\"pcaconfig.xml\"\n"
		"\n";
}

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}


int exec(const std::string &config, const std::string &input, const std::string &pca);

int main(int argc, char *argv[]) {
	std::string pcaConfigFile = "pcaconfig.xml";
	std::string inputName;
	std::string pcaName;

	for (int i = 1; i < argc; ++i) {
		if (testArg(pcaConfigOpt, argv[i])) {
			pcaConfigFile.assign(argv[i] + pcaConfigOpt.length());
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option " << argv[i] << std::endl;
		} else if (inputName.empty()) {
			inputName.assign(argv[i]);
		} else if (pcaName.empty()) {
			pcaName.assign(argv[i]);
		} else {
			printUsage();
			return 1;
		}
	}

	if (inputName.empty() || pcaName.empty()) {
		printUsage();
		return 0;
	}

	return exec(pcaConfigFile, inputName, pcaName);
}


int exec(const std::string &config, const std::string &input, const std::string &pca) {
	std::string msg;
	EigenProcessor processor;

	msg = processor.init(config);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	msg = processor.processInput(input);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	msg = processor.saveToFile(pca);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	return 0;
}


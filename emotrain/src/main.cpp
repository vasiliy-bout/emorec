
#include <string>
#include <iostream>

#include "TrainProcessor.h"


const std::string classesOpt = "--classes=";
const std::string mlpConfigOpt = "--mlp-config=";
const std::string pcaOpt = "--pca=";

void printUsage() {
	std::cerr << "Usage: emotrain\n"
		"\t[--classes=\"<classes xml file>\"]\n"
		"\t[--mlp-config=\"<mlp config xml file>\"]\n"
		"\t--pca=\"<pca xml file>\"\n"
		"\t<input index file>\n"
		"\t<output mlp file>\n"
		"\n"
		"Default values:\n"
		"\t--classes=\"classes.xml\"\n"
		"\t--mlp-config=\"mlpconfig.xml\"\n"
		"\n";
}

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}


int exec(const std::string &input, const std::string &classesFile,
	const std::string &mlpConfigFile, const std::string &pcaFile, const std::string &mlpName);

int main(int argc, char *argv[]) {
	std::string classesFile = "classes.xml";
	std::string mlpConfigFile = "mlpconfig.xml";
	std::string pcaFile;
	std::string inputName;
	std::string mlpName;

	for (int i = 1; i < argc; ++i) {
		if (testArg(classesOpt, argv[i])) {
			classesFile.assign(argv[i] + classesOpt.length());
		} else if (testArg(mlpConfigOpt, argv[i])) {
			mlpConfigFile.assign(argv[i] + mlpConfigOpt.length());
		} else if (testArg(pcaOpt, argv[i])) {
			pcaFile.assign(argv[i] + pcaOpt.length());
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option " << argv[i] << std::endl;
		} else if (inputName.empty()) {
			inputName.assign(argv[i]);
		} else if (mlpName.empty()) {
			mlpName.assign(argv[i]);
		} else {
			printUsage();
			return 1;
		}
	}

	if (inputName.empty() || mlpName.empty() || pcaFile.empty()) {
		printUsage();
		return 0;
	}

	return exec(inputName, classesFile, mlpConfigFile, pcaFile, mlpName);
}


int exec(const std::string &input, const std::string &classesFile,
		const std::string &mlpConfigFile, const std::string &pcaFile, const std::string &mlpName) {
	std::string msg;
	TrainProcessor processor;

	msg = processor.init(mlpConfigFile, classesFile, pcaFile);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	msg = processor.processInput(input);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	msg = processor.saveToFile(mlpName);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	return 0;
}


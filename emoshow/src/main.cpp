
#include <string>
#include <iostream>

#include "ShowProcessor.h"


void printUsage() {
	std::cerr << "Usage: emoshow\n"
		"\t<input pca file>\n"
		"\n";
}

/*static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}*/


int exec(const std::string &pca);

int main(int argc, char *argv[]) {
	std::string pcaFile;

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option " << argv[i] << std::endl;
		} else if (pcaFile.empty()) {
			pcaFile.assign(argv[i]);
		} else {
			printUsage();
			return 1;
		}
	}

	if (pcaFile.empty()) {
		printUsage();
		return 0;
	}

	return exec(pcaFile);
}


int exec(const std::string &pca) {
	std::string msg;
	ShowProcessor processor;

	msg = processor.init(pca);
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	msg = processor.exec();
	if (!msg.empty()) {
		std::cerr << "ERROR: " << msg << std::endl;
		return 1;
	}

	return 0;
}


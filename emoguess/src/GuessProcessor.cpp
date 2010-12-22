
#include "GuessProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoClassesReader.h>

#include <EmoClassesReader.h>


GuessProcessor::GuessProcessor() {
}

std::string GuessProcessor::init(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile) {

	myCore = EmoCore::create();

	std::map<std::string, std::string> coreConfig;
	//std::map<std::string, std::string> guiConfig;

	if (!EmoConfigReader::loadConfig(coreConfigFile, coreConfig)) {
		return "Unable to load core config";
	}
	/*if (!EmoConfigReader::loadConfig(guiConfigFile, guiConfig)) {
		return "Unable to load GUI config";
	}*/

	if (!EmoClassesReader::loadClasses(classesFile, myClasses)) {
		return "Unable load classes";
	}

	std::vector<unsigned char> letters;
	for (size_t i = 0; i < myClasses.size(); ++i) {
		letters.push_back(myClasses[i].letter());
	}

	int err = myCore->init(letters, coreConfig);
	if (err != EMOERR_OK) {
		return myCore->errorMessage(err);
	}

	return "";
}


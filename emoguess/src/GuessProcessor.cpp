
#include "GuessProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoClassesReader.h>

#include <EmoClassesReader.h>


GuessProcessor::GuessProcessor(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile) :
		myClassesFile(classesFile), myCoreConfigFile(coreConfigFile), myGuiConfigFile(guiConfigFile) {
}

std::string GuessProcessor::init() {

	myCore = EmoCore::create();

	std::map<std::string, std::string> coreConfig;
	//std::map<std::string, std::string> guiConfig;

	if (!EmoConfigReader::loadConfig(myCoreConfigFile, coreConfig)) {
		return "Unable to load core config";
	}
	/*if (!EmoConfigReader::loadConfig(myGuiConfigFile, guiConfig)) {
		return "Unable to load GUI config";
	}*/

	if (!EmoClassesReader::loadClasses(myClassesFile, myClasses)) {
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


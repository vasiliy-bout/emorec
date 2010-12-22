
#ifndef __GUESSPROCESSOR_H__
#define __GUESSPROCESSOR_H__

#include <string>
#include <vector>

#include <EmoCore.h>
#include <EmoClass.h>


class GuessProcessor {

public:
	GuessProcessor(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile);

	std::string init();

private:
	const std::string myClassesFile;
	const std::string myCoreConfigFile;
	const std::string myGuiConfigFile;

	shared_ptr<EmoCore> myCore;
	std::vector<EmoClass> myClasses;
};

#endif /* __GUESSPROCESSOR_H__ */

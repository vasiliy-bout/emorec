
#ifndef __GUESSPROCESSOR_H__
#define __GUESSPROCESSOR_H__

#include <string>
#include <vector>

#include <EmoCore.h>
#include <EmoClass.h>


class GuessProcessor {

public:
	GuessProcessor();

	std::string init(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile);

	//std::string processInput(const std::string &input);

private:
	shared_ptr<EmoCore> myCore;
	std::vector<EmoClass> myClasses;
};

#endif /* __GUESSPROCESSOR_H__ */

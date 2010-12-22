
#ifndef __GUESSPROCESSOR_H__
#define __GUESSPROCESSOR_H__

#include <string>
#include <vector>
#include <map>

#include <EmoCore.h>
#include <EmoClass.h>


class GuessProcessor {

public:
	GuessProcessor();

	std::string init(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile);

	std::string processInput(const std::string &input);

private:
	int readConfig(const std::map<std::string, std::string> &config);

	std::string processImage(const cv::Mat &image);
	void displayResults(const cv::Mat &image, const cv::Rect &faceRect, const std::map<unsigned char, float> &results);
	int computeVerdict(const std::map<unsigned char, float> &results);

private:
	shared_ptr<EmoCore> myCore;
	std::vector<EmoClass> myClasses;
	float myThreshold;
};

#endif /* __GUESSPROCESSOR_H__ */

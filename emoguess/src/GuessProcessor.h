
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

	std::string processImage(const cv::Mat &image, char answer = '\0');
	void displayResults(const cv::Mat &image, const cv::Rect &faceRect, const std::map<unsigned char, float> &results, char answer);
	int computeVerdict(const std::map<unsigned char, float> &results);

	bool tryProcessVideo(const std::string &input);
	bool tryProcessIndex(const std::string &input);

	static char category(const std::string &fileName);

private:
	shared_ptr<EmoCore> myCore;
	std::vector<EmoClass> myClasses;
	float myThreshold;

	int myTotalFaces;
	int myPositiveFaces;
};

#endif /* __GUESSPROCESSOR_H__ */


#ifndef __GUESSPROCESSOR_H__
#define __GUESSPROCESSOR_H__

#include <string>
#include <vector>
#include <map>

#include <EmoCore.h>
#include <EmoClass.h>


class EigenProcessor {

public:
	EigenProcessor();

	std::string init(const std::string &config);
	std::string processInput(const std::string &input);
	std::string saveToFile(const std::string &pca);

private:
	int readConfig(const std::map<std::string, std::string> &config);

	bool fillSamples(const std::string &input, cv::Mat &samples);

private:
	int myFaceWidth;
	int myFaceHeight;
	int myMaxComponents;
	cv::PCA myCvPCA;
};

#endif /* __GUESSPROCESSOR_H__ */

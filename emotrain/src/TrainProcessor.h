
#ifndef __TrainProcessor_H__
#define __TrainProcessor_H__

#include <string>
#include <vector>
#include <map>

#include <EmoCore.h>
#include <EmoClass.h>


class TrainProcessor {

public:
	TrainProcessor();

	std::string init(const std::string &configFile, const std::string &classesFile, const std::string &pcaFile);
	std::string processInput(const std::string &input);
	std::string saveToFile(const std::string &mlp);

private:
	int readConfig(const std::map<std::string, std::string> &config);

	static char category(const std::string &fileName);

	bool fillSamples(const std::string &inputName, cv::Mat &samples, cv::Mat &categories);

private:
	std::string myClassesList;
	float myFeatures;
	cv::PCA myCvPCA;
	cv::Size myFaceSize;

	float myLayersScale;
	int myLayersCount;

	CvANN_MLP myCvMLP;
};

#endif /* __TrainProcessor_H__ */

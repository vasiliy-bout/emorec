
#ifndef __ExtractProcessor_H__
#define __ExtractProcessor_H__

#include <string>
#include <vector>
#include <map>

#include <EmoCore.h>
#include <EmoClass.h>


class ExtractProcessor {

public:
	ExtractProcessor();

	std::string init(const std::string &classesFile, const std::string &configFile);

	std::string processInput(const std::string &input);

private:
	int readConfig(const std::map<std::string, std::string> &config);

	bool processIndex(const std::string &input);

	static std::string baseName(const std::string &fileName);

	bool processImage(const cv::Mat &image, const std::string &base);

	bool processFace(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number);

	char readClass();

	int writeImage(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number, int morphnumber, char letter);

	int doScale(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number, int morphnumber, char letter);
	int doOffset(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number, int morphnumber, char letter);

private:
	int myMorphOffset;
	int myMorphOffsetStep;
	int myMorphScale;
	int myMorphScaleStep;

	float myStartScale;

	std::vector<EmoClass> myClasses;

	cv::CascadeClassifier myCvCascade;
	float myScale;
	float myMinFace;

	std::string myOutFolder;
	cv::Size myOutSize;
};

#endif /* __ExtractProcessor_H__ */

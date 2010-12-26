
#ifndef __FEATURESPROCESSOR_H__
#define __FEATURESPROCESSOR_H__

#include <string>
#include <vector>
#include <map>

#include <EmoCore.h>
#include <EmoClass.h>


class FeaturesProcessor {

public:
	FeaturesProcessor();

	std::string init(const std::string &configFile);

	std::string processInput(const std::string &input);

private:
	int readConfig(const std::map<std::string, std::string> &config);

	bool tryProcessVideo(const std::string &input);
	bool tryProcessIndex(const std::string &input);

	std::string processImage(const cv::Mat &image);

	void convertFace(const cv::Mat &face, cv::Mat &converted) const;

	static void drawKeyPoints(const cv::Mat &image, const std::vector<cv::KeyPoint> &points, cv::Mat &canvas);
	static void drawPoints(const cv::Mat &image, const std::vector<cv::Point> &points, cv::Mat &canvas);
	static void drawPointsVector(const cv::Mat &image, const std::vector<std::vector<cv::Point> > &vpoints, cv::Mat &canvas);

	static void showKeyPoints(const cv::Mat &image, const std::vector<cv::KeyPoint> &points);

private:
	int myFaceSize;
};

#endif /* __FEATURESPROCESSOR_H__ */

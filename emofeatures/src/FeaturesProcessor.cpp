
#include <iostream>
#include <stdio.h>

#include "FeaturesProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoClassesReader.h>

#include <EmoClassesReader.h>


FeaturesProcessor::FeaturesProcessor() {
	myFaceSize = 100;
}

std::string FeaturesProcessor::init(const std::string &configFile) {
	std::map<std::string, std::string> config;

	if (!EmoConfigReader::loadConfig(configFile, config)) {
		return "Unable to load config";
	}

	int err = readConfig(config);
	if (err != EMOERR_OK) {
		return EmoCore::errorMessage(err);
	}

	return "";
}


//static const std::string PARAM_THRESHOLD = "threshold";

int FeaturesProcessor::readConfig(const std::map<std::string, std::string> &config) {
	/*std::map<std::string, std::string>::const_iterator thresholdIter = config.find(PARAM_THRESHOLD);

	if (thresholdIter == config.end()) {
		return EMOERR_NOPARAMETERS;
	}

	if (sscanf(thresholdIter->second.c_str(), "%f", &myThreshold) != 1 || myThreshold < 0.0f || myThreshold >= 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}*/

	return EMOERR_OK;
}


static const std::string imageWindow = "imageWindow";
static const std::string pointsWindow = "pointsWindow";

std::string FeaturesProcessor::processInput(const std::string &input) {
	std::string msg;
	cv::namedWindow(imageWindow, 1);
	cv::namedWindow(pointsWindow, 1);

	cv::Mat image = cv::imread(input, 1);

	if (!image.empty()) {
		std::cout << "Process image \"" << input << "\"" << std::endl;
		msg = processImage(image);
	} else if (!tryProcessVideo(input)
			&& !tryProcessIndex(input)) {
		msg = "Unable to handle input file";
	}

	std::cerr << "Press any key to quit..." << msg << std::endl;
	cv::waitKey();

	cvDestroyWindow(imageWindow.c_str());
	cvDestroyWindow(pointsWindow.c_str());

	return msg;
}


bool FeaturesProcessor::tryProcessVideo(const std::string &input) {
	cv::VideoCapture capture;
	if (input.length() == 1 && input[0] >= '0' && input[0] <= '9') {
		int index = input[0] - '0';
		capture.open(index);
	} else {
		capture.open(input);
	}
	if (!capture.isOpened()) {
		return false;
	}

	std::cout << "Processing capture \"" << input << "\"" << std::endl;
	std::cout << "Press `q` to quit..." << std::endl;

	std::string msg;
	cv::Mat frame;
	char ch = 0;
	int skipFrames = 0;
	double fps = capture.get(CV_CAP_PROP_FPS);
	if (fps > 1.0) {
		skipFrames = (fps - 1.0);
	}

	std::cout << "DBG: fps = " << fps << "; skip = " << skipFrames << std::endl;

	while (ch != 'q' && ch != 'Q' && capture.grab()) {
		capture.retrieve(frame);
		msg = processImage(frame);
		if (!msg.empty()) {
			std::cerr << "ERROR: " << msg << std::endl;
			return false;
		}
		ch = cv::waitKey(1000);
		for (int i = 0; i < skipFrames; ++i) {
			if (!capture.grab()) {
				return true;
			}
		}
	}
	return true;
}

bool FeaturesProcessor::tryProcessIndex(const std::string &input) {
	FILE* f = fopen(input.c_str(), "rt");
	if (f == 0) {
		return false;
	}

	std::cout << "Processing index file \"" << input << "\"" << std::endl;
	std::cout << "Press `q` to quit..." << std::endl;

	std::string msg;
	cv::Mat frame;
	char ch = 0;

	char buf[1000];
	while (fgets(buf, 1000, f)) {
		int len = (int) strlen(buf);
		while (len > 0 && isspace(buf[len-1])) {
			len--;
		}
		buf[len] = '\0';
		std::string imageName(buf);

		std::cout << "file " << imageName.c_str() << std::endl;
		frame = cv::imread(imageName, 1);
		if (!frame.empty()) {
			msg = processImage(frame);
		} else {
			std::cout << "WARNING: unable to read file" << std::endl;
			std::cout << std::endl;
		}
		ch = cv::waitKey();
		if (ch == 'q' || ch == 'Q') {
			break;
		}
	}

	fclose(f);
	return true;
}


void FeaturesProcessor::convertFace(const cv::Mat &face, cv::Mat &converted) const {
	cv::Mat gray, resized;
	cv::cvtColor(face, gray, CV_BGR2GRAY);

	//cv::resize(gray, resized, cv::Size(myFaceSize, myFaceSize), 0, 0, cv::INTER_LINEAR);
	gray.copyTo(resized);

	resized.copyTo(converted);

	//cv::equalizeHist(resized, converted);
	//resized.convertTo(converted, CV_32FC1, 1.0 / 255);
}


std::string FeaturesProcessor::processImage(const cv::Mat &image) {
	cv::imshow(imageWindow, image);
	cv::imshow(pointsWindow, image);

	double timer = (double)cv::getTickCount();

	std::vector<cv::KeyPoint> keyPoints;
	//std::vector<cv::Point> points;
	//std::vector<std::vector<cv::Point> > vpoints;

	cv::Mat face;
	convertFace(image, face);

	//cv::StarDetector detector;
	//detector(face, keyPoints);

	//cv::MSER detector;
	//cv::Mat mask;
	//detector(face, vpoints, mask);

	//cv::SURF detector(0.05);
	//cv::Mat mask;
	//detector(face, mask, keyPoints);

	timer = (double)cv::getTickCount() - timer;
	std::cout << "Detection time = " << (timer / cv::getTickFrequency()) << " s" << std::endl;
	std::cout << std::endl;

	cv::Mat canvas;

	//cvDrawKeypoints(face, keyPoints, canvas, CV_RGB(255, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//drawKeyPoints(face, keyPoints, canvas);
	//drawPointsVector(face, vpoints, canvas);
	//cv::imshow(pointsWindow, canvas);

	//showKeyPoints(face, keyPoints);

	/*for (size_t v = 0; v < vpoints.size(); ++v) {
		cv::cvtColor(face, canvas, CV_GRAY2BGR);

		std::cerr << "CONTOURS NUMBER = " << vpoints.size() << std::endl;

		cv::drawContours(canvas, vpoints, v, CV_RGB(255, 0, 0), 1, 8, std::vector<cv::Vec4i>(), 0);

		cv::imshow(pointsWindow, canvas);

		char ch = cv::waitKey();
		if (ch == 27) break;
	}*/

	return "";
}

void FeaturesProcessor::drawKeyPoints(const cv::Mat &image, const std::vector<cv::KeyPoint> &points, cv::Mat &canvas) {
	cv::cvtColor(image, canvas, CV_GRAY2BGR);

	std::cerr << "POINTS NUMBER = " << points.size() << std::endl;

	for (size_t i = 0; i < points.size(); ++i) {
		const cv::KeyPoint &point = points[i];

		std::cerr << "POINT (" << i << "): (" << point.pt.x << ", " << point.pt.y << ") " << point.size << " / " << point.angle << " -- " << point.response << std::endl;

		cv::circle(canvas, point.pt, std::max(1, (int)point.size), CV_RGB(255, 0, 0));

		if (point.angle >= 0.f) {
			float length = std::max(5.f, point.response / 10.f);
			float dx = length * cos(point.angle);
			float dy = length * sin(point.angle);

			cv::line(canvas, point.pt, point.pt + cv::Point2f(dx, dy), CV_RGB(0, 255, 0));
		}
	}
}

void FeaturesProcessor::drawPoints(const cv::Mat &image, const std::vector<cv::Point> &points, cv::Mat &canvas) {
	cv::cvtColor(image, canvas, CV_GRAY2BGR);

	std::cerr << "POINTS NUMBER = " << points.size() << std::endl;

	for (size_t i = 0; i < points.size(); ++i) {
		const cv::Point &point = points[i];

		std::cerr << "POINT (" << i << "): (" << point.x << ", " << point.y << ") " << std::endl;

		cv::circle(canvas, point, 1, CV_RGB(255, 0, 0));
	}
}

void FeaturesProcessor::drawPointsVector(const cv::Mat &image, const std::vector<std::vector<cv::Point> > &vpoints, cv::Mat &canvas) {
	cv::cvtColor(image, canvas, CV_GRAY2BGR);

	std::cerr << "CONTOURS NUMBER = " << vpoints.size() << std::endl;

	cv::drawContours(canvas, vpoints, -1, CV_RGB(255, 0, 0));
}



void FeaturesProcessor::showKeyPoints(const cv::Mat &image, const std::vector<cv::KeyPoint> &points) {
	std::cerr << "POINTS NUMBER = " << points.size() << std::endl;

	cv::Mat canvas;

	for (size_t i = 0; i < points.size(); ++i) {

		cv::cvtColor(image, canvas, CV_GRAY2BGR);

		const cv::KeyPoint &point = points[i];

		std::cerr << "POINT (" << i << "): (" << point.pt.x << ", " << point.pt.y << ") " << point.size << " / " << point.angle << " -- " << point.response << " | " << point.octave << " : " << point.class_id << std::endl;

		cv::circle(canvas, point.pt, std::max(1, (int)point.size), CV_RGB(255, 0, 0));

		if (point.angle >= 0.f) {
			float length = std::max(5.f, point.response / 10.f);
			float dx = length * cos(point.angle);
			float dy = length * sin(point.angle);

			cv::line(canvas, point.pt, point.pt + cv::Point2f(dx, dy), CV_RGB(0, 255, 0));
		}

		cv::imshow(pointsWindow, canvas);

		char ch = cv::waitKey();
		if (ch == 27) break;
	}
}


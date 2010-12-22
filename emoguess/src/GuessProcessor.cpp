
#include <iostream>
#include <stdio.h>

#include "GuessProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoClassesReader.h>

#include <EmoClassesReader.h>


GuessProcessor::GuessProcessor() {
}

std::string GuessProcessor::init(const std::string &classesFile, const std::string &coreConfigFile, const std::string &guiConfigFile) {

	myCore = EmoCore::create();

	std::map<std::string, std::string> coreConfig;
	std::map<std::string, std::string> guiConfig;

	if (!EmoConfigReader::loadConfig(coreConfigFile, coreConfig)) {
		return "Unable to load core config";
	}
	if (!EmoConfigReader::loadConfig(guiConfigFile, guiConfig)) {
		return "Unable to load GUI config";
	}
	if (!EmoClassesReader::loadClasses(classesFile, myClasses)) {
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

	err = readConfig(guiConfig);
	if (err != EMOERR_OK) {
		return myCore->errorMessage(err);
	}

	return "";
}


static const std::string PARAM_THRESHOLD = "threshold";

int GuessProcessor::readConfig(const std::map<std::string, std::string> &config) {
	std::map<std::string, std::string>::const_iterator thresholdIter = config.find(PARAM_THRESHOLD);

	if (thresholdIter == config.end()) {
		return EMOERR_NOPARAMETERS;
	}

	if (sscanf(thresholdIter->second.c_str(), "%f", &myThreshold) != 1 || myThreshold < 0.0f || myThreshold >= 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}

	return EMOERR_OK;
}


static const std::string imageWindow = "imageWindow";

std::string GuessProcessor::processInput(const std::string &input) {
	std::string msg;
	cvNamedWindow(imageWindow.c_str(), 1);

	cv::Mat image = cv::imread(input, 1);

	if (!image.empty()) {
		std::cout << "Process image \"" << input << "\"" << std::endl;
		msg = processImage(image);
	} else {
		msg = "Unable to handle input file";
	}

	cvDestroyWindow(imageWindow.c_str());
	return msg;
}

std::string GuessProcessor::processImage(const cv::Mat &image) {
	cv::Rect faceRect;
	int err;

	cv::imshow(imageWindow, image);

	double timer = (double)cv::getTickCount();

	err = myCore->extractFace(image, faceRect);
	if (err != EMOERR_OK) {
		return myCore->errorMessage(err);
	}

	if (faceRect.width == 0 || faceRect.height == 0) {
		return "Unable to find face";
	}

	cv::Mat face = image(faceRect);

	std::map<unsigned char, float> results;

	err = myCore->guess(face, results);
	if (err != EMOERR_OK) {
		return myCore->errorMessage(err);
	}

	timer = (double)cv::getTickCount() - timer;
	std::cout << "Detection time = " << (timer / cv::getTickFrequency()) << " s" << std::endl;

	if (results.empty()) {
		return "Unable to get results";
	}

	displayResults(image, faceRect, results);

	std::cout << std::endl;
	return "";
}


const static cv::Scalar colors[] =  {
	CV_RGB(0,0,255),
	CV_RGB(0,128,255),
	CV_RGB(0,255,255),
	CV_RGB(0,255,0),
	CV_RGB(255,128,0),
	CV_RGB(255,255,0),
	CV_RGB(255,0,0),
	CV_RGB(255,0,255)
};

void GuessProcessor::displayResults(const cv::Mat &image, const cv::Rect &faceRect, const std::map<unsigned char, float> &results) {
	cv::Mat canvas;
	image.copyTo(canvas);

	const cv::Scalar &color = colors[cv::getTickCount() % 8];
	cv::rectangle(canvas, cvPoint(faceRect.x, faceRect.y), cvPoint(faceRect.x + faceRect.width, faceRect.y + faceRect.height), color, 3);
	cv::imshow(imageWindow, canvas);

	std::cout << std::endl;

	for (size_t i = 0; i < myClasses.size(); ++i) {
		const EmoClass &cls = myClasses[i];
		std::map<unsigned char, float>::const_iterator it = results.find(cls.letter());
		if (it == results.end()) {
			std::cout << "Result: " << "unknown" << "\tfor \"" << cls.name() << "\"" << std::endl;
		} else {
			const float prob = it->second;
			std::cout << "Result: " << prob << "\tfor \"" << cls.name() << "\"" << std::endl;
		}
	}
	std::cout << std::endl;

	int verdict = computeVerdict(results);

	if (verdict < 0) {
		verdict = -verdict - 1;
		std::cout << "Prediction is too unreliable... " << std::endl;
	}
	std::cout << "VERDICT:\t\"" << myClasses[verdict].name().c_str() << "\"" << std::endl;
	std::cout << "DONE" << std::endl;
	std::cout << std::endl;
}

/**
 * If verdict is relyable, returns class index;
 * If verdict is unreliable, returns (-index - 1), where index is most probable answer.
 */
int GuessProcessor::computeVerdict(const std::map<unsigned char, float> &results) {
	int defaultClass = -1;
	int verdict = -1;
	float verdictProb = 0.0f;

	for (size_t i = 0; i < myClasses.size(); ++i) {
		const EmoClass &cls = myClasses[i];
		if (cls.isDefault()) {
			defaultClass = i;
		}
		std::map<unsigned char, float>::const_iterator it = results.find(cls.letter());
		if (it != results.end()) {
			const float prob = it->second;
			if (verdict == -1 || prob > verdictProb || (prob == verdictProb && cls.isDefault())) {
				verdict = i;
				verdictProb = prob;
			}
		}
	}

	if (verdict == -1) {
		return -defaultClass - 1;
	}

	if (verdictProb < myThreshold) {
		return -defaultClass - 1;
		//return -verdict - 1;
	}
	return verdict;
}



#include <stdio.h>

#include "EmoCoreImpl.h"

#include "EmoPCAReader.h"
#include "EmoErrors.h"



static const std::string PARAM_CASCADE = "cascade";
static const std::string PARAM_MLP = "mlp";

static const std::string PARAM_WINDOW_SIZE = "window-size";
static const std::string PARAM_FACE_WIDTH = "face-width";
static const std::string PARAM_FACE_HEIGHT = "face-height";

static const std::string PARAM_SCALE = "scale";
static const std::string PARAM_MIN_FACE = "minFacePercent";


EmoCoreImpl::EmoCoreImpl() : myInitialized(false) {
}

int EmoCoreImpl::init(std::vector<unsigned char> &classes, const std::map<std::string, std::string> &parameters) {
	if (classes.empty()) {
		return EMOERR_INVALID_CLASSES;
	}
	myClasses.assign(classes.begin(), classes.end());

	std::map<std::string, std::string>::const_iterator cascadeIter = parameters.find(PARAM_CASCADE);
	std::map<std::string, std::string>::const_iterator mlpIter = parameters.find(PARAM_MLP);

	std::map<std::string, std::string>::const_iterator windowSizeIter = parameters.find(PARAM_WINDOW_SIZE);
	std::map<std::string, std::string>::const_iterator faceWidthIter = parameters.find(PARAM_FACE_WIDTH);
	std::map<std::string, std::string>::const_iterator faceHeightIter = parameters.find(PARAM_FACE_HEIGHT);

	std::map<std::string, std::string>::const_iterator scaleIter = parameters.find(PARAM_SCALE);
	std::map<std::string, std::string>::const_iterator minFaceIter = parameters.find(PARAM_MIN_FACE);

	if (cascadeIter == parameters.end() ||
			mlpIter == parameters.end() ||
			faceWidthIter == parameters.end() ||
			faceHeightIter == parameters.end() ||
			windowSizeIter == parameters.end() ||
			scaleIter == parameters.end() ||
			minFaceIter == parameters.end() ||
			myClasses.empty()) {
		return EMOERR_NOPARAMETERS;
	}

	const std::string &cascade = cascadeIter->second;
	const std::string &mlp = mlpIter->second;

	if (sscanf(faceWidthIter->second.c_str(), "%d", &myFaceSize.width) != 1 || myFaceSize.width <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(faceHeightIter->second.c_str(), "%d", &myFaceSize.height) != 1 || myFaceSize.height <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}

	int windowSize;
	if (sscanf(windowSizeIter->second.c_str(), "%d", &windowSize) != 1 || windowSize <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}

	if (sscanf(scaleIter->second.c_str(), "%f", &myScale) != 1 || myScale <= 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(minFaceIter->second.c_str(), "%f", &myMinFace) != 1 || myMinFace < 0.0f || myMinFace > 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}


	if (!myCvCascade.load(cascade)) {
		return EMOERR_CANT_LOAD_CLASSIFIER;
	}

	myFeatures = new EmoFeatures(windowSize);

	myCvMLP.load(mlp.c_str());
	if (myCvMLP.get_layer_count() == 0) {
		return EMOERR_CANT_LOAD_MLP;
	}
	const CvMat *sizes = myCvMLP.get_layer_sizes();
	if (sizes == 0) {
		return EMOERR_INTERNAL_ERROR;
	}
	cv::Mat sizesMat(sizes);
	if (sizesMat.type() != CV_32SC1 || sizesMat.rows != 1 || sizesMat.cols < 2) {
		return EMOERR_INTERNAL_ERROR;
	}
	if (sizesMat.at<int>(0, 0) != myFeatures->featuresNumber(myFaceSize)) {
		return EMOERR_INVALID_MLP_OR_FEATURES;
	}
	if (sizesMat.at<int>(0, sizesMat.cols - 1) != (int)myClasses.size()) {
		return EMOERR_INVALID_MLP_OR_CLASSES;
	}

	myInitialized = true;
	return 0;
}

int EmoCoreImpl::extractFace(const cv::Mat &img, cv::Rect &face) {
	if (!myInitialized) {
		return EMOERR_NOT_INITIALIZED;
	}
	cv::Mat buffer;
	cv::cvtColor(img, buffer, CV_BGR2GRAY);
	cv::equalizeHist(buffer, buffer);

	std::vector<cv::Rect> faces;
	myCvCascade.detectMultiScale(buffer, faces, myScale, 2,
		CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT,
		cv::Size(buffer.cols * myMinFace, buffer.rows * myMinFace));

	if (faces.empty()) {
		face = cv::Rect(0, 0, 0, 0);
	} else {
		face = faces[0];
	}

	return 0;
}



void EmoCoreImpl::convertFace(const cv::Mat &face, cv::Mat &converted) const {
	cv::Mat gray, resized;
	cv::cvtColor(face, gray, CV_BGR2GRAY);
	cv::resize(gray, resized, myFaceSize, 0, 0, cv::INTER_LINEAR);
	cv::equalizeHist(resized, resized);
	resized.convertTo(converted, CV_32FC1, 1.0 / 255);
}


int EmoCoreImpl::guess(const cv::Mat &face, std::map<unsigned char, float> &results) {
	if (!myInitialized) {
		return EMOERR_NOT_INITIALIZED;
	}

	cv::Mat converted;
	convertFace(face, converted);

	cv::Mat features;
	myFeatures->project(converted, features);

	cv::Mat output;
	myCvMLP.predict(features, output);

	if (output.rows != 1) {
		return EMOERR_INTERNAL_ERROR;
	}
	if (output.cols != (int)myClasses.size()) {
		return EMOERR_INVALID_MLP_OR_CLASSES;
	}

	float maxValue = 0.0f;
	for (int j = 0; j < output.cols; ++j) {
		float &prob = output.at<float>(0, j);
		if (prob < 0.0f) {
			prob = 0.0f;
		} else if (prob > 1.0f) {
			maxValue = prob;
		}
	}

	if (maxValue > 1.0f) {
		std::cerr << "maxValue = " << maxValue << std::endl;
		output /= maxValue;
	}

	results.clear();
	for (int j = 0; j < output.cols; ++j) {
		const float prob = output.at<float>(0, j);
		const unsigned char letter = myClasses[j];
		results[letter] = prob;
	}
	return 0;
}


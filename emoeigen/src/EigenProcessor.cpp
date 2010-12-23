
#include <iostream>
#include <stdio.h>

#include "EigenProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoPCAWriter.h>


EigenProcessor::EigenProcessor() {
}

std::string EigenProcessor::init(const std::string &config) {
	std::map<std::string, std::string> parameters;

	if (!EmoConfigReader::loadConfig(config, parameters)) {
		return "Unable to load config";
	}

	int err = readConfig(parameters);
	if (err != EMOERR_OK) {
		return EmoCore::errorMessage(err);
	}

	return "";
}


static const std::string PARAM_FACE_WIDTH = "face-width";
static const std::string PARAM_FACE_HEIGHT = "face-height";
static const std::string PARAM_MAX_COMPONENTS = "max-components";

int EigenProcessor::readConfig(const std::map<std::string, std::string> &config) {
	std::map<std::string, std::string>::const_iterator widthIter = config.find(PARAM_FACE_WIDTH);
	std::map<std::string, std::string>::const_iterator heightIter = config.find(PARAM_FACE_HEIGHT);
	std::map<std::string, std::string>::const_iterator maxComponentsIter = config.find(PARAM_MAX_COMPONENTS);

	if (widthIter == config.end()
			|| heightIter == config.end()) {
		return EMOERR_NOPARAMETERS;
	}

	if (sscanf(widthIter->second.c_str(), "%d", &myFaceWidth) != 1 || myFaceWidth <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(heightIter->second.c_str(), "%d", &myFaceHeight) != 1 || myFaceHeight <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (maxComponentsIter == config.end()
			|| sscanf(maxComponentsIter->second.c_str(), "%d", &myMaxComponents) != 1 || myMaxComponents < 0) {
		myMaxComponents = 0;
	}

	return EMOERR_OK;
}


std::string EigenProcessor::processInput(const std::string &input) {
	cv::Mat samples;
	if (!fillSamples(input, samples)) {
		return "Unable to read samples";
	}
	myCvPCA(samples, cv::Mat(), CV_PCA_DATA_AS_ROW, myMaxComponents);
	return "";
}

bool EigenProcessor::fillSamples(const std::string &input, cv::Mat &samples) {
	FILE* f = fopen(input.c_str(), "rt");
	if (f == 0) {
		return false;
	}

	cv::Mat image, gray, resized, resizedFloat;

	std::vector<cv::Mat> imagesVector;

	char buf[1000];
	while (fgets(buf, 1000, f)) {
		int len = (int) strlen(buf);
		while (len > 0 && isspace(buf[len-1])) {
			len--;
		}
		buf[len] = '\0';
		std::string imageName(buf);

		std::cout << "file " << imageName.c_str() << std::endl;
		image = cv::imread(imageName, 1);
		if (!image.empty()) {
			cv::cvtColor(image, gray, CV_BGR2GRAY);
			cv::resize(gray, resized, cv::Size(myFaceWidth, myFaceHeight), 0, 0, cv::INTER_LINEAR);
			cv::equalizeHist(resized, resized);

			resized.convertTo(resizedFloat, CV_32FC1, 1.0 / 255);

			imagesVector.push_back(cv::Mat());
			cv::Mat &vec = imagesVector.back();
			resizedFloat.reshape(0, 1).copyTo(vec);
		} else {
			std::cout << "WARNING: unable to read file" << std::endl;
			std::cout << std::endl;
		}
	}
	fclose(f);

	samples.create(imagesVector.size(), myFaceWidth*myFaceHeight, CV_32FC1);
	for (size_t i = 0; i < imagesVector.size(); ++i) {
		cv::Mat rowi = samples.row(i);
		imagesVector[i].copyTo(rowi);
	}
	return true;
}


std::string EigenProcessor::saveToFile(const std::string &pca) {
	EmoPCAWriter writer;

	std::cout << "Saving PCA to file: " << pca << std::endl;
	if (!writer.write(pca, cv::Size(myFaceWidth, myFaceHeight), myCvPCA)) {
		return "Unable to write PCA data to file";
	}

	return "";
}



#include <sstream>
#include <iostream>
#include <stdio.h>

#include "TrainProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoClassesReader.h>
#include <EmoPCAReader.h>


TrainProcessor::TrainProcessor() {
}

std::string TrainProcessor::init(const std::string &configFile, const std::string &classesFile, const std::string &pcaFile) {

	std::map<std::string, std::string> config;

	if (!EmoConfigReader::loadConfig(configFile, config)) {
		return "Unable to load config";
	}

	std::vector<EmoClass> classes;
	if (!EmoClassesReader::loadClasses(classesFile, classes)) {
		return "Unable to load classes";
	}

	for (size_t i = 0; i < classes.size(); ++i) {
		myClassesList.append(1, classes[i].letter());
	}


	int err = readConfig(config);
	if (err != EMOERR_OK) {
		return EmoCore::errorMessage(err);
	}

	if (!EmoPCAReader::loadPCA(pcaFile, myFaceSize, myCvPCA, myFeatures)) {
		return "Unable to load PCA";
	}

	return "";
}


static const std::string PARAM_FEATURES = "features";

int TrainProcessor::readConfig(const std::map<std::string, std::string> &config) {
	std::map<std::string, std::string>::const_iterator featuresIter = config.find(PARAM_FEATURES);

	if (featuresIter == config.end()) {
		return EMOERR_NOPARAMETERS;
	}

	if (sscanf(featuresIter->second.c_str(), "%f", &myFeatures) != 1) {
		return EMOERR_INVALID_PARAMETERS;
	}

	return EMOERR_OK;
}


static const std::string imageWindow = "imageWindow";

std::string TrainProcessor::processInput(const std::string &input) {
	cv::Mat samples, categories;
	if (!fillSamples(input, samples, categories)) {
		return "Unable to load samples from file";
	}

	const int inputSize = myCvPCA.eigenvalues.rows;
	const int outputSize = myClassesList.length();

	cv::Mat inputs;
	myCvPCA.project(samples, inputs);

	cv::Mat outputs(samples.rows, outputSize, CV_32FC1);
	outputs = 0.0f;
	for (int i = 0; i < categories.rows; ++i) {
		char cat = categories.at<unsigned char>(i, 0);
		int index = (int) myClassesList.find(cat);
		outputs.at<float>(i, index) = 1.0f;
	}

	cv::Mat layerSizes(1, 3, CV_32SC1);
	layerSizes.at<int>(0, 0) = inputSize;
	layerSizes.at<int>(0, 1) = (inputSize + outputSize) * 2 / 3;
	layerSizes.at<int>(0, 2) = outputSize;

	myCvMLP.create(layerSizes, CvANN_MLP::SIGMOID_SYM, 1.0, 1.0);
	myCvMLP.train(inputs, outputs, cv::Mat(), cv::Mat(), CvANN_MLP_TrainParams(), 0);

	return "";
}


bool TrainProcessor::fillSamples(const std::string &inputName, cv::Mat &samples, cv::Mat &categories) {
	FILE* f = fopen(inputName.c_str(), "rt");
	if (f == 0) {
		return false;
	}

	cv::Mat image, gray, resized, resizedFloat;

	std::vector<cv::Mat> imagesVector;
	std::vector<char> categoriesVector;

	char buf[1000];
	while (fgets(buf, 1000, f)) {
		int len = (int) strlen(buf);
		while (len > 0 && isspace(buf[len-1])) {
			len--;
		}
		buf[len] = '\0';
		std::string imageName(buf);
		char cat = category(imageName);

		std::cout << "file " << imageName.c_str() << std::endl;
		if (cat == '\0') {
			std::cout << "WARNING: no category detected" << std::endl;
			std::cout << std::endl;
			continue;
		} else if (myClassesList.find(cat) == std::string::npos) {
			std::cout << "WARNING: unknown category detected" << std::endl;
			std::cout << std::endl;
			continue;
		}
		image = cv::imread(imageName, 1);
		if (!image.empty()) {
			cv::cvtColor(image, gray, CV_BGR2GRAY);
			cv::resize(gray, resized, myFaceSize, 0, 0, cv::INTER_LINEAR);
			cv::equalizeHist(resized, resized);

			resized.convertTo(resizedFloat, CV_32FC1, 1.0 / 255);

			imagesVector.push_back(cv::Mat());
			cv::Mat &vec = imagesVector.back();
			resizedFloat.reshape(0, 1).copyTo(vec);

			categoriesVector.push_back(cat);
		} else {
			std::cout << "WARNING: unable to read file" << std::endl;
			std::cout << std::endl;
		}
	}
	fclose(f);

	samples.create(imagesVector.size(), myFaceSize.width * myFaceSize.height, CV_32FC1);
	categories.create(imagesVector.size(), 1, CV_8UC1);
	for (size_t i = 0; i < imagesVector.size(); ++i) {
		cv::Mat rowi = samples.row(i);
		imagesVector[i].copyTo(rowi);
		categories.at<unsigned char>(i, 0) = categoriesVector.at(i);
	}
	return true;
}

char TrainProcessor::category(const std::string &fileName) {
	size_t line = fileName.rfind('-');
	if (line == std::string::npos) {
		return '\0';
	}
	line += 2;
	if (line == fileName.length() || line == fileName.find('.', line)) {
		return fileName.at(line - 1);
	}
	return '\0';
}


std::string TrainProcessor::saveToFile(const std::string &mlp) {

	std::cout << "Saving MLP to file: " << mlp << std::endl;
	myCvMLP.save(mlp.c_str());

	return "";
}


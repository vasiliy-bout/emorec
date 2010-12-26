
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

std::string TrainProcessor::init(const std::string &configFile, const std::string &classesFile) {

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

	return "";
}


static const std::string PARAM_LAYERS_SCALE = "layers-scale";
static const std::string PARAM_LAYERS_COUNT = "layers-count";
static const std::string PARAM_FACE_WIDTH = "face-width";
static const std::string PARAM_FACE_HEIGHT = "face-height";


int TrainProcessor::readConfig(const std::map<std::string, std::string> &config) {
	std::map<std::string, std::string>::const_iterator layersScaleIter = config.find(PARAM_LAYERS_SCALE);
	std::map<std::string, std::string>::const_iterator layersCountIter = config.find(PARAM_LAYERS_COUNT);
	std::map<std::string, std::string>::const_iterator faceWidthIter = config.find(PARAM_FACE_WIDTH);
	std::map<std::string, std::string>::const_iterator faceHeightIter = config.find(PARAM_FACE_HEIGHT);

	if (faceWidthIter == config.end()
			|| faceHeightIter == config.end()
			|| (layersScaleIter == config.end() && layersCountIter == config.end())) {
		return EMOERR_NOPARAMETERS;
	}

	if (sscanf(faceWidthIter->second.c_str(), "%d", &myFaceSize.width) != 1 || myFaceSize.width <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(faceHeightIter->second.c_str(), "%d", &myFaceSize.height) != 1 || myFaceSize.height <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}

	myLayersScale = -1.0f;
	myLayersCount = 0;
	if (layersCountIter != config.end()) {
		if (sscanf(layersCountIter->second.c_str(), "%d", &myLayersCount) != 1 || (myLayersCount < 3 && myLayersCount != 0)) {
			return EMOERR_INVALID_PARAMETERS;
		}
	}
	if (myLayersCount == 0) {
		if (layersScaleIter == config.end() || sscanf(layersScaleIter->second.c_str(), "%f", &myLayersScale) != 1 || myLayersScale <= 0.0f) {
			return EMOERR_INVALID_PARAMETERS;
		}
	}

	if (myLayersCount == 0 && myLayersScale < 0.0f) {
		return EMOERR_INTERNAL_ERROR;
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

	int layers = (myLayersCount > 0) ? myLayersCount : std::max(3, (int)(inputSize * myLayersScale));

	std::cout << std::endl;
	std::cout << "Layers number = " << layers << std::endl;

	cv::Mat layerSizes(1, layers, CV_32SC1);
	--layers;

	std::cout << "Layer sizes: " << inputSize;
	layerSizes.at<int>(0, 0) = inputSize;

	for (int i = 1; i < layers; ++i) {
		const int sz = inputSize + (outputSize - inputSize) * i / layers;
		std::cout << " " << sz;
		layerSizes.at<int>(0, i) = sz;
	}

	std::cout << " " << outputSize << std::endl;
	layerSizes.at<int>(0, layers) = outputSize;

	std::cout << std::endl;
	double timer = (double)cv::getTickCount();

	myCvMLP.create(layerSizes, CvANN_MLP::SIGMOID_SYM, 1.0, 1.0);
	myCvMLP.train(inputs, outputs, cv::Mat(), cv::Mat(), CvANN_MLP_TrainParams(), 0);

	timer = (double)cv::getTickCount() - timer;
	std::cout << "Training time = " << (timer / cv::getTickFrequency()) << " s" << std::endl;
	std::cout << std::endl;

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


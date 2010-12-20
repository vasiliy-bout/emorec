#define CV_NO_BACKWARD_COMPATIBILITY

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

#include <iostream>
#include <sstream>

#include "model.h"


const std::string outOpt = "--out=";
const std::string pcaOpt = "--pca=";
const std::string featuresOpt = "--features=";
const std::string classesOpt = "--classes=";

void printUsage() {
	std::cerr << "Usage: train\n"
		"\t--classes=\"<classes xml file>\"\n"
		"\t--pca=\"<pca file>\"\n"
		"\t--out=\"<out file>\"\n"
		"\t[--features=\"<number of features | eigenvectors factor>\"]\n"
//		"\t[--layers=\"<layers config>\"]\n"
		"\tfilename\n" ;
}

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}

//void traceImage(const cv::Mat &image, const std::string &message) {
//	cv::imshow("debug", image);
//	std::cerr << "TRACE: " << message << std::endl;
//	cv::waitKey();
//}

bool loadClasses(const std::string &fileName, std::vector<EmoClass> &classes);
bool loadFromFile(const std::string &fileName, cv::Size &imageSize, cv::PCA &pca, float featuresArg);
bool fillSamples(const std::string &inputName, int size, cv::Mat &samples, cv::Mat &categories);


int main( int argc, const char** argv )
{
	std::string inputName;
	std::string outFile;
	std::string pcaFile;
	std::string classesFile;
	float features = 0.01f;

	for (int i = 1; i < argc; ++i) {
		if (testArg(outOpt, argv[i])) {
			outFile.assign(argv[i] + outOpt.length());
		} else if (testArg(pcaOpt, argv[i])) {
			pcaFile.assign(argv[i] + pcaOpt.length());
		} else if (testArg(classesOpt, argv[i])) {
			classesFile.assign(argv[i] + classesOpt.length());
		} else if (testArg(featuresOpt, argv[i])) {
			if (!sscanf(argv[i] + featuresOpt.length(), "%f", &features)) {
				features = 0.01f;
			}
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || outFile.empty() || pcaFile.empty() || classesFile.empty()) {
		printUsage();
		return 0;
	}

	std::cout << "Loading classes XML" << std::endl;

	std::vector<EmoClass> classes;
	if (!loadClasses(classesFile, classes)) {
		std::cerr << "Unable to load classes from " << classesFile.c_str()<< std::endl;
		return 1;
	}

	std::cout << "Loading PCA model" << std::endl;

	cv::Size imageSize;
	cv::PCA pca;
	if (!loadFromFile(pcaFile, imageSize, pca, features)) {
		std::cerr << "Unable to load PCA model from " << pcaFile.c_str()<< std::endl;
		return 1;
	}
	if (imageSize.width != imageSize.height) {
		std::cerr << "Invalid pca model: images must be square" << std::endl;
		return 1;
	}

	const int size = imageSize.height;

	std::cout << "Loading sample images" << std::endl;
	cv::Mat samples, categories;
	if (!fillSamples(inputName, size, samples, categories)) {
		std::cerr << "Unable to load samples from " << inputName.c_str() << std::endl;
		return 1;
	}

	/*cvNamedWindow("source");
	cvNamedWindow("image");

	for (int i = 0; i < samples.rows; ++i) {
		cv::Mat &vector = samples.row(i);
		cv::Mat coefficients = pca.project(vector);
		cv::Mat back = pca.backProject(coefficients);

		cv::imshow("source", vector.reshape(0, size));
		cv::imshow("image", back.reshape(0, size));

		std::cout << "coeffs: ";
		for (int k = 0; k < coefficients.cols; ++k) {
			std::cout << coefficients.at<float>(0, k);
			if (k == coefficients.cols - 1) {
				std::cout << std::endl;
			} else {
				std::cout << " ";
			}
		}
		if (cv::waitKey() == 27) {
			break;
		}
	}

	cvDestroyWindow("source");
	cvDestroyWindow("image");*/

	const int inputSize = pca.eigenvalues.rows;

	std::string categoriesList;
	for (size_t i = 0; i < classes.size(); ++i) {
		categoriesList.append(1, classes[i].Letter);
	}
	const int outputSize = categoriesList.length();

	for (int i = 0; i < categories.rows; ++i) {
		char ch = categories.at<unsigned char>(i, 0);
		if (categoriesList.find(ch) == std::string::npos) {
			std::cerr << "Unknown class \"" << ch << "\" found." << std::endl;
			return 1;
		}
	}

	//cv::Mat inputs(samples.rows, inputSize, CV_32FC1);
	cv::Mat inputs;
	pca.project(samples, inputs);

	cv::Mat outputs(samples.rows, outputSize, CV_32FC1);
	outputs = 0.0f;
	for (int i = 0; i < categories.rows; ++i) {
		char cat = categories.at<unsigned char>(i, 0);
		int index = (int) categoriesList.find(cat);
		outputs.at<float>(i, index) = 1.0f;
	}

	cv::Mat layer_sizes(1, 3, CV_32SC1);
	layer_sizes.at<int>(0, 0) = inputSize;
	layer_sizes.at<int>(0, 1) = (inputSize + outputSize) * 2 / 3;
	layer_sizes.at<int>(0, 2) = outputSize;

	CvANN_MLP mlp;
	mlp.create(layer_sizes, CvANN_MLP::SIGMOID_SYM, 1.0, 1.0);

	mlp.train(inputs, outputs, cv::Mat(), cv::Mat(), CvANN_MLP_TrainParams(), 0);

	mlp.save(outFile.c_str());

	return 0;
}

char category(const std::string &fileName) {
	size_t line = fileName.rfind('-');
	if (line == fileName.length() - 2) {
		return fileName.at(line + 1);
	}

	size_t ext = fileName.find('.', line);
	if (ext == line + 2) {
		return fileName.at(line + 1);
	}
	return '\0';
}

bool fillSamples(const std::string &inputName, int size, cv::Mat &samples, cv::Mat &categories) {
	FILE* f = fopen(inputName.c_str(), "rt");
	if (f == 0) {
		return false;
	}

	cv::Mat image, gray, resized, resizedFloat;

	std::vector<cv::Mat> imagesVector;
	std::vector<char> categoriesVector;

	char buf[1000+1];
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
			std::cout << "skipped (no category found)" << std::endl;
			continue;
		}
		image = cv::imread(imageName, 1);
		if (image.empty()) {
			std::cout << "skipped (no image loaded)" << std::endl;
		} else {
			cv::cvtColor(image, gray, CV_BGR2GRAY);
			cv::resize(gray, resized, cv::Size(size, size), 0, 0, cv::INTER_LINEAR);
			cv::equalizeHist(resized, resized);

			resized.convertTo(resizedFloat, CV_32FC1, 1.0 / 255);

			imagesVector.push_back(cv::Mat());
			cv::Mat &vec = imagesVector.back();
			resizedFloat.reshape(0, 1).copyTo(vec);

			categoriesVector.push_back(cat);
		}
	}
	fclose(f);

	samples.create(imagesVector.size(), size*size, CV_32FC1);
	categories.create(imagesVector.size(), 1, CV_8UC1);
	for (size_t i = 0; i < imagesVector.size(); ++i) {
		imagesVector[i].copyTo(samples.row(i));
		categories.at<unsigned char>(i, 0) = categoriesVector.at(i);
	}
	return true;
}

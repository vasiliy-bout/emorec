#define CV_NO_BACKWARD_COMPATIBILITY

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

#include <iostream>
#include <sstream>

#include "model.h"


const std::string pcaOpt = "--pca=";
const std::string featuresOpt = "--features=";
const std::string modelOpt = "--model=";
const std::string scaleOpt = "--scale=";
const std::string startScaleOpt = "--start-scale=";
const std::string cascadeOpt = "--cascade=";
const std::string classesOpt = "--classes=";
const std::string thresholdOpt = "--threshold=";

void printUsage() {
	std::cerr << "Usage: guess\n"
		"\t--classes=\"<classes xml file>\"\n"
		"\t--pca=\"<pca file>\"\n"
		"\t[--features=\"<number of features | eigenvectors factor>\"]\n"
		"\t--model=\"<model xml file>\"\n"
		"\t[--threshold=<verdict probability threshold>]\n"
		"\t[--cascade=\"<cascade_path>\"]\n"
		"\t[--scale=<image scale>]\n"
		"\t[--start-scale=<start image scale>]\n"
		"\t<image file>\n" ;
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

bool detectAndGuess(const cv::Mat &img, cv::CascadeClassifier &cascade,
		double scale, double startScale,
		int size, const cv::PCA &pca,
		const CvANN_MLP &mlp,
		const std::vector<EmoClass> &classes, float threshold);


int main( int argc, const char** argv )
{
	std::string classesFile;
	std::string modelFile;
	std::string inputName;
	std::string cascadeName = "data/haarcascades/haarcascade_frontalface_alt.xml";
	double scale = 1.1;
	double startScale = 1.0;
	std::string pcaFile;
	float features = 0.01f;
	float threshold = 0.5f;

	for (int i = 1; i < argc; ++i) {
		if (testArg(modelOpt, argv[i])) {
			modelFile.assign(argv[i] + modelOpt.length());
		} else if (testArg(cascadeOpt, argv[i])) {
			cascadeName.assign(argv[i] + cascadeOpt.length());
		} else if (testArg(scaleOpt, argv[i])) {
			if (!sscanf(argv[i] + scaleOpt.length(), "%lf", &scale) || scale <= 1 ) {
				scale = 1.1;
			}
		} else if (testArg(startScaleOpt, argv[i])) {
			if (!sscanf(argv[i] + startScaleOpt.length(), "%lf", &startScale) || startScale < 1 ) {
				startScale = 1.0;
			}
		} else if (testArg(pcaOpt, argv[i])) {
			pcaFile.assign(argv[i] + pcaOpt.length());
		} else if (testArg(featuresOpt, argv[i])) {
			if (!sscanf(argv[i] + featuresOpt.length(), "%f", &features)) {
				features = 0.01f;
			}
		} else if (testArg(classesOpt, argv[i])) {
			classesFile.assign(argv[i] + classesOpt.length());
		} else if (testArg(thresholdOpt, argv[i])) {
			if (!sscanf(argv[i] + thresholdOpt.length(), "%f", &threshold)) {
				threshold = 0.5f;
			}
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || modelFile.empty() || pcaFile.empty() || classesFile.empty()) {
		printUsage();
		return 0;
	}

	std::cout << "Loading classes XML" << std::endl;
	std::vector<EmoClass> classes;
	if (!loadClasses(classesFile, classes)) {
		std::cerr << "Unable to load classes from " << classesFile.c_str()<< std::endl;
		return 1;
	}

	std::cout << "Loading classifier" << std::endl;
	cv::CascadeClassifier cascade;
	if (!cascade.load(cascadeName)) {
		std::cerr << "ERROR: Could not load classifier cascade" << std::endl;
		printUsage();
		return -1;
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

	std::cout << "Loading model" << std::endl;
	CvANN_MLP mlp;
	mlp.load(modelFile.c_str());

	std::cout << "Reading image(s)" << std::endl;
	cvNamedWindow("result", 1);
	cv::Mat image = cv::imread(inputName, 1);
	if (!image.empty()) {
		detectAndGuess(image, cascade, scale, startScale, size, pca, mlp, classes, threshold);
	} else if (!inputName.empty()) {
		/* assume it is a text file containing the
		list of the image filenames to be processed - one per line */
		FILE* f = fopen(inputName.c_str(), "rt");
		if (f != 0) {
			char buf[1000+1];
			while (fgets(buf, 1000, f)) {
				int len = (int) strlen(buf);
				while (len > 0 && isspace(buf[len-1])) {
					len--;
				}
				buf[len] = '\0';
				std::string imageName(buf);
				std::cout << std::endl;
				std::cout << "file " << imageName.c_str() << std::endl;
				image = cv::imread(imageName, 1);
				if (!image.empty()) {
					if (!detectAndGuess(image, cascade, scale, startScale, size, pca, mlp, classes, threshold)) {
						break;
					}
				}
			}
			fclose(f);
		}
	}

	cvDestroyWindow("result");
	return 0;
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

bool detectAndGuess(const cv::Mat &img, cv::CascadeClassifier &cascade,
		double scale, double startScale,
		int size, const cv::PCA &pca,
		const CvANN_MLP &mlp,
		const std::vector<EmoClass> &classes, float threshold) {

	cv::Mat buffer;
	cv::Mat smallImg(cvRound(img.rows/startScale), cvRound(img.cols/startScale), CV_8UC1);

	cv::cvtColor(img, buffer, CV_BGR2GRAY);

	const double factor = 1.0 / startScale;
	cv::resize(buffer, smallImg, cv::Size(), factor, factor, cv::INTER_LINEAR);
	cv::equalizeHist(smallImg, smallImg);

	cv::resize(img, buffer, cv::Size(), factor, factor, cv::INTER_LINEAR);

	int i = 0;
	double t = 0;
	std::vector<cv::Rect> faces;

	t = (double)cvGetTickCount();
	cascade.detectMultiScale(smallImg, faces, scale, 2, 
		CV_HAAR_SCALE_IMAGE /*| CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH*/,
		cv::Size(30, 30));
	t = (double)cvGetTickCount() - t;

	std::cout << "detection time = " << (t / (cvGetTickFrequency() * 1000)) << " ms" << std::endl;

	cv::Mat canvas;

	for (std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); ++r, ++i) {
		const cv::Scalar &color = colors[i%8];

		buffer.copyTo(canvas);
		cv::rectangle(canvas, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), color, 3);

		imshow("result", canvas);

		while (true) {
			std::cout << "action (g -- guess; s -- skip; q -- quit): ";

			char ch = cv::waitKey(0);
			std::cout << ch << std::endl;

			if (ch == 'q') {
				return false;
			} else if (ch == 's') {
				break;
			} else if (ch != 'g') {
				std::cout << "Unknown action: " << ch << std::endl;
				continue;
			}

			const cv::Rect roi(
				(int)(r->x * startScale),
				(int)(r->y * startScale),
				(int)(r->width * startScale),
				(int)(r->height * startScale)
			);

			cv::Mat face = img(roi);
			cv::Mat gray, resized, resizedFloat;

			cv::cvtColor(face, gray, CV_BGR2GRAY);
			cv::resize(gray, resized, cv::Size(size, size), 0, 0, cv::INTER_LINEAR);
			cv::equalizeHist(resized, resized);

			resized.convertTo(resizedFloat, CV_32FC1, 1.0 / 255);

			cv::Mat features;
			pca.project(resizedFloat.reshape(0, 1), features);

			cv::Mat output;
			mlp.predict(features, output);

			if (output.rows != 1) {
				std::cerr << "ERROR: prediction result has more than 1 row" << std::endl;
				return false;
			}
			if (output.cols != classes.size()) {
				std::cerr << "ERROR: prediction result has wrong number of classes: " << output.cols << " instead of " << classes.size() << std::endl;
				return false;
			}

			std::cout << std::endl;
			int defaultClass = -1;
			int verdict = -1;
			float verdictProb = 0.0f;
			for (int j = 0; j < output.cols; ++j) {
				if (classes[j].Default) {
					defaultClass = j;
				}
				const float prob = output.at<float>(0, j);
				if (verdict == -1 || prob > verdictProb || (prob == verdictProb && classes[j].Default)) {
					verdict = j;
					verdictProb = prob;
				}
				std::cout << "Result " << prob << "\tfor \"" << classes[j].Name << "\"" << std::endl;
			}
			std::cout << std::endl;
			if (verdict == -1) {
				std::cout << "VERDICT:\tUnknown" << std::endl;
			} else {
				if (verdictProb < threshold) {
					std::cout << "Prediction is too unreliable: prob == " << verdictProb
						<< "... Overriding to default" << std::endl;
					verdict = defaultClass;
					verdictProb = 0.0f;
				}
				std::cout << "VERDICT:\t\"" << classes[verdict].Name.c_str() << "\"" << std::endl;
			}
			std::cout << std::endl;
			std::cout << "DONE" << std::endl;
			std::cout << "press any key to continue..." << std::endl;
			std::cout << std::endl;
			ch = cv::waitKey(0);
			break;
		}
	}
	return true;
}

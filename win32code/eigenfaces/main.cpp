#define CV_NO_BACKWARD_COMPATIBILITY

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>


const std::string outOpt = "--out=";
const std::string sizeOpt = "--size=";

void printUsage() {
	std::cerr << "Usage: eigenfaces\n"
		"\t[--size=\"<images_size>\"]\n"
		"\t--out=\"<out_file>\"\n"
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

bool fillSamples(const std::string &inputName, int size, cv::Mat &samples);
bool saveToFile(const std::string &fileName, const cv::Size &imageSize, const cv::PCA &pca);

int main( int argc, const char** argv )
{
	std::string inputName;
	std::string outFile;
	int size = 100;

	for (int i = 1; i < argc; ++i) {
		if (testArg(outOpt, argv[i])) {
			outFile.assign(argv[i] + outOpt.length());
		} else if (testArg(sizeOpt, argv[i])) {
			if (!sscanf(argv[i] + sizeOpt.length(), "%d", &size) || size <= 0 ) {
				size = 100;
			}
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || outFile.empty()) {
		printUsage();
		return 0;
	}

	/*cvNamedWindow("debug");*/

	cv::Mat samples;
	if (!fillSamples(inputName, size, samples)) {
		std::cerr << "Unable to read samples" << std::endl;
		return 1;
	}

	/*for (int i = 0; i < samples.rows; ++i) {
		traceImage(samples.row(i).reshape(0, size), "sample");
	}*/

	cv::PCA pca;
	pca(samples, cv::Mat(), CV_PCA_DATA_AS_ROW);

	/*cv::Mat idxs;
	traceImage(pca.mean.reshape(0, size), "Mean PCA matrix");
	for (int i = 0; i < pca.eigenvectors.rows; ++i) {
		std::cerr << "Next eigenface (" << i << ")" << std::endl;
		std::cerr << "... eigenvalue = " << pca.eigenvalues.at<float>(i, 0) << std::endl;

		cv::Mat row = pca.eigenvectors.row(i);

		idxs.create(1, pca.eigenvectors.cols, CV_32SC1);
		cv::sortIdx(row, idxs, CV_SORT_EVERY_ROW | CV_SORT_DESCENDING);

		const int maxId = idxs.at<int>(0, 0);
		const int minId = idxs.at<int>(0, idxs.cols - 1);
		const float max = row.at<float>(0, maxId);
		const float min = row.at<float>(0, minId);
		const float alpha = std::max(std::abs(max), std::abs(min));

		cv::Mat vec8;
		pca.eigenvectors.row(i).reshape(0, size).convertTo(vec8, CV_8UC1, 128 / alpha, 128);
		cv::equalizeHist(vec8, vec8);
		traceImage(vec8, "... vector");
	}*/

	if (!saveToFile(outFile, cv::Size(size, size), pca)) {
		std::cerr << "Unable to write model to file" << std::endl;
	}

	/*cvDestroyWindow("debug");*/
	return 0;
}


bool fillSamples(const std::string &inputName, int size, cv::Mat &samples) {
	FILE* f = fopen(inputName.c_str(), "rt");
	if (f == 0) {
		return false;
	}

	cv::Mat image, gray, resized, resizedFloat;

	std::vector<cv::Mat> imagesVector;

	char buf[1000+1];
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
			cv::resize(gray, resized, cv::Size(size, size), 0, 0, cv::INTER_LINEAR);
			cv::equalizeHist(resized, resized);

			resized.convertTo(resizedFloat, CV_32FC1, 1.0 / 255);

			imagesVector.push_back(cv::Mat());
			cv::Mat &vec = imagesVector.back();
			resizedFloat.reshape(0, 1).copyTo(vec);
		}
	}
	fclose(f);

	samples.create(imagesVector.size(), size*size, CV_32FC1);
	for (size_t i = 0; i < imagesVector.size(); ++i) {
		imagesVector[i].copyTo(samples.row(i));
	}
	return true;
}


#include "EmoCoreImpl.h"


EmoCoreImpl::EmoCoreImpl(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes) :
		EmoCore(classifier, pca, mlp, classes), myInitialized(false) {
}

int EmoCoreImpl::init() {
	if (!myCvCascade.load(getClassifier())) {
		return ERROR_CANT_LOAD_CLASSIFIER;
	}

	myCvMLP.load(getMLP().c_str());
	if (myCvMLP.get_layer_count() == 0) {
		return ERROR_CANT_LOAD_MLP;
	}

	myInitialized = true;
	return 0;
}

int EmoCoreImpl::extractFace(const cv::Mat &img, cv::Rect &face) {
	return 0;
}

int EmoCoreImpl::guess(const cv::Mat &face, std::map<unsigned char, float> &results) {
	return 0;
}

int EmoCoreImpl::collectClasses(std::vector<EmoClass> &classes) {
	if (!myInitialized) {
		return ERROR_NOT_INITIALIZED;
	}
	classes.assign(myEmoClasses.begin(), myEmoClasses.end());
	return 0;
}


/*
EmoFaceDetector::EmoFaceDetector(cv::CascadeClassifier &classifier) : myCvClassifier(classifier) {
}


void EmoFaceDetector::detectFaces(const cv::Mat &img, double scale, std::vector<cv::Rect> &faces) const {
	cv::Mat buffer;
	cv::cvtColor(img, buffer, CV_BGR2GRAY);
	cv::equalizeHist(buffer, buffer);

	faces.clear();
	myCvClassifier.detectMultiScale(buffer, faces, scale, 2, 
		CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT,
		cv::Size(30, 30));
}*/


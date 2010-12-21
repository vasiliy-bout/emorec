
#include "EmoFaceDetector.h"


shared_ptr<EmoFaceDetector> EmoFaceDetector::load(const std::string &fileName) {
	cv::CascadeClassifier cascade;
	if (!cascade.load(fileName)) {
		return 0;
	}
	return new EmoFaceDetector(cascade);
}


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
}


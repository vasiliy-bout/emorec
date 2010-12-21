
#include "emocore.h"


EmoCore::EmoCore(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes) :
		myClassifier(classifier), myPCA(pca), myMLP(mlp), myClasses(classes) {
}


int EmoCore::init() {
	return 0;
}

int EmoCore::extractFace(const cv::Mat &img, cv::Mat &points) {
	return 0;
}

int EmoCore::guess(const cv::Mat &face, cv::Mat &results) {
	return 0;
}

std::string EmoCore::getErrorMessage(int) {
	return "";
}


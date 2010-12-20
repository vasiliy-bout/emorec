
#include "emocore.h"


EmoCore::EmoCore(const std::string &classifier, const std::string &model, const std::string &mlp) :
		myClassifier(classifier), myModel(model), myMLP(mlp) {
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


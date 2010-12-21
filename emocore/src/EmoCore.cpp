
#include "EmoCore.h"


const int EmoCore::ERROR_OK = 0;
const int EmoCore::ERROR_NOT_INITIALIZED = 1;
const int EmoCore::ERROR_CANT_LOAD_CLASSIFIER = 2;
const int EmoCore::ERROR_CANT_LOAD_PCA = 3;
const int EmoCore::ERROR_CANT_LOAD_MLP = 4;
const int EmoCore::ERROR_CANT_LOAD_CLASSES = 5;


shared_ptr<EmoCore> EmoCore::create(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes) {
	return 0;
}


EmoCore::EmoCore(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes) :
		myClassifier(classifier), myPCA(pca), myMLP(mlp), myClasses(classes) {
}

EmoCore::~EmoCore() {
}


std::string EmoCore::getErrorMessage(int errcode) {
	switch (errcode) {
	case ERROR_OK:
		return "";
	case ERROR_NOT_INITIALIZED:
		return "Core is used before it is initialized";
	case ERROR_CANT_LOAD_CLASSIFIER:
		return "Unable to load classifier";
	case ERROR_CANT_LOAD_PCA:
		return "Unable to load PCA-model";
	case ERROR_CANT_LOAD_MLP:
		return "Unable to load MLP-model";
	case ERROR_CANT_LOAD_CLASSES:
		return "Unable to load classes XML-file";
	}
	return "Unknown error";
}


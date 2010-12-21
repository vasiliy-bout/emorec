
#include "EmoCore.h"
#include "EmoErrors.h"


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
	case EMOERR_OK:
		return "";
	case EMOERR_NOT_INITIALIZED:
		return "Core is used before it is initialized";
	case EMOERR_CANT_LOAD_CLASSIFIER:
		return "Unable to load classifier";
	case EMOERR_CANT_LOAD_PCA:
		return "Unable to load PCA-model";
	case EMOERR_CANT_LOAD_MLP:
		return "Unable to load MLP-model";
	case EMOERR_CANT_LOAD_CLASSES:
		return "Unable to load classes XML-file";
	}
	return "Unknown error";
}


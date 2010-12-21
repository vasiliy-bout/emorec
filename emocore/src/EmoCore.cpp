
#include "EmoCore.h"


const int EmoCore::ERROR_OK = 0;
const int EmoCore::ERROR_NOT_INITIALIZED = 1;


shared_ptr<EmoCore> EmoCore::create(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes) {
	return 0;
}


EmoCore::EmoCore(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes) :
		myClassifier(classifier), myPCA(pca), myMLP(mlp), myClasses(classes) {
}


std::string EmoCore::getErrorMessage(int errcode) {
	switch (errcode) {
	case ERROR_OK:
		return "";
	}
	return "Unknown error";
}


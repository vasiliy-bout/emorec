
#include "EmoCore.h"
#include "EmoErrors.h"

#include "EmoCoreImpl.h"


shared_ptr<EmoCore> EmoCore::create() {
	return new EmoCoreImpl();
}


EmoCore::EmoCore() {
}

EmoCore::~EmoCore() {
}


std::string EmoCore::errorMessage(int errcode) {
	switch (errcode) {
	case EMOERR_OK:
		return "";

	case EMOERR_INTERNAL_ERROR:
		return "Internal application error";

	case EMOERR_NOT_INITIALIZED:
		return "Core is used before it is initialized";
	case EMOERR_CANT_LOAD_CLASSIFIER:
		return "Unable to load classifier";
	case EMOERR_CANT_LOAD_PCA:
		return "Unable to load PCA-model";
	case EMOERR_CANT_LOAD_MLP:
		return "Unable to load MLP-model";
	case EMOERR_INVALID_CLASSES:
		return "Invalid list of classes";
	case EMOERR_NOPARAMETERS:
		return "Required parameter have not been specified";
	case EMOERR_INVALID_PARAMETERS:
		return "Invalid parameter value";
	case EMOERR_INVALID_PCA_MODEL:
		return "Invalid PCA model";
	case EMOERR_INVALID_MLP_OR_CLASSES:
		return "MLP doesn't correspond to classes";
	case EMOERR_INVALID_MLP_OR_PCA:
		return "MLP doesn't correspond to PCA";
	}
	return "Unknown error";
}


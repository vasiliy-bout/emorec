
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
	case EMOERR_INVALID_CLASSES:
		return "Invalid list of classes";
	case EMOERR_NOPARAMETERS:
		return "Required parameter have not been specified";
	case EMOERR_INVALID_PARAMETERS:
		return "Invalid parameter value";
	}
	return "Unknown error";
}



#include <iostream>

#include "ShowProcessor.h"

#include <EmoErrors.h>

#include <EmoPCAReader.h>


ShowProcessor::ShowProcessor() {
}

std::string ShowProcessor::init(const std::string &pcaFile) {

	if (!EmoPCAReader::loadPCA(pcaFile, myFaceSize, myCvPCA, -1.0f)) {
		return "Unable to load PCA";
	}

	return "";
}


static const std::string imageWindow = "imageWindow";
static const std::string positionBar = "positionBar";


static void positionCallBack(int pos, void *data) {
	((ShowProcessor*)data)->showFace(pos);
}


std::string ShowProcessor::exec() {
	cvNamedWindow(imageWindow.c_str(), 1);
	//cvResizeWindow(imageWindow.c_str(), myFaceSize.width, myFaceSize.height);
	cv::createTrackbar(positionBar, imageWindow, 0, myCvPCA.eigenvectors.rows - 1, &positionCallBack, this);
	std::cout << "Press <ESC> to quit..." << std::endl;
	while (cv::waitKey() != 27);
	cvDestroyWindow(imageWindow.c_str());
	return "";
}

void ShowProcessor::showFace(int pos) {
	if (pos < 0 || pos >= myCvPCA.eigenvectors.rows) {
		std::cerr << "WARNING: invalid slider position: " << pos << std::endl;
		return;
	}
	cv::Mat buffer;
	myCvPCA.eigenvectors.row(pos).reshape(0, myFaceSize.height).copyTo(buffer);
	cv::normalize(buffer, buffer, 0.0, 1.0, cv::NORM_MINMAX);
	cv::imshow(imageWindow, buffer);
}


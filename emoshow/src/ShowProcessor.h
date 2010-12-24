
#ifndef __SHOWPROCESSOR_H__
#define __SHOWPROCESSOR_H__

#include <string>
#include <vector>
#include <map>

#include <EmoCore.h>
#include <EmoClass.h>


class ShowProcessor {

public:
	ShowProcessor();

	std::string init(const std::string &pcaFile);
	std::string exec();

	void showFace(int pos);

private:
	cv::PCA myCvPCA;
	cv::Size myFaceSize;
};

#endif /* __SHOWPROCESSOR_H__ */


#ifndef __EMOFACEDETECTOR_H__
#define __EMOFACEDETECTOR_H__

#include "EmoCore.h"


class EmoCoreImpl : public EmoCore {

public:
	EmoCoreImpl(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes);

	int init();
	int extractFace(const cv::Mat &img, cv::Rect &face);
	int guess(const cv::Mat &face, std::map<unsigned char, float> &results);
	int collectClasses(std::vector<EmoClass> &classes);

private:
	bool myInitialized;
	cv::CascadeClassifier myCvCascade;
	cv::PCA myCvPCA;
	CvANN_MLP myCvMLP;
	std::vector<EmoClass> myEmoClasses;

	float myScale;
	float myMinFace;
};


#endif /* __EMOFACEDETECTOR_H__ */

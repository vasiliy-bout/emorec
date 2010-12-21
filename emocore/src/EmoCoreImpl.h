
#ifndef __EMOFACEDETECTOR_H__
#define __EMOFACEDETECTOR_H__

#include "EmoCore.h"


class EmoCoreImpl : public EmoCore {

public:
	EmoCoreImpl();

	int init(std::vector<unsigned char> &classes, const std::map<std::string, std::string> &parameters);
	int extractFace(const cv::Mat &img, cv::Rect &face);
	int guess(const cv::Mat &face, std::map<unsigned char, float> &results);

	const std::vector<unsigned char> &classes() const;

private:
	bool myInitialized;
	std::vector<unsigned char> myClasses;
	cv::CascadeClassifier myCvCascade;
	cv::PCA myCvPCA;
	CvANN_MLP myCvMLP;
	float myScale;
	float myMinFace;
};

inline const std::vector<unsigned char> &EmoCoreImpl::classes() const { return myClasses; }

#endif /* __EMOFACEDETECTOR_H__ */

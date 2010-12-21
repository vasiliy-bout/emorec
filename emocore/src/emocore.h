
#ifndef __EMOCORE_H__
#define __EMOCORE_H__

#include <emobase.h>


class EmoCore {

public:
	EmoCore(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes);

	int init();
	int extractFace(const cv::Mat &img, cv::Mat &points);
	int guess(const cv::Mat &face, cv::Mat &results);

	std::string getErrorMessage(int);

private:
	const std::string myClassifier;
	const std::string myPCA;
	const std::string myMLP;
	const std::string myClasses;
};


#endif /* __EMOCORE_H__ */

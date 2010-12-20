
#ifndef __EMOCORE_H__
#define __EMOCORE_H__


#define CV_NO_BACKWARD_COMPATIBILITY

#include <opencv/cv.h>
#include <opencv/highgui.h>


class EmoCore {

public:
	EmoCore(const std::string &classifier, const std::string &model, const std::string &mlp);

	int init();
	int extractFace(const cv::Mat &img, cv::Mat &points);
	int guess(const cv::Mat &face, cv::Mat &results);

	std::string getErrorMessage(int);
};


#endif /* __EMOCORE_H__ */

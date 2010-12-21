
#ifndef __EMOFACEDETECTOR_H__
#define __EMOFACEDETECTOR_H__

#include <vector>

#include <emobase.h>


class EmoFaceDetector {

public:
	static shared_ptr<EmoFaceDetector> load(const std::string &fileName);

private:
	EmoFaceDetector(cv::CascadeClassifier &classifier);

public:
	void detectFaces(const cv::Mat &img, double scale, std::vector<cv::Rect> &faces) const;

private:
	mutable cv::CascadeClassifier myCvClassifier;

private: // disable copying
	EmoFaceDetector(const EmoFaceDetector &);
	const EmoFaceDetector &operator = (const EmoFaceDetector &);
};


#endif /* __EMOFACEDETECTOR_H__ */

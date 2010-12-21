
#ifndef __EMOCORE_H__
#define __EMOCORE_H__

#include <map>
#include <vector>

#include <emobase.h>


class EmoCore {

public:
	static shared_ptr<EmoCore> create();

protected:
	EmoCore();

public:
	virtual ~EmoCore();

	std::string getErrorMessage(int errcode);

	virtual int init(std::vector<unsigned char> &classes, const std::map<std::string, std::string> &parameters) = 0;
	virtual int extractFace(const cv::Mat &img, cv::Rect &face) = 0;
	virtual int guess(const cv::Mat &face, std::map<unsigned char, float> &results) = 0;

private: // disable copying
	EmoCore(const EmoCore &);
	const EmoCore &operator = (const EmoCore &);
};

#endif /* __EMOCORE_H__ */

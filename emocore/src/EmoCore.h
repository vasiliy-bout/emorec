
#ifndef __EMOCORE_H__
#define __EMOCORE_H__

#include <map>
#include <vector>

#include <emobase.h>

#include <EmoClass.h>


class EmoCore {

public:
	static shared_ptr<EmoCore> create(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes);

protected:
	EmoCore(const std::string &classifier, const std::string &pca, const std::string &mlp, const std::string &classes);

public:
	virtual ~EmoCore();

	std::string getErrorMessage(int errcode);

	virtual int init() = 0;
	virtual int extractFace(const cv::Mat &img, cv::Rect &face) = 0;
	virtual int guess(const cv::Mat &face, std::map<unsigned char, float> &results) = 0;
	virtual int collectClasses(std::vector<EmoClass> &classes) = 0;

public:
	const std::string &getClassifier() const;
	const std::string &getPCA() const;
	const std::string &getMLP() const;
	const std::string &getClasses() const;

private:
	const std::string myClassifier;
	const std::string myPCA;
	const std::string myMLP;
	const std::string myClasses;

private: // disable copying
	EmoCore(const EmoCore &);
	const EmoCore &operator = (const EmoCore &);
};

inline const std::string &EmoCore::getClassifier() const { return myClassifier; }
inline const std::string &EmoCore::getPCA() const { return myPCA; }
inline const std::string &EmoCore::getMLP() const { return myMLP; }
inline const std::string &EmoCore::getClasses() const { return myClasses; }

#endif /* __EMOCORE_H__ */


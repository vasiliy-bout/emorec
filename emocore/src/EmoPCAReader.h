
#ifndef __EMOPCAREADER_H__
#define __EMOPCAREADER_H__

#include <vector>

#include <EmoXMLReader.h>

#include <emobase.h>


class EmoPCAReader : public EmoXMLReader {

public:
	static bool loadPCA(const std::string &fileName, cv::Size &imageSize, cv::PCA &pca, float featuresArg);

private:
	EmoPCAReader(cv::Size &imageSize, cv::PCA &pca, float featuresArg);

public:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

private:
	template <class _MatType>
	bool scanBuffer(cv::Mat &matrix, size_t maxRows);

private:
	cv::Size &myImageSize;
	cv::PCA &myPca;
	std::string myBuffer;

	enum {
		READ_NOTHING,
		READ_PCA,
		READ_MEAN,
		READ_EIGENVALUES,
		READ_EIGENVECTORS,
		READ_SOURCE,
	} myState;

	size_t myLoadCount;
	float myLoadThreshold;
};


#endif /* __EMOPCAREADER_H__ */

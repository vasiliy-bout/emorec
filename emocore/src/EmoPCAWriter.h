
#ifndef __EMOPCAWRITER_H__
#define __EMOPCAWRITER_H__

#include <vector>

#include <emobase.h>


class EmoXMLWriter;

class EmoPCAWriter {

public:
	EmoPCAWriter();

public:
	bool write(const std::string &fileName, const cv::Size &imageSize, const cv::PCA &pca);

private:
	void writeSource(EmoXMLWriter &writer, const cv::Size &imageSize);
	void writeMatrix(EmoXMLWriter &writer, const std::string &name, const cv::Mat &matrix, int indent);
};


#endif /* __EMOPCAWRITER_H__ */

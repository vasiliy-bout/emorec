
#include <sstream>
#include <iomanip>

#include "EmoPCAWriter.h"

#include "xml/EmoXMLWriter.h"


EmoPCAWriter::EmoPCAWriter() {
}

bool EmoPCAWriter::write(const std::string &fileName, const cv::Size &imageSize, const cv::PCA &pca) {
	EmoXMLWriter writer(fileName);

	if (imageSize.width * imageSize.height != pca.mean.cols) {
		return false;
	}

	writer.addTag("pca", false);

	writeSource(writer, imageSize);
	writeMatrix(writer, "mean", pca.mean, 1);
	writeMatrix(writer, "eigenvalues", pca.eigenvalues, 1);
	writeMatrix(writer, "eigenvectors", pca.eigenvectors, 1);

	writer.closeAllTags();

	return true;
}


void EmoPCAWriter::writeSource(EmoXMLWriter &writer, const cv::Size &imageSize) {

	std::ostringstream oss;

	writer.addTag("source", true);

	oss.str("");
	oss << imageSize.height;
	writer.addAttribute("rows", oss.str());

	oss.str("");
	oss << imageSize.width;
	writer.addAttribute("cols", oss.str());
}

void EmoPCAWriter::writeMatrix(EmoXMLWriter &writer, const std::string &name, const cv::Mat &matrix, int indent) {
	writer.addTag(name, false);

	std::ostringstream oss;
	
	oss.str("");
	oss << matrix.rows;
	writer.addAttribute("rows", oss.str());

	oss.str("");
	oss << matrix.cols;
	writer.addAttribute("cols", oss.str());

	oss.str("");
	oss << "0x" << std::hex << matrix.type() << std::dec;
	writer.addAttribute("type", oss.str());

	oss.str("");
	oss << "\n";
	for (int i = 0; i <= indent; ++i) {
		oss << "\t";
	}
	std::string dataIndent(oss.str());

	oss.str("");
	oss << std::setprecision(12);
	for (int i = 0; i < matrix.rows; ++i) {
		for (int j = 0; j < matrix.cols; ++j) {
			float el = matrix.at<float>(i, j);
			oss << el;
			if (j < matrix.cols - 1) {
				oss << " ";
			}
		}
		writer.addData(dataIndent);
		writer.addData(oss.str());
		oss.str("");
	}

	oss << "\n";
	for (int i = 0; i < indent; ++i) {
		oss << "\t";
	}
	writer.addData(oss.str());

	writer.closeTag();
}


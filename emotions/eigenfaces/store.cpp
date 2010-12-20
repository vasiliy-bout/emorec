#define CV_NO_BACKWARD_COMPATIBILITY

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <opencv/cv.h>



template <class _MatType> void writeMatrix(std::ofstream &ofs, const std::string &name, const cv::Mat &mat, const std::string &indent = std::string()) {
	std::cout << "Writing matrix \"" << name << "\"" << std::endl;

	ofs << indent << "<" << name;
	do {
		std::ostringstream oss;
		oss.str("");
		oss << mat.cols;
		ofs << " cols=\"" << oss.str().c_str() << "\"";
		oss.str("");
		oss << mat.rows;
		ofs << " rows=\"" << oss.str().c_str() << "\"";
		oss.str("");
		oss << "0x" << std::hex << mat.type() << std::dec;
		ofs << " type=\"" << oss.str().c_str() << "\"";
	} while (false);
	ofs << ">" << std::endl;

	const int step = mat.rows * mat.cols / 10;
	for (int i = 0; i < mat.rows; ++i) {
		const int written = i * mat.cols;
		ofs << indent << "\t";
		for (int j = 0; j < mat.cols; ++j) {
			_MatType el = mat.at<_MatType>(i, j);
			ofs << el;
			if (j < mat.cols - 1) {
				ofs << " ";
			} else {
				ofs << "\n";
			}
			if ((written + j + 1) % step == 0) {
				std::cout << "." << std::flush;
			}
		}
	}
	std::cout << "\n";
	ofs << indent << "</" << name << ">" << std::endl;
}


bool saveToFile(const std::string &fileName, const cv::Size &imageSize, const cv::PCA &pca) {
	std::ofstream ofs(fileName.c_str());
	if (ofs.bad()) {
		return false;
	}
	if (imageSize.width * imageSize.height != pca.mean.cols) {
		return false;
	}
	ofs << std::setprecision(12);
	ofs << "<?xml version=\"1.0\"?>" << std::endl;
	ofs << "<pca>" << std::endl;
	ofs << "\t<source rows=\"" << imageSize.height << "\" cols=\"" << imageSize.width << "\"/>" << std::endl;
	writeMatrix<float>(ofs, "mean", pca.mean, "\t");
	writeMatrix<float>(ofs, "eigenvalues", pca.eigenvalues, "\t");
	writeMatrix<float>(ofs, "eigenvectors", pca.eigenvectors, "\t");
	ofs << "</pca>" << std::endl;
	return true;
}

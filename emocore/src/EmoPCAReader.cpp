
#include <stdio.h>

#include "EmoPCAReader.h"


bool EmoPCAReader::loadPCA(const std::string &fileName, cv::Size &imageSize, cv::PCA &pca, float featuresArg) {
	EmoPCAReader reader(imageSize, pca, featuresArg);

	if (!reader.readDocument(fileName)) {
		return false;
	}
	if (reader.isInterrupted()) {
		return false;
	}

	if (imageSize.width * imageSize.height != pca.mean.cols) {
		return false;
	}
	return true;
}


EmoPCAReader::EmoPCAReader(cv::Size &imageSize, cv::PCA &pca, float featuresArg) : myImageSize(imageSize), myPca(pca) {
	if (featuresArg <= 0.0f) {
		myLoadCount = -1;
		myLoadThreshold = 0.0f;
	} else if (featuresArg >= 1.0f) {
		myLoadCount = (size_t) featuresArg;
		myLoadThreshold = 0.0f;
	} else {
		myLoadCount = 0;
		myLoadThreshold = featuresArg;
	}
	myState = READ_NOTHING;
}


static const std::string TAG_PCA = "pca";
static const std::string TAG_MEAN = "mean";
static const std::string TAG_EIGENVALUES = "eigenvalues";
static const std::string TAG_EIGENVECTORS = "eigenvectors";
static const std::string TAG_SOURCE = "source";


void EmoPCAReader::startElementHandler(const char *tag, const char **attributes) {
	switch (myState) {
		case READ_NOTHING:
			if (TAG_PCA.compare(tag) == 0) {
				myState = READ_PCA;
			} else {
				interrupt();
			}
			break;
		case READ_PCA:
			do {
				int cols = -1, rows = -1;
				const char *buf;
				if ((buf = attributeValue(attributes, "cols")) != 0) {
					if (sscanf(buf, "%d", &cols) != 1) {
						cols = -1;
					}
				}
				if ((buf = attributeValue(attributes, "rows")) != 0) {
					if (sscanf(buf, "%d", &rows) != 1) {
						rows = -1;
					}
				}
				if (cols == -1 || rows == -1) {
					interrupt();
				} else if (TAG_MEAN.compare(tag) == 0) {
					myState = READ_MEAN;
					myPca.mean.create(rows, cols, CV_32FC1);
				} else if (TAG_EIGENVALUES.compare(tag) == 0) {
					myState = READ_EIGENVALUES;
					myPca.eigenvalues.create(rows, cols, CV_32FC1);
				} else if (TAG_EIGENVECTORS.compare(tag) == 0) {
					myState = READ_EIGENVECTORS;
					myPca.eigenvectors.create(rows, cols, CV_32FC1);
				} else if (TAG_SOURCE.compare(tag) == 0) {
					myState = READ_SOURCE;
					myImageSize.height = rows;
					myImageSize.width = cols;
				} else {
					interrupt();
				}
			} while (false);
			break;
		case READ_MEAN:
		case READ_EIGENVALUES:
		case READ_EIGENVECTORS:
		case READ_SOURCE:
			interrupt();
			break;
	}
	myBuffer.clear();
}

void EmoPCAReader::endElementHandler(const char *tag) {
	switch (myState) {
		case READ_NOTHING:
			interrupt();
			break;
		case READ_PCA:
			if (TAG_PCA.compare(tag) == 0) {
				myState = READ_NOTHING;
			} else {
				interrupt();
			}
			break;
		case READ_MEAN:
			if (TAG_MEAN.compare(tag) == 0) {
				if (!scanBuffer<float>(myPca.mean, -1)) {
					interrupt();
				} else {
					myState = READ_PCA;
				}
			} else {
				interrupt();
			}
			break;
		case READ_EIGENVALUES:
			if (TAG_EIGENVALUES.compare(tag) == 0) {
				size_t maxRows = (myLoadCount == 0) ? -1 : myLoadCount;
				if (!scanBuffer<float>(myPca.eigenvalues, maxRows)) {
					interrupt();
				} else {
					if (myLoadCount == 0) {
						const float thresh = myPca.eigenvalues.at<float>(0, 0) * myLoadThreshold;
						while ((int)myLoadCount < myPca.eigenvalues.rows
								&& myPca.eigenvalues.at<float>((int)myLoadCount, 0) >= thresh) {
							++ myLoadCount;
						}
						myLoadThreshold = 0.0f;
						if (myPca.eigenvalues.rows != (int)myLoadCount) {
							cv::Mat tmp;
							myPca.eigenvalues.rowRange(0, myLoadCount).copyTo(tmp);
							tmp.copyTo(myPca.eigenvalues);
						}
					}
					myState = READ_PCA;
				}
			} else {
				interrupt();
			}
			break;
		case READ_EIGENVECTORS:
			if (TAG_EIGENVECTORS.compare(tag) == 0) {
				if (myLoadCount == 0) {
					interrupt();
				} else if (!scanBuffer<float>(myPca.eigenvectors, myLoadCount)) {
					interrupt();
				} else {
					myState = READ_PCA;
				}
			} else {
				interrupt();
			}
			break;
		case READ_SOURCE:
			if (TAG_SOURCE.compare(tag) == 0) {
				myState = READ_PCA;
			} else {
				interrupt();
			}
			break;
	}
	myBuffer.clear();
}

void EmoPCAReader::characterDataHandler(const char *text, size_t len) {
	myBuffer.append(text, len);
}


template <class _MatType>
bool EmoPCAReader::scanBuffer(cv::Mat &matrix, size_t maxRows) {
	static const std::string spaces = " \n\r\t";

	size_t beginIndex;
	size_t endIndex = 0;

	if ((size_t)matrix.rows > maxRows) {
		matrix.create(maxRows, matrix.cols, matrix.type());
	}

	std::istringstream iss;
	for (int i = 0; i < matrix.rows; ++i) {
		for (int j = 0; j < matrix.cols; ++j) {
			if (endIndex == std::string::npos) {
				return false;
			}
			beginIndex = myBuffer.find_first_not_of(spaces, endIndex);
			if (beginIndex == std::string::npos) {
				return false;
			}
			endIndex = myBuffer.find_first_of(spaces, beginIndex);

			iss.clear();
			iss.str(myBuffer.substr(beginIndex, endIndex - beginIndex));
			if (iss.eof()) {
				return false;
			}

			_MatType el;
			iss >> el;
			matrix.at<_MatType>(i, j) = el;
		}
	}
	return true;
}


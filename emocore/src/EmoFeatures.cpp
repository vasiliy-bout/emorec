
#include <iostream>

#include "EmoFeatures.h"


EmoFeatures::Feature::Feature() {
	Width = 0;
	Height = 0;
	for (int i = 0; i < EMO_FEATURES_MAX_RECS; ++i) {
		FeatureRect &r = Recs[i];
		r.Rect = cv::Rect(0, 0, 0, 0);
		r.Weight = 0.f;
	}
}

EmoFeatures::Feature::Feature(int w, int h, const cv::Rect &rect0, float weight0, const cv::Rect &rect1, float weight1) {
	Width = w;
	Height = h;
	for (int i = 0; i < EMO_FEATURES_MAX_RECS; ++i) {
		FeatureRect &r = Recs[i];
		r.Rect = cv::Rect(0, 0, 0, 0);
		r.Weight = 0.f;
	}
	Recs[0].Rect = rect0;
	Recs[0].Weight = weight0;
	Recs[1].Rect = rect1;
	Recs[1].Weight = weight1;
}

float EmoFeatures::Feature::compute(const cv::Mat &integral, int x, int y) const {
	float res = 0.f;
	for (int i = 0; i < EMO_FEATURES_MAX_RECS; ++i) {
		const FeatureRect &r = Recs[i];
		if (r.Weight != 0.f) {
			const int x0 = x + r.Rect.x;
			const int y0 = y + r.Rect.y;
			const int x1 = x0 + r.Rect.width;
			const int y1 = y0 + r.Rect.height;
			res += r.Weight * (integral.at<double>(y1, x1) - integral.at<double>(y1, x0) - integral.at<double>(y0, x1) + integral.at<double>(y0, x0));
		}
	}
	return res;
}

float EmoFeatures::Feature::computeTilted(const cv::Mat &integral, int x, int y) const {
	float res = 0.f;
	for (int i = 0; i < EMO_FEATURES_MAX_RECS; ++i) {
		const FeatureRect &r = Recs[i];
		if (r.Weight != 0.f) {
			const int x0 = x + r.Rect.x - r.Rect.y;
			const int y0 = y + r.Rect.x + r.Rect.y;
			const int x1 = x0 + r.Rect.width;
			const int y1 = y0 + r.Rect.width;
			const int x2 = x0 - r.Rect.height;
			const int y2 = y0 + r.Rect.height;
			const int x3 = x2 + r.Rect.width;
			const int y3 = y2 + r.Rect.width;
			res += r.Weight * (integral.at<double>(y3, x3) - integral.at<double>(y2, x2) - integral.at<double>(y1, x1) + integral.at<double>(y0, x0));
		}
	}
	return res;
}



EmoFeatures::EmoFeatures(int size) : mySize(size) {
	myFeatures.assign(7, Feature());

	float sz2 = 1.f / (size * size);

	myFeatures[0] = Feature(4 * size, 4 * size, cv::Rect(0, 0, 2 * size, 4 * size), 1.f * sz2, cv::Rect(2, 0, 2 * size, 4 * size), -1.f * sz2);
	myFeatures[1] = Feature(4 * size, 4 * size, cv::Rect(0, 0, 4 * size, 2 * size), 1.f * sz2, cv::Rect(0, 2, 4 * size, 2 * size), -1.f * sz2);
	myFeatures[2] = Feature(3 * size, 2 * size, cv::Rect(0, 0, 3 * size, 2 * size), 1.f * sz2, cv::Rect(1, 0, 1 * size, 2 * size), -3.f * sz2);
	myFeatures[3] = Feature(4 * size, 2 * size, cv::Rect(0, 0, 4 * size, 2 * size), 1.f * sz2, cv::Rect(1, 0, 2 * size, 2 * size), -2.f * sz2);
	myFeatures[4] = Feature(2 * size, 3 * size, cv::Rect(0, 0, 2 * size, 3 * size), 1.f * sz2, cv::Rect(0, 1, 2 * size, 1 * size), -3.f * sz2);
	myFeatures[5] = Feature(2 * size, 4 * size, cv::Rect(0, 0, 2 * size, 4 * size), 1.f * sz2, cv::Rect(0, 1, 2 * size, 2 * size), -2.f * sz2);
	myFeatures[6] = Feature(3 * size, 3 * size, cv::Rect(0, 0, 3 * size, 3 * size), 1.f * sz2, cv::Rect(1, 1, 1 * size, 1 * size), -9.f * sz2);
}

void EmoFeatures::project(const cv::Mat &image, cv::Mat &features) {
	int count = featuresNumber(image.size());

	if (features.rows != 1 || features.cols != count || features.type() != CV_32FC1) {
		features.create(1, count, CV_32FC1);
	}

	cv::Mat sum, sqsum, tilted;
	cv::integral(image, sum, sqsum, tilted, CV_64F);

	int featureNumber = 0;

	for (size_t i = 0; i < myFeatures.size(); ++i) {
		const Feature &f = myFeatures[i];
		for (int y = 0; y + f.Height < sum.rows; y += f.Height) {
			for (int x = 0; x + f.Width < sum.cols; x += f.Width) {
				features.at<float>(0, featureNumber++) = f.compute(sum, x, y);
			}
		}
	}
	for (size_t i = 0; i < myFeatures.size(); ++i) {
		const Feature &f = myFeatures[i];
		const int wh = f.Width + f.Height;
		for (int y = 0; y + wh < sum.rows; y += wh) {
			for (int x = f.Height; x + f.Width < sum.cols; x += wh) {
				features.at<float>(0, featureNumber++) = f.computeTilted(tilted, x, y);
			}
		}
	}
	if (featureNumber != count) {
		std::cerr << "ERROR: featureNumber != count" << std::endl;
		exit(1);
	}
}


int EmoFeatures::featuresNumber(const cv::Size &imageSize) {
	int count = 0;
	for (size_t i = 0; i < myFeatures.size(); ++i) {
		const Feature &f = myFeatures[i];
		int nx = imageSize.width / f.Width;
		int ny = imageSize.height / f.Height;
		count += nx * ny;

		int wh = f.Width + f.Height;
		nx = imageSize.width / wh;
		ny = imageSize.height / wh;
		count += nx * ny;
	}
	return count;
}



#ifndef __EMOFEATURES_H__
#define __EMOFEATURES_H__

#include <emobase.h>

#define EMO_FEATURES_MAX_RECS 2

class EmoFeatures {

	struct Feature {
		int Width;
		int Height;
		struct FeatureRect {
			cv::Rect Rect;
			float Weight;
		} Recs[EMO_FEATURES_MAX_RECS];

		Feature();
		Feature(int w, int h, const cv::Rect &rect0, float weight0, const cv::Rect &rect1, float weight1);

		float compute(const cv::Mat &integral, int x, int y) const;
		float computeTilted(const cv::Mat &integral, int x, int y) const;
	};

public:
	EmoFeatures(int size);

	void project(const cv::Mat &image, cv::Mat &features);

	int featuresNumber(const cv::Size &imageSize);

private:
	const int mySize;
	std::vector<Feature> myFeatures;
};

#endif /* __EMOFEATURES_H__ */

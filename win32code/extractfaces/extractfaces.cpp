#define CV_NO_BACKWARD_COMPATIBILITY

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>


bool detectAndDraw(const cv::Mat &img, cv::CascadeClassifier &cascade,
		double scale, double startScale,
		const std::string &baseName, const std::string &outdir);


const std::string scaleOpt = "--scale=";
const std::string startScaleOpt = "--start-scale=";
const std::string cascadeOpt = "--cascade=";
const std::string outOpt = "--out=";

static bool testArg(const std::string &flag, const char *arg) {
	const size_t len = flag.size();
	return flag.compare(0, len, arg, len) == 0;
}

void printUsage() {
	std::cerr << "Usage: extractfaces\n"
		"\t[--cascade=\"<cascade_path>\"]\n"
		"\t[--scale=<image scale>]\n"
		"\t[--start-scale=<start image scale>]\n"
		"\t--out=\"<out_folder>\"\n"
		"\tfilename\n" ;
}

void extractBaseName(const std::string &fileName, std::string &baseName) {
	size_t index = fileName.find_last_of("/\\");
	baseName.assign(fileName, index + 1, -1);
	index = baseName.rfind('.');
	if (index != -1) {
		baseName.erase(index);
	}
}

int main( int argc, const char** argv )
{
	std::string inputName;
	std::string outFolder;
	std::string cascadeName = "data/haarcascades/haarcascade_frontalface_alt.xml";
	double scale = 1.1;
	double startScale = 1.0;

	for (int i = 1; i < argc; ++i) {
		if (testArg(cascadeOpt, argv[i])) {
			cascadeName.assign(argv[i] + cascadeOpt.length());
		} else if (testArg(scaleOpt, argv[i])) {
			if (!sscanf(argv[i] + scaleOpt.length(), "%lf", &scale) || scale <= 1 ) {
				scale = 1.1;
			}
		} else if (testArg(startScaleOpt, argv[i])) {
			if (!sscanf(argv[i] + startScaleOpt.length(), "%lf", &startScale) || startScale < 1 ) {
				startScale = 1.0;
			}
		} else if (testArg(outOpt, argv[i])) {
			outFolder.assign(argv[i] + outOpt.length());
		} else if (argv[i][0] == '-') {
			std::cerr << "WARNING: Unknown option %s" << argv[i] << std::endl;
		} else {
			inputName.assign(argv[i]);
		}
	}

	if (inputName.empty() || outFolder.empty()) {
		printUsage();
		return 0;
	}

	cv::CascadeClassifier cascade;

	if (!cascade.load(cascadeName)) {
		std::cerr << "ERROR: Could not load classifier cascade" << std::endl;
		printUsage();
		return -1;
	}

	cv::Mat image = cv::imread(inputName, 1);
	std::string baseName;

	cvNamedWindow("result", 1);

	if (!image.empty()) {
		extractBaseName(inputName, baseName);
		detectAndDraw(image, cascade, scale, startScale, baseName, outFolder);
	} else if (!inputName.empty()) {
		/* assume it is a text file containing the
		list of the image filenames to be processed - one per line */
		FILE* f = fopen(inputName.c_str(), "rt");
		if (f != 0) {
			char buf[1000+1];
			while (fgets(buf, 1000, f)) {
				int len = (int) strlen(buf);
				while (len > 0 && isspace(buf[len-1])) {
					len--;
				}
				buf[len] = '\0';
				std::string imageName(buf);
				std::cout << "file " << imageName.c_str() << std::endl;
				image = cv::imread(imageName, 1);
				if (!image.empty()) {
					extractBaseName(imageName, baseName);
					if (!detectAndDraw(image, cascade, scale, startScale, baseName, outFolder)) {
						break;
					}
				}
			}
			fclose(f);
		}
	}

	cvDestroyWindow("result");
	return 0;
}


const static cv::Scalar colors[] =  {
	CV_RGB(0,0,255),
	CV_RGB(0,128,255),
	CV_RGB(0,255,255),
	CV_RGB(0,255,0),
	CV_RGB(255,128,0),
	CV_RGB(255,255,0),
	CV_RGB(255,0,0),
	CV_RGB(255,0,255)
};


bool detectAndDraw(const cv::Mat &img, cv::CascadeClassifier &cascade,
		double scale, double startScale,
		const std::string &baseName, const std::string &outdir) {

	cv::Mat buffer;
	cv::Mat smallImg(cvRound(img.rows/startScale), cvRound(img.cols/startScale), CV_8UC1);

	cv::cvtColor(img, buffer, CV_BGR2GRAY);

	const double factor = 1.0 / startScale;
	cv::resize(buffer, smallImg, cv::Size(), factor, factor, cv::INTER_LINEAR);
	cv::equalizeHist(smallImg, smallImg);

	cv::resize(img, buffer, cv::Size(), factor, factor, cv::INTER_LINEAR);

	int i = 0;
	double t = 0;
	std::vector<cv::Rect> faces;

	t = (double)cvGetTickCount();
	cascade.detectMultiScale(smallImg, faces, scale, 2, 
		CV_HAAR_SCALE_IMAGE /*| CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH*/,
		cv::Size(30, 30));
	t = (double)cvGetTickCount() - t;

	std::cout << "detection time = " << (t / (cvGetTickFrequency() * 1000)) << " ms\n" << std::endl;

	cv::Mat canvas;

	for (std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); ++r, ++i) {
		const cv::Scalar &color = colors[i%8];

		buffer.copyTo(canvas);
		cv::rectangle(canvas, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), color, 3);

		imshow("result", canvas);

		while (true) {
			std::cout << "action (q -- quit; s -- skip; z -- save): ";

			char ch = cv::waitKey(0);
			std::cout << ch << std::endl;

			if (ch == 'q') {
				return false;
			} else if (ch == 's') {
				break;
			} else if (ch != 'z') {
				std::cout << "Unknown action: " << ch << std::endl;
				continue;
			}

			std::ostringstream fns;
			fns << outdir;
			const char last = outdir[outdir.length() - 1];
			if (last != '/' && last != '\\') {
				fns << "\\";
			}
			fns << baseName << "_" << i << ".ppm";
			std::string fileName = fns.str();
			const cv::Rect roi(
				(int)(r->x * startScale),
				(int)(r->y * startScale),
				(int)(r->width * startScale),
				(int)(r->height * startScale)
			);
			cv::imwrite(fileName, img(roi));
			break;
		}
	}
	return true;
}

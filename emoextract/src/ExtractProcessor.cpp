
#include <iostream>
#include <iomanip>
#include <stdio.h>

#include "ExtractProcessor.h"

#include <EmoErrors.h>

#include <EmoConfigReader.h>
#include <EmoClassesReader.h>

#include <EmoClassesReader.h>


ExtractProcessor::ExtractProcessor() {
}

std::string ExtractProcessor::init(const std::string &classesFile, const std::string &configFile) {

	std::map<std::string, std::string> config;

	if (!EmoConfigReader::loadConfig(configFile, config)) {
		return "Unable to load config";
	}

	int err = readConfig(config);
	if (err != EMOERR_OK) {
		return EmoCore::errorMessage(err);
	}

	if (!EmoClassesReader::loadClasses(classesFile, myClasses)) {
		return "Unable load classes";
	}

	return "";
}


static const std::string PARAM_MORPH_OFFSET = "morph-offset";
static const std::string PARAM_MORPH_OFFSET_STEP = "morph-offset-step";
static const std::string PARAM_MORPH_SCALE = "morph-scale";
static const std::string PARAM_MORPH_SCALE_STEP = "morph-scale-step";

static const std::string PARAM_CASCADE = "cascade";
static const std::string PARAM_SCALE = "scale";
static const std::string PARAM_MIN_FACE = "minFacePercent";

static const std::string PARAM_OUT_FOLDER = "out-folder";
static const std::string PARAM_OUT_WIDTH = "out-width";
static const std::string PARAM_OUT_HEIGHT = "out-height";

static const std::string PARAM_START_SCALE = "start-scale";


int ExtractProcessor::readConfig(const std::map<std::string, std::string> &config) {
	std::map<std::string, std::string>::const_iterator morhpOffsetIter = config.find(PARAM_MORPH_OFFSET);
	std::map<std::string, std::string>::const_iterator morhpOffsetStepIter = config.find(PARAM_MORPH_OFFSET_STEP);
	std::map<std::string, std::string>::const_iterator morhpScaleIter = config.find(PARAM_MORPH_SCALE);
	std::map<std::string, std::string>::const_iterator morhpScaleStepIter = config.find(PARAM_MORPH_SCALE_STEP);

	std::map<std::string, std::string>::const_iterator cascadeIter = config.find(PARAM_CASCADE);
	std::map<std::string, std::string>::const_iterator scaleIter = config.find(PARAM_SCALE);
	std::map<std::string, std::string>::const_iterator minFaceIter = config.find(PARAM_MIN_FACE);

	std::map<std::string, std::string>::const_iterator outFolderIter = config.find(PARAM_OUT_FOLDER);
	std::map<std::string, std::string>::const_iterator outWidthIter = config.find(PARAM_OUT_WIDTH);
	std::map<std::string, std::string>::const_iterator outHeightIter = config.find(PARAM_OUT_HEIGHT);

	std::map<std::string, std::string>::const_iterator startScaleIter = config.find(PARAM_START_SCALE);

	if (morhpOffsetIter == config.end()
			|| morhpOffsetStepIter == config.end()
			|| morhpScaleIter == config.end()
			|| morhpScaleStepIter == config.end()
			|| cascadeIter == config.end()
			|| scaleIter == config.end()
			|| minFaceIter == config.end()
			|| outFolderIter == config.end()
			|| startScaleIter == config.end()) {
		return EMOERR_NOPARAMETERS;
	}

	if (sscanf(morhpOffsetIter->second.c_str(), "%d", &myMorphOffset) != 1 || myMorphOffset <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(morhpOffsetStepIter->second.c_str(), "%d", &myMorphOffsetStep) != 1 || myMorphOffsetStep <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(morhpScaleIter->second.c_str(), "%d", &myMorphScale) != 1 || myMorphScale <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(morhpScaleStepIter->second.c_str(), "%d", &myMorphScaleStep) != 1 || myMorphScaleStep <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}

	const std::string &cascade = cascadeIter->second;

	if (sscanf(scaleIter->second.c_str(), "%f", &myScale) != 1 || myScale <= 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(minFaceIter->second.c_str(), "%f", &myMinFace) != 1 || myMinFace < 0.0f || myMinFace > 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}

	if (!myCvCascade.load(cascade)) {
		return EMOERR_CANT_LOAD_CLASSIFIER;
	}

	myOutFolder = outFolderIter->second;
	if (myOutFolder.empty()) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (myOutFolder[myOutFolder.length() - 1] != '/') {
		myOutFolder.append("/");
	}

	if (sscanf(outWidthIter->second.c_str(), "%d", &myOutSize.width) != 1 || myOutSize.width <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}
	if (sscanf(outHeightIter->second.c_str(), "%d", &myOutSize.height) != 1 || myOutSize.height <= 0) {
		return EMOERR_INVALID_PARAMETERS;
	}

	if (sscanf(startScaleIter->second.c_str(), "%f", &myStartScale) != 1 || myStartScale <= 1.0f) {
		return EMOERR_INVALID_PARAMETERS;
	}

	return EMOERR_OK;
}


static const std::string imageWindow = "imageWindow";

std::string ExtractProcessor::processInput(const std::string &input) {
	cvNamedWindow(imageWindow.c_str(), 1);

	if (!processIndex(input)) {
		std::cerr << "ERROR: unable to process index file" << std::endl;
	}

	cvDestroyWindow(imageWindow.c_str());

	return "";
}


std::string ExtractProcessor::baseName(const std::string &fileName) {
	size_t index = fileName.rfind('/');
	std::string name;
	if (index == std::string::npos) {
		name = fileName;
	} else {
		name = fileName.substr(index + 1);
	}
	index = name.rfind('.');
	if (index == std::string::npos || index + 4 != name.length()) {
		return name;
	}
	return name.substr(0, index);
}


bool ExtractProcessor::processIndex(const std::string &input) {
	FILE* f = fopen(input.c_str(), "rt");
	if (f == 0) {
		return false;
	}

	std::cout << "Processing index file \"" << input << "\"" << std::endl;

	cv::Mat frame;

	char buf[1000];
	while (fgets(buf, 1000, f)) {
		int len = (int) strlen(buf);
		while (len > 0 && isspace(buf[len-1])) {
			len--;
		}
		buf[len] = '\0';
		std::string imageName(buf);
		std::string base = baseName(imageName);

		std::cout << "file " << imageName.c_str() << std::endl;
		frame = cv::imread(imageName, 1);
		if (!frame.empty()) {
			if (!processImage(frame, base)) {
				break;
			}
		} else {
			std::cout << "WARNING: unable to read file" << std::endl;
			std::cout << std::endl;
		}
	}

	fclose(f);
	return true;
}

bool ExtractProcessor::processImage(const cv::Mat &image, const std::string &base) {

	cv::Mat buffer;
	cv::cvtColor(image, buffer, CV_BGR2GRAY);
	cv::equalizeHist(buffer, buffer);

	std::vector<cv::Rect> faces;
	myCvCascade.detectMultiScale(buffer, faces, myScale, 2, CV_HAAR_SCALE_IMAGE,
		cv::Size(buffer.cols * myMinFace, buffer.rows * myMinFace));

	if (faces.empty()) {
		cv::Mat canvas;
		const double factor = 1.0 / myStartScale;
		cv::resize(image, canvas, cv::Size(), factor, factor, cv::INTER_LINEAR);
		cv::imshow(imageWindow, canvas);

		std::cerr << "Unable to find faces" << std::endl;
		std::cerr << std::endl;
		cv::waitKey();
		return true;
	}

	for (size_t i = 0; i < faces.size(); ++i) {
		if (!processFace(image, faces[i], base, i)) {
			return false;
		}
	}

	return true;
}


char ExtractProcessor::readClass() {
	std::cout << "classes: " << std::endl;
	for (size_t i = 0; i < myClasses.size(); ++i) {
		const EmoClass &cls = myClasses[i];
		std::cout << "\tclass \"" << cls.name() << "\" -- " << cls.letter() << std::endl;
	}
	std::cout << "Press <ESC> to skip image" << std::endl;
	while (true) {
		std::cout << "Enter class: " << std::flush;
		char letter = cv::waitKey();
		if (letter == 27) {
			std::cout << "<ESC>" << std::endl;
			return '\0';
		}
		std::cout << letter << std::endl;
		for (size_t i = 0; i < myClasses.size(); ++i) {
			const EmoClass &cls = myClasses[i];
			if (letter == cls.letter()) {
				return letter;
			}
		}
		std::cout << "Unknown class" << std::endl;
	}
}

bool ExtractProcessor::processFace(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number) {

	cv::Mat canvas;
	const double factor = 1.0 / myStartScale;
	cv::resize(image, canvas, cv::Size(), factor, factor, cv::INTER_LINEAR);
	cv::rectangle(
		canvas,
		cvPoint(rect.x * factor, rect.y * factor),
		cvPoint((rect.x + rect.width) * factor, (rect.y + rect.height) * factor),
		CV_RGB(255, 0, 255), 3);
	cv::imshow(imageWindow, canvas);

	while (true) {
		std::cout << "action (q -- quit; s -- skip; z -- save): " << std::flush;

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
		std::cout << std::endl;

		char letter = readClass();
		if (letter == '\0') {
			break;
		}
		std::cout << std::endl;

		doScale(image, rect, base, number, 0, letter);
		break;
	}
	return true;
}

int ExtractProcessor::writeImage(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number, int morphnumber, char letter) {
	if (rect.x < 0 || rect.y < 0 || (rect.x + rect.width) > image.cols || (rect.y + rect.height) > image.rows) {
		std::cerr << "\t\tout of range..." << std::endl;
		return morphnumber;
	}

	cv::Mat face = image(rect);

	std::ostringstream fns;
	fns << myOutFolder << base << "_";
	fns << std::setfill('0') << std::setw(2) << number << std::setfill(' ') << std::setw(0);
	fns << "_";
	fns << std::setfill('0') << std::setw(4) << morphnumber << std::setfill(' ') << std::setw(0);
	fns << "-" << letter << ".ppm";

	std::string fileName = fns.str();

	cv::Mat buffer;
	cv::resize(face, buffer, myOutSize, 0.f, 0.f, cv::INTER_LINEAR);
	cv::imwrite(fileName, buffer);
	return morphnumber + 1;
}

int ExtractProcessor::doScale(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number, int morphnumber, char letter) {
	std::cerr << "SCALE: " << 0 << std::endl;

	morphnumber = doOffset(image, rect, base, number, morphnumber, letter);

	int scale = myMorphScaleStep;
	while (scale <= myMorphScale) {
		std::cerr << "SCALE: " << scale << std::endl;
		cv::Rect scaled;
		int dx = rect.width * scale / 100;
		int dy = rect.height * scale / 100;

		scaled = rect;
		scaled.x -= dx / 2;
		scaled.y -= dy / 2;
		scaled.width += dx;
		scaled.height += dy;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.x += dx / 2;
		scaled.y -= dy / 2;
		scaled.width -= dx;
		scaled.height += dy;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.x -= dx / 2;
		scaled.y += dy / 2;
		scaled.width += dx;
		scaled.height -= dy;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.x += dx / 2;
		scaled.y += dy / 2;
		scaled.width -= dx;
		scaled.height -= dy;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.x -= dx / 2;
		scaled.width += dx;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.y -= dy / 2;
		scaled.height += dy;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.x += dx / 2;
		scaled.width -= dx;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scaled = rect;
		scaled.y += dy / 2;
		scaled.height -= dy;
		morphnumber = doOffset(image, scaled, base, number, morphnumber, letter);

		scale += myMorphScaleStep;
	}
	return morphnumber;
}

int ExtractProcessor::doOffset(const cv::Mat &image, const cv::Rect &rect, const std::string &base, int number, int morphnumber, char letter) {

	std::cerr << "\tOFFSET: " << 0 << std::endl;
	morphnumber = writeImage(image, rect, base, number, morphnumber, letter);

	int offset = myMorphOffsetStep;
	while (offset <= myMorphOffset) {
		std::cerr << "\tOFFSET: " << offset << std::endl;
		int dx = rect.width * offset / 100;
		int dy = rect.height * offset / 100;
		cv::Rect face;

		face = rect;
		face.x += dx;
		face.y += dy;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.x -= dx;
		face.y += dy;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.x += dx;
		face.y -= dy;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.x -= dx;
		face.y -= dy;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.x += dx;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.y += dy;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.x -= dx;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		face = rect;
		face.y -= dy;
		morphnumber = writeImage(image, face, base, number, morphnumber, letter);

		offset += myMorphOffsetStep;
	}
	return morphnumber;
}


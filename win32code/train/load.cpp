#define CV_NO_BACKWARD_COMPATIBILITY

#include <msxml6.h>

#include <string>
#include <iostream>
#include <sstream>

#include <opencv/cv.h>

#include "model.h"


class ReleaseOnExit {
public:
	ReleaseOnExit(IUnknown &iface) : myIface(iface) {
	}

	~ReleaseOnExit() {
		myIface.Release();
	}

private:
	IUnknown &myIface;
};


class ContentHandler : public ISAXContentHandler {
public:
	ContentHandler(cv::Size &imageSize, cv::PCA &pca, float featuresArg);
	virtual ~ContentHandler();

public:
	HRESULT STDMETHODCALLTYPE putDocumentLocator(ISAXLocator *pLocator);
	HRESULT STDMETHODCALLTYPE startDocument();
	HRESULT STDMETHODCALLTYPE endDocument();
	HRESULT STDMETHODCALLTYPE startPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix, const wchar_t *pwchUri, int cchUri);
	HRESULT STDMETHODCALLTYPE endPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix);
	HRESULT STDMETHODCALLTYPE startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes);
	HRESULT STDMETHODCALLTYPE endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName);
	HRESULT STDMETHODCALLTYPE characters(const wchar_t *pwchChars, int cchChars);
	HRESULT STDMETHODCALLTYPE ignorableWhitespace(const wchar_t *pwchChars, int cchChars);
	HRESULT STDMETHODCALLTYPE processingInstruction(const wchar_t *pwchTarget, int cchTarget, const wchar_t *pwchData, int cchData);
	HRESULT STDMETHODCALLTYPE skippedEntity(const wchar_t *pwchName, int cchName);

private:
	template <class _MatType>
	bool scanBuffer(const std::string &name, cv::Mat &matrix, size_t maxRows);

private:
	cv::Size &myImageSize;
	cv::PCA &myPca;
	std::wstring myBuffer;

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

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

private:
	LONG _Refs;

private: // disable copying
	ContentHandler(const ContentHandler &);
	const ContentHandler &operator = (const ContentHandler &);
};

ContentHandler::ContentHandler(cv::Size &imageSize, cv::PCA &pca, float featuresArg) : myImageSize(imageSize), myPca(pca), _Refs(1) {
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
}

ContentHandler::~ContentHandler() {
}

HRESULT STDMETHODCALLTYPE ContentHandler::QueryInterface(REFIID riid, void **ppvObject) {
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ISAXContentHandler)) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ContentHandler::AddRef() {
	return (ULONG)InterlockedIncrement(&_Refs);
}

ULONG STDMETHODCALLTYPE ContentHandler::Release() {
	ULONG res = (ULONG) InterlockedDecrement(&_Refs);
	if (res == 0) {
		delete this;
	}
	return res;
}

HRESULT STDMETHODCALLTYPE ContentHandler::putDocumentLocator(ISAXLocator *pLocator) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::startDocument() {
	myState = READ_NOTHING;
	if (myLoadCount == 0 && myLoadThreshold == 0.0f) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::endDocument() {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::startPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix, const wchar_t *pwchUri, int cchUri) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::endPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::ignorableWhitespace(const wchar_t *pwchChars, int cchChars) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::processingInstruction(const wchar_t *pwchTarget, int cchTarget, const wchar_t *pwchData, int cchData) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::skippedEntity(const wchar_t *pwchName, int cchName) {
	return S_OK;
}


bool readAttribute(ISAXAttributes *attributes, const std::wstring &name, std::wstring &value) {
	if (attributes == 0) {
		return false;
	}
	const wchar_t *pwchValue;
	int cchValue;
	HRESULT hr = attributes->getValueFromQName(name.c_str(), name.length(), &pwchValue, &cchValue);
	if (FAILED(hr)) {
		return false;
	}
	value.assign(pwchValue, cchValue);
	return true;
}

HRESULT STDMETHODCALLTYPE ContentHandler::startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri,
		const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes) {
	std::wstring tag(pwchQName, cchQName);

	int cols = -1, rows = -1, type = -1;
	std::wstring buf;
	if (readAttribute(pAttributes, L"cols", buf)) {
		swscanf(buf.c_str(), L"%d", &cols);
	}
	if (readAttribute(pAttributes, L"rows", buf)) {
		swscanf(buf.c_str(), L"%d", &rows);
	}
	if (readAttribute(pAttributes, L"type", buf)) {
		swscanf(buf.c_str(), L"%x", &type);
	}

	switch (myState) {
		case READ_NOTHING:
			if (tag.compare(L"pca") == 0) {
				myState = READ_PCA;
			} else {
				return E_FAIL;
			}
			break;
		case READ_PCA:
			if (tag.compare(L"mean") == 0) {
				myState = READ_MEAN;
				myPca.mean.create(rows, cols, CV_32FC1);
			} else if (tag.compare(L"eigenvalues") == 0) {
				myState = READ_EIGENVALUES;
				myPca.eigenvalues.create(rows, cols, CV_32FC1);
			} else if (tag.compare(L"eigenvectors") == 0) {
				myState = READ_EIGENVECTORS;
				myPca.eigenvectors.create(rows, cols, CV_32FC1);
			} else if (tag.compare(L"source") == 0) {
				myState = READ_SOURCE;
				myImageSize.height = rows;
				myImageSize.width = cols;
			} else {
				return E_FAIL;
			}
			break;
		case READ_MEAN:
		case READ_EIGENVALUES:
		case READ_EIGENVECTORS:
			return E_FAIL;
	}

	myBuffer.clear();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName) {
	std::wstring tag(pwchQName, cchQName);

	switch (myState) {
		case READ_NOTHING:
			return E_FAIL;
		case READ_PCA:
			if (tag.compare(L"pca") == 0) {
				myState = READ_NOTHING;
			} else {
				return E_FAIL;
			}
			break;
		case READ_MEAN:
			if (tag.compare(L"mean") == 0) {
				if (!scanBuffer<float>("mean", myPca.mean, -1)) {
					return E_FAIL;
				}
				myState = READ_PCA;
			} else {
				return E_FAIL;
			}
			break;
		case READ_EIGENVALUES:
			if (tag.compare(L"eigenvalues") == 0) {
				size_t maxRows = (myLoadCount == 0) ? -1 : myLoadCount;
				if (!scanBuffer<float>("eigenvalues", myPca.eigenvalues, maxRows)) {
					return E_FAIL;
				}
				if (myLoadCount == 0) {
					const float thresh = myPca.eigenvalues.at<float>(0, 0) * myLoadThreshold;
					while ((int)myLoadCount < myPca.eigenvalues.rows
							&& myPca.eigenvalues.at<float>((int)myLoadCount, 0) >= thresh) {
						++ myLoadCount;
					}
					myLoadThreshold = 0.0f;
					if (myPca.eigenvalues.rows != myLoadCount) {
						cv::Mat tmp;
						myPca.eigenvalues.rowRange(0, myLoadCount).copyTo(tmp);
						tmp.copyTo(myPca.eigenvalues);
					}
				}
				myState = READ_PCA;
			} else {
				return E_FAIL;
			}
			break;
		case READ_EIGENVECTORS:
			if (tag.compare(L"eigenvectors") == 0) {
				if (myLoadCount == 0) {
					return E_FAIL;
				}
				if (!scanBuffer<float>("eigenvectors", myPca.eigenvectors, myLoadCount)) {
					return E_FAIL;
				}
				myState = READ_PCA;
			} else {
				return E_FAIL;
			}
			break;
		case READ_SOURCE:
			if (tag.compare(L"source") == 0) {
				myState = READ_PCA;
			} else {
				return E_FAIL;
			}
			break;
	}

	myBuffer.clear();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ContentHandler::characters(const wchar_t *pwchChars, int cchChars) {
	myBuffer.append(pwchChars, cchChars);
	return S_OK;
}


template <class _MatType>
bool ContentHandler::scanBuffer(const std::string &name, cv::Mat &matrix, size_t maxRows) {
	static const std::wstring spaces = L" \n\r\t";

	size_t beginIndex;
	size_t endIndex = 0;

	if ((size_t)matrix.rows > maxRows) {
		matrix.create(maxRows, matrix.cols, matrix.type());
	}

	std::cout << "Reading matrix \"" << name << "\"" << std::endl;
	const int step = matrix.rows * matrix.cols / 10;

	std::wistringstream iss;
	for (int i = 0; i < matrix.rows; ++i) {
		const int written = i * matrix.cols;
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

			if ((written + j + 1) % step == 0) {
				std::cout << "." << std::flush;
			}
		}
	}
	std::cout << "\n";
	return true;
}



/*
 * featuresArg parameter:
 *    featuresArg <= 0.0f       -- load all available eigenfaces
 *    0.0f < featuresArg < 1.0f -- load eigenfaces for which (eigenvalue >= featuresArg * max(eigenvalue))
 *    featuresArg >= 1.0f       -- load at most ((int) featuresArg) first eigenfaces
 */
bool loadFromFile(const std::string &fileName, cv::Size &imageSize, cv::PCA &pca, float featuresArg) {
	const int len = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, 0, 0);
	if (len <= 0) {
		return false;
	}
	wchar_t *buffer = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, buffer, len);
	std::wstring wfileName(buffer);
	delete[] buffer;

	HRESULT hr = CoInitialize(0);
	if (FAILED(hr)) {
		return false;
	}

	ISAXXMLReader *reader = 0;
	hr = CoCreateInstance(CLSID_SAXXMLReader60, 0, CLSCTX_INPROC_SERVER, IID_ISAXXMLReader, (LPVOID*) &reader);
	if (FAILED(hr)) {
		return false;
	}

	ContentHandler *handler = new ContentHandler(imageSize, pca, featuresArg);

	ReleaseOnExit releaseReader(*reader);
	ReleaseOnExit releaseHandler(*handler);

	hr = reader->putContentHandler(handler);
	if (FAILED(hr)) {
		return false;
	}

	hr = reader->parseURL(wfileName.c_str());
	if (FAILED(hr)) {
		return false;
	}

	if (imageSize.width * imageSize.height != pca.mean.cols) {
		return false;
	}
	return true;
}

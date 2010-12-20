#define CV_NO_BACKWARD_COMPATIBILITY

#include <msxml6.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

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



class ClassesContentHandler : public ISAXContentHandler {
public:
	ClassesContentHandler(std::vector<EmoClass> &classes);
	virtual ~ClassesContentHandler();

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
	std::vector<EmoClass> &myClasses;

	enum {
		READ_NOTHING,
		READ_CLASSES,
		READ_CLASS,
	} myState;

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

private:
	LONG _Refs;

private: // disable copying
	ClassesContentHandler(const ClassesContentHandler &);
	const ClassesContentHandler &operator = (const ClassesContentHandler &);
};

ClassesContentHandler::ClassesContentHandler(std::vector<EmoClass> &classes) :
		myClasses(classes), _Refs(1) {
}

ClassesContentHandler::~ClassesContentHandler() {
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::QueryInterface(REFIID riid, void **ppvObject) {
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ISAXContentHandler)) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ClassesContentHandler::AddRef() {
	return (ULONG)InterlockedIncrement(&_Refs);
}

ULONG STDMETHODCALLTYPE ClassesContentHandler::Release() {
	ULONG res = (ULONG) InterlockedDecrement(&_Refs);
	if (res == 0) {
		delete this;
	}
	return res;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::putDocumentLocator(ISAXLocator *pLocator) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::startDocument() {
	myState = READ_NOTHING;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::endDocument() {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::startPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix, const wchar_t *pwchUri, int cchUri) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::endPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::ignorableWhitespace(const wchar_t *pwchChars, int cchChars) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::processingInstruction(const wchar_t *pwchTarget, int cchTarget, const wchar_t *pwchData, int cchData) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::skippedEntity(const wchar_t *pwchName, int cchName) {
	return S_OK;
}


bool readAttribute(ISAXAttributes *attributes, const std::wstring &name, std::wstring &value);

bool readAttribute(ISAXAttributes *attributes, const std::wstring &name, std::string &value) {
	std::wstring wide;
	if (!readAttribute(attributes, name, wide)) {
		return false;
	}

	if (wide.length() == 0) {
		value.assign("");
		return true;
	}

	const int len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), wide.length(), 0, 0, 0, 0);
	if (len == 0) {
		return false;
	}
	char *buffer = new char[len];
	if (WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), wide.length(), buffer, len, 0, 0) != len) {
		return false;
	}
	value.assign(buffer, len);
	delete[] buffer;
	return true;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri,
		const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes) {
	std::wstring tag(pwchQName, cchQName);

	/*int cols = -1, rows = -1, type = -1;
	std::wstring buf;
	if (readAttribute(pAttributes, L"cols", buf)) {
		swscanf(buf.c_str(), L"%d", &cols);
	}
	if (readAttribute(pAttributes, L"rows", buf)) {
		swscanf(buf.c_str(), L"%d", &rows);
	}
	if (readAttribute(pAttributes, L"type", buf)) {
		swscanf(buf.c_str(), L"%x", &type);
	}*/

	switch (myState) {
		case READ_NOTHING:
			if (tag.compare(L"classes") == 0) {
				myState = READ_CLASSES;
			} else {
				return E_FAIL;
			}
			break;
		case READ_CLASSES:
			if (tag.compare(L"class") == 0) {
				myState = READ_CLASS;
				std::string name, letter;
				if (!readAttribute(pAttributes, L"name", name)
						|| !readAttribute(pAttributes, L"letter", letter)
						|| letter.length() != 1) {
					return E_FAIL;
				}
				std::wstring isDefault;
				if (!readAttribute(pAttributes, L"default", isDefault)) {
					isDefault.assign(L"false");
				}
				myClasses.push_back(EmoClass(name, letter.at(0), isDefault == L"true"));
			} else {
				return E_FAIL;
			}
			break;
		case READ_CLASS:
			return E_FAIL;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName) {
	std::wstring tag(pwchQName, cchQName);

	switch (myState) {
		case READ_NOTHING:
			return E_FAIL;
		case READ_CLASSES:
			if (tag.compare(L"classes") == 0) {
				myState = READ_NOTHING;
			} else {
				return E_FAIL;
			}
			break;
		case READ_CLASS:
			if (tag.compare(L"class") == 0) {
				myState = READ_CLASSES;
			} else {
				return E_FAIL;
			}
			break;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ClassesContentHandler::characters(const wchar_t *pwchChars, int cchChars) {
	return S_OK;
}


bool loadClasses(const std::string &fileName, std::vector<EmoClass> &classes) {
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

	ClassesContentHandler *handler = new ClassesContentHandler(classes);

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

	bool isDefault = false;
	for (size_t i = 0; i < classes.size(); ++i) {
		if (classes[i].Default) {
			if (isDefault) {
				return false;
			}
			isDefault = true;
		}
	}
	if (!isDefault) {
		return false;
	}
	return true;
}

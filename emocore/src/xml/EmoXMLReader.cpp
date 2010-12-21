
#include <string.h>
#include <stdio.h>

#include <algorithm>

#include "EmoXMLReader.h"

#include "expat/EmoXMLReaderInternal.h"



static const size_t BUFFER_SIZE = 2048;

void EmoXMLReader::startElementHandler(const char*, const char**) {
}

void EmoXMLReader::endElementHandler(const char*) {
}

void EmoXMLReader::characterDataHandler(const char*, size_t) {
}




EmoXMLReader::EmoXMLReader(const char *encoding) {
	myInternalReader = new EmoXMLReaderInternal(*this, encoding);
	myParserBuffer = new char[BUFFER_SIZE];
}

EmoXMLReader::~EmoXMLReader() {
	delete[] myParserBuffer;
	delete myInternalReader;
}


bool EmoXMLReader::readDocument(const std::string &fileName) {
	FILE *file = fopen(fileName.c_str(), "rt");
	if (file == 0) {
		return false;
	}

	size_t length;
	bool useWindows1252 = false;
	length = fread(myParserBuffer, sizeof(char), 256, file);
	if (length != 0) {
		myParserBuffer[length] = '\0';
		fseek(file, 0, SEEK_SET);
		char *ptr = strchr(myParserBuffer, '>');
		if (ptr != 0) {
			for (char *p = myParserBuffer; p < ptr; ++p) {
				unsigned char ch = *p;
				*p = tolower(ch);
			}
			*ptr = '\0';
			if (strstr(myParserBuffer, "\"iso-8859-1\"") != 0) {
				useWindows1252 = true;
			}
		}
	}
	initialize(useWindows1252 ? "windows-1252" : 0);

	do {
		length = fread(myParserBuffer, sizeof(char), BUFFER_SIZE, file);
		if (!myInternalReader->parseBuffer(myParserBuffer, length)) {
			break;
		}
	} while ((length == BUFFER_SIZE) && !myInterrupted);

	fclose(file);

	return true;
}

void EmoXMLReader::initialize(const char *encoding) {
	myInternalReader->init(encoding);
	myInterrupted = false;
}


const char *EmoXMLReader::attributeValue(const char **xmlattributes, const char *name) {
	while (*xmlattributes != 0) {
		bool useNext = strcmp(*xmlattributes, name) == 0;
		++xmlattributes;
		if (*xmlattributes == 0) {
			return 0;
		}
		if (useNext) {
			return *xmlattributes;
		}
		++xmlattributes;
	}
	return 0;
}


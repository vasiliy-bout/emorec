
#include <string.h>

#include "EmoXMLReaderInternal.h"
#include "../EmoXMLReader.h"

void EmoXMLReaderInternal::fCharacterDataHandler(void *userData, const char *text, int len) {
	EmoXMLReader &reader = *(EmoXMLReader*)userData;
	if (!reader.isInterrupted()) {
		reader.characterDataHandler(text, len);
	}
}

void EmoXMLReaderInternal::fStartElementHandler(void *userData, const char *name, const char **attributes) {
	EmoXMLReader &reader = *(EmoXMLReader*)userData;
	if (!reader.isInterrupted()) {
		reader.startElementHandler(name, attributes);
	}
}

void EmoXMLReaderInternal::fEndElementHandler(void *userData, const char *name) {
	EmoXMLReader &reader = *(EmoXMLReader*)userData;
	if (!reader.isInterrupted()) {
		reader.endElementHandler(name);
	}
}


EmoXMLReaderInternal::EmoXMLReaderInternal(EmoXMLReader &reader, const char *encoding) : myReader(reader) {
	myParser = XML_ParserCreate(encoding);
	myInitialized = false;
}

EmoXMLReaderInternal::~EmoXMLReaderInternal() {
	XML_ParserFree(myParser);
}

void EmoXMLReaderInternal::init(const char *encoding) {
	if (myInitialized) {
		XML_ParserReset(myParser, encoding);
	}

	myInitialized = true;
	XML_UseForeignDTD(myParser, XML_FALSE);

	XML_SetUserData(myParser, &myReader);
	if (encoding != 0) {
		XML_SetEncoding(myParser, encoding);
	}
	XML_SetStartElementHandler(myParser, fStartElementHandler);
	XML_SetEndElementHandler(myParser, fEndElementHandler);
	XML_SetCharacterDataHandler(myParser, fCharacterDataHandler);
}

bool EmoXMLReaderInternal::parseBuffer(const char *buffer, size_t len) {
	return XML_Parse(myParser, buffer, len, 0) != XML_STATUS_ERROR;
}


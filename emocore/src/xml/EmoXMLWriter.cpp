
#include "EmoXMLWriter.h"


static void writeString(FILE *file, const std::string &str) {
	if (file != 0) {
		fprintf(file, "%s", str.c_str());
	}
}


EmoXMLWriter::Tag::Tag(const std::string &name, bool single) : myName(name), mySingle(single) {
}

void EmoXMLWriter::Tag::addAttribute(const std::string &name, const std::string &value) {
	myAttributes.push_back(Attribute(name, value));
}

void EmoXMLWriter::Tag::addData(const std::string &data) {
	if (!data.empty()) {
		myData += data;
		mySingle = false;
	}
}

static const std::string XML_BANNER = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
static const std::string LANGLE = "<";
static const std::string LANGLE_SLASH = "</";
static const std::string RANGLE = ">";
static const std::string RANGLE_EOL = ">\n";
static const std::string SLASH = "/";
static const std::string SPACE = " ";
static const std::string TWO_SPACES = "  ";
static const std::string QUOTE = "\"";
static const std::string EQUALS_QUOTE = "=\"";

void EmoXMLWriter::Tag::writeStart(FILE *file) const {
	writeString(file, LANGLE);
	writeString(file, myName);
	for (unsigned int i = 0; i < myAttributes.size(); ++i) {
		writeString(file, SPACE);
		writeString(file, myAttributes[i].Name);
		writeString(file, EQUALS_QUOTE);
		writeString(file, myAttributes[i].Value);
		writeString(file, QUOTE);
	}
	if (mySingle) {
		writeString(file, SLASH);
	}
	if (myData.empty()) {
		writeString(file, RANGLE_EOL);
	} else {
		writeString(file, RANGLE);
		writeString(file, myData);
	}
}

void EmoXMLWriter::Tag::writeEnd(FILE *file) const {
	if (!mySingle) {
		writeString(file, LANGLE_SLASH);
		writeString(file, myName);
		writeString(file, RANGLE_EOL);
	}
}

EmoXMLWriter::EmoXMLWriter(const std::string &fileName) {
	myFile = fopen(fileName.c_str(), "wt");
	myCurrentTag = 0;
	writeString(myFile, XML_BANNER);
}

void EmoXMLWriter::addTag(const std::string &name, bool single) {
	flushTagStart();
	myCurrentTag = new Tag(name, single);
}

void EmoXMLWriter::addAttribute(const std::string &name, const std::string &value) {
	if (myCurrentTag != 0) {
		myCurrentTag->addAttribute(name, value);
	}
}

void EmoXMLWriter::addData(const std::string &data) {
	if (myCurrentTag != 0) {
		myCurrentTag->addData(data);
	}
}

void EmoXMLWriter::closeTag() {
	flushTagStart();
	if (myTags.size() > 0) {
		Tag *tag = myTags.top();
		myTags.pop();
		if (tag->isDataEmpty()) {
			for (unsigned int i = 0; i < myTags.size(); ++i) {
				writeString(myFile, TWO_SPACES);
			}
		}
		tag->writeEnd(myFile);
		delete tag;
	}
}

void EmoXMLWriter::closeAllTags() {
	while (myTags.size() > 0) {
		closeTag();
	}
}

void EmoXMLWriter::flushTagStart() {
	if (myCurrentTag != 0) {
		for (unsigned int i = 0; i < myTags.size(); ++i) {
			writeString(myFile, TWO_SPACES);
		}
		myCurrentTag->writeStart(myFile);
		if (!myCurrentTag->isSingle()) {
			myTags.push(myCurrentTag);
		} else {
			delete myCurrentTag;
		}
		myCurrentTag = 0;
	}
}


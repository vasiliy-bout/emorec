
#include <string.h>

#include "EmoClassesReader.h"
#include "EmoErrors.h"



bool EmoClassesReader::loadClasses(const std::string &fileName, std::vector<EmoClass> &classes) {
	EmoClassesReader reader(classes);

	if (!reader.readDocument(fileName)) {
		return false;
	}
	if (reader.isInterrupted()) {
		return false;
	}

	bool isDefault = false;
	for (size_t i = 0; i < classes.size(); ++i) {
		if (classes[i].isDefault()) {
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

EmoClassesReader::EmoClassesReader(std::vector<EmoClass> &classes) : myClasses(classes) {
	myState = READ_NOTHING;
}



static const std::string TAG_ROOT = "classes";
static const std::string TAG_CLASS = "class";

static const std::string STR_TRUE = "true";


void EmoClassesReader::startElementHandler(const char *tag, const char **attributes) {
	switch (myState) {
	case READ_NOTHING:
		if (TAG_ROOT.compare(tag) == 0) {
			myState = READ_CLASSES;
		} else {
			interrupt();
		}
		break;
	case READ_CLASSES:
		if (TAG_CLASS.compare(tag) == 0) {
			myState = READ_CLASS;
			const char *name = attributeValue(attributes, "name");
			const char *letter = attributeValue(attributes, "letter");
			if (name == 0 || letter == 0 || strlen(letter) != 1) {
				interrupt();
			} else {
				const char *isDefault = attributeValue(attributes, "default");
				myClasses.push_back(EmoClass(name, letter[0], isDefault != 0 && STR_TRUE.compare(isDefault) == 0));
			}
		} else {
			interrupt();
		}
		break;
	case READ_CLASS:
		interrupt();
		break;
	}
}

void EmoClassesReader::endElementHandler(const char *tag) {
	switch (myState) {
	case READ_NOTHING:
		interrupt();
		break;
	case READ_CLASSES:
		if (TAG_ROOT.compare(tag) == 0) {
			myState = READ_NOTHING;
		} else {
			interrupt();
		}
		break;
	case READ_CLASS:
		if (TAG_CLASS.compare(tag) == 0) {
			myState = READ_CLASSES;
		} else {
			interrupt();
		}
		break;
	}
}


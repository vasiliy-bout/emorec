
#include <string.h>

#include "EmoConfigReader.h"
#include "EmoErrors.h"


bool EmoConfigReader::loadConfig(const std::string &fileName, std::map<std::string, std::string> &properties) {
	EmoConfigReader reader(properties);
	if (!reader.readDocument(fileName)) {
		return false;
	}
	return !reader.isInterrupted();
}

EmoConfigReader::EmoConfigReader(std::map<std::string, std::string> &properties) : myProperties(properties) {
	myState = READ_NOTHING;
}



static const std::string TAG_ROOT = "config";
static const std::string TAG_PROPERTY = "property";


void EmoConfigReader::startElementHandler(const char *tag, const char **attributes) {
	switch (myState) {
	case READ_NOTHING:
		if (TAG_ROOT.compare(tag) == 0) {
			myState = READ_CONFIG;
		} else {
			interrupt();
		}
		break;
	case READ_CONFIG:
		if (TAG_PROPERTY.compare(tag) == 0) {
			myState = READ_PROPERTY;
			const char *name = attributeValue(attributes, "name");
			const char *value = attributeValue(attributes, "value");
			if (name == 0 || value == 0) {
				interrupt();
			} else {
				std::string nameStr(name);
				std::string valueStr(value);
				myProperties[name] = value;
			}
		} else {
			interrupt();
		}
		break;
	case READ_PROPERTY:
		interrupt();
		break;
	}
}

void EmoConfigReader::endElementHandler(const char *tag) {
	switch (myState) {
	case READ_NOTHING:
		interrupt();
		break;
	case READ_CONFIG:
		if (TAG_ROOT.compare(tag) == 0) {
			myState = READ_NOTHING;
		} else {
			interrupt();
		}
		break;
	case READ_PROPERTY:
		if (TAG_PROPERTY.compare(tag) == 0) {
			myState = READ_CONFIG;
		} else {
			interrupt();
		}
		break;
	}
}


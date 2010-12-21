
#include "EmoClass.h"

EmoClass::EmoClass() : myName(""), myLetter('\0'), myDefault(false) {
}

EmoClass::EmoClass(const std::string &name, unsigned char letter, bool isDefault) :
		myName(name), myLetter(letter), myDefault(isDefault) {
}

EmoClass::EmoClass(const EmoClass &cls) :
		myName(cls.myName), myLetter(cls.myLetter), myDefault(cls.myDefault) {
}

const EmoClass &EmoClass::operator = (const EmoClass &cls) {
	myName = cls.myName;
	myLetter = cls.myLetter;
	myDefault = cls.myDefault;
	return cls;
}


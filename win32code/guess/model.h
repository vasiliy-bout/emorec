
#ifndef __MODEL_H__
#define __MODEL_H__

#include <string>


struct EmoClass {

	EmoClass() : Name(""), Letter('\0') {
	}

	EmoClass(const std::string &name, unsigned char letter, bool isDefault) :
			Name(name), Letter(letter), Default(isDefault) {
	}

	std::string Name;
	unsigned char Letter;
	bool Default;
};

#endif /* __MODEL_H__ */
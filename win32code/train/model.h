
#ifndef __MODEL_H__
#define __MODEL_H__

#include <string>


struct EmoClass {

	EmoClass() : Name(""), Letter('\0') {
	}

	EmoClass(const std::string &name, unsigned char letter) : Name(name), Letter(letter) {
	}

	std::string Name;
	unsigned char Letter;
};

#endif /* __MODEL_H__ */
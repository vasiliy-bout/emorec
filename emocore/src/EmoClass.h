
#ifndef __EMOCLASS_H__
#define __EMOCLASS_H__

#include <string>

class EmoClass {

public:
	EmoClass();
	EmoClass(const std::string &name, unsigned char letter, bool isDefault);
	EmoClass(const EmoClass &cls);

	const EmoClass &operator = (const EmoClass &cls);

	const std::string &name() const;
	unsigned char letter() const;
	bool isDefault() const;

private:
	std::string myName;
	unsigned char myLetter;
	bool myDefault;
};

inline const std::string &EmoClass::name() const { return myName; }
inline unsigned char EmoClass::letter() const { return myLetter; }
inline bool EmoClass::isDefault() const { return myDefault; }

#endif /* __EMOCLASS_H__ */

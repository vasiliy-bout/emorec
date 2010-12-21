
#ifndef __EMOXMLREADERINTERNAL_H__
#define __EMOXMLREADERINTERNAL_H__

#include <expat.h>

#include <set>

class EmoXMLReader;

class EmoXMLReaderInternal {

private:
	static void fStartElementHandler(void *userData, const char *name, const char **attributes);
	static void fEndElementHandler(void *userData, const char *name);
	static void fCharacterDataHandler(void *userData, const char *text, int len);

public:
	EmoXMLReaderInternal(EmoXMLReader &reader, const char *encoding);
	~EmoXMLReaderInternal();
	void init(const char *encoding = 0);
	bool parseBuffer(const char *buffer, size_t len);

private:
	EmoXMLReader &myReader;
	XML_Parser myParser;
	bool myInitialized;
};

#endif /* __EMOXMLREADERINTERNAL_H__ */


#ifndef __EMOCLASSESREADER_H__
#define __EMOCLASSESREADER_H__

#include <vector>

#include <EmoXMLReader.h>
#include <EmoClass.h>


class EmoClassesReader : public EmoXMLReader {

	static bool loadClasses(const std::string &fileName, std::vector<EmoClass> &classes);

private:
	EmoClassesReader(std::vector<EmoClass> &classes);

public:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);

private:
	std::vector<EmoClass> &myClasses;

	enum {
		READ_NOTHING,
		READ_CLASSES,
		READ_CLASS,
	} myState;
};


#endif /* __EMOCLASSESREADER_H__ */


#ifndef __EMOCONFIGREADER_H__
#define __EMOCONFIGREADER_H__

#include <map>

#include <EmoXMLReader.h>



class EmoConfigReader : public EmoXMLReader {

	static bool loadClasses(const std::string &fileName, std::map<std::string, std::string> &properties);

private:
	EmoConfigReader(std::map<std::string, std::string> &properties);

public:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);

private:
	std::map<std::string, std::string> &myProperties;

	enum {
		READ_NOTHING,
		READ_CONFIG,
		READ_PROPERTY,
	} myState;
};


#endif /* __EMOCONFIGREADER_H__ */


#ifndef __EMOXMLREADER_H__
#define __EMOXMLREADER_H__

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>


class EmoXMLReaderInternal;

class EmoXMLReader {

protected:
	EmoXMLReader(const char *encoding = 0);

public:
	virtual ~EmoXMLReader();

	bool readDocument(const std::string &fileName);

	const char *attributeValue(const char **xmlattributes, const char *name);

private:
	void initialize(const char *encoding = 0);

protected:
	virtual void startElementHandler(const char *tag, const char **attributes);
	virtual void endElementHandler(const char *tag);
	virtual void characterDataHandler(const char *text, size_t len);

	bool isInterrupted() const;

protected:
	void interrupt();

private:
	bool myInterrupted;
	EmoXMLReaderInternal *myInternalReader;
	char *myParserBuffer;

friend class EmoXMLReaderInternal;
};

inline bool EmoXMLReader::isInterrupted() const {
	return myInterrupted;
}

inline void EmoXMLReader::interrupt() {
	myInterrupted = true;
}

#endif /* __EMOXMLREADER_H__ */

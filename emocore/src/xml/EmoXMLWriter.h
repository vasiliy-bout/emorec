
#ifndef __EMOXMLWRITER_H__
#define __EMOXMLWRITER_H__

#include <stack>
#include <vector>
#include <string>

#include <stdio.h>


class EmoXMLWriter {

private:
	struct Attribute {
		std::string Name;
		std::string Value;

		Attribute(const std::string &name, const std::string &value);
		~Attribute();
	};

	class Tag {

	public:
		Tag(const std::string &name, bool single);
		~Tag();
		void addAttribute(const std::string &name, const std::string &value);
		void addData(const std::string &data);
		bool isSingle() const;
		bool isDataEmpty() const;

		void writeStart(FILE *file) const;
		void writeEnd(FILE *file) const;

	private:
		std::string myName;
		std::string myData;
		bool mySingle;
		std::vector<Attribute> myAttributes;
	};

protected:
	EmoXMLWriter(const std::string &fileName);
	virtual ~EmoXMLWriter();

	void addTag(const std::string &name, bool single);
	void addAttribute(const std::string &name, const std::string &value);
	void addData(const std::string &data);
	void closeTag();
	void closeAllTags();

private:
	void flushTagStart();

private:
	FILE *myFile;
	Tag *myCurrentTag;
	std::stack<Tag*> myTags;
};

inline EmoXMLWriter::Attribute::Attribute(const std::string &name, const std::string &value) : Name(name), Value(value) {}
inline EmoXMLWriter::Attribute::~Attribute() {}

inline bool EmoXMLWriter::Tag::isSingle() const { return mySingle; }
inline bool EmoXMLWriter::Tag::isDataEmpty() const { return myData.empty(); }
inline EmoXMLWriter::Tag::~Tag() {}

inline EmoXMLWriter::~EmoXMLWriter() {}

#endif /* __EMOXMLWRITER_H__ */

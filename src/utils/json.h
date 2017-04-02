#pragma once
#include <string.h>
#include <vector>
#include "..\..\..\diesel\diesel.h"
#include "Stack.h"
#include "DynamicArray.h"
#include "BlockArray.h"

struct File {

	ds::StaticHash name;
	char* data;
	uint32_t size;

	File() : name((uint32_t)0), data(0), size(0) {}

	explicit File(const ds::StaticHash& n) : name(n), data(0), size(0) {}

	~File() {
		if (data != 0) {
			delete[] data;
		}
	}
};

struct Token {

	enum TokenType { 
		EMPTY, 
		NUMBER, 
		NAME, 
		DELIMITER, 
		SEPARATOR, 
		OPEN_BRACES, 
		CLOSE_BRACES, 
		STRING, 
		ASSIGN, 
		SEMICOLON,
		OPEN_BRACKET,
		CLOSE_BRACKET,
		SLASH,
		NEWLINE,
		PLUS,
		MINUS,
		DIVISION,
		MULTIPLY 
	};

	Token() :type(EMPTY) , value(0.0f) , index(0) , size(0) {}
	Token(TokenType type) : type(type) , value(0.0f) , index(0), size(0) {}
	Token(TokenType type, float v) : type(type), value(v) , index(0), size(0) {}
	Token(TokenType type, int i, int s) : type(type), index(i), size(s) , value(0.0f) {}

	TokenType type;
	float value;
	int index;
	int size;
};

class Tokenizer {

public:
	Tokenizer() : UNKNOWN_TOKEN(Token::EMPTY) {}
	~Tokenizer() {}
	void parseFile(const char* fileName, bool skipComments = true);
	void parse(const char* text,bool skipComments = true);
	int size() const {
		return _tokens.size();
	}
	const Token& get(uint32_t index) const {
		if (index < _tokens.size()) {
			return _tokens[index];
		}
		else {
			return UNKNOWN_TOKEN;
		}
	}

	Token& get(uint32_t index) {
		if (index < _tokens.size()) {
			return _tokens[index];
		}
		else {
			return UNKNOWN_TOKEN;
		}
	}
	bool checkType(int index, Token::TokenType type) const;
	const char* name(int index) const;
private:
	std::vector<Token> _tokens;
	Token UNKNOWN_TOKEN;
};

class SJSONReader {

	struct CategoryEntry {
		int text_index;
		int text_length;
	};

	struct Property {
		ds::StaticHash hash;
		int index;
		int num;
	};

public:
	SJSONReader();
	~SJSONReader() {}
	bool parse(const char* fileName);
	bool get_float(const char* name, float* ret) const;
	bool get(const char* name, float* ret) const;
	bool get(const char* name, int* ret) const;
	bool get(const char* name, ds::vec4* ret) const;
	bool get(const char* name, ds::Color* ret) const;
	bool get(const char* name, ds::vec3* ret) const;
	bool get(const char* name, ds::vec2* ret) const;
	//bool get(const char* name, V3Path* ret) const;
	bool contains(const char* name) const;	
private:
	void add(int pIndex, float value);
	void add(int pIndex, const char* c, int len);
	void add(int pIndex, char c);
	int add_category(const char* name);
	void buildName(const Stack<CategoryEntry>& stack, char* buffer);
	int create_property(const char* name);
	void alloc(int elements);
	float get(int index) const;
	int get_index(const char* name) const;
	Array<char> _name_buffer;
	BlockArray _data_buffer;
	ds::StaticHash* _data_keys;
	int* _data_indices;
	int* _data_sizes;
	Array<char> _values;
};


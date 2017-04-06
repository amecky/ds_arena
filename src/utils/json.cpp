#include "json.h"
#include <stdio.h>
#include <string.h>
#include <stack>

namespace string {

	bool isDigit(const char* c) {
		if ((*c >= '0' && *c <= '9') || *c == '.') {
			return true;
		}
		if (*c == '-' || *c == '+') {
			const char* n = c + 1;
			if (n) {
				return (*n >= '0' && *n <= '9');
			}
		}
		return false;
	}

	bool isNumeric(const char c) {
		return ((c >= '0' && c <= '9'));
	}

	bool isWhitespace(const char c) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			return true;
		}
		return false;
	}

	float strtof(const char* p, char** endPtr) {
		while (isWhitespace(*p)) {
			++p;
		}
		float sign = 1.0f;
		if (*p == '-') {
			sign = -1.0f;
			++p;
		}
		else if (*p == '+') {
			++p;
		}
		float value = 0.0f;
		while (isNumeric(*p)) {
			value *= 10.0f;
			value = value + (*p - '0');
			++p;
		}
		if (*p == '.') {
			++p;
			float dec = 1.0f;
			float frac = 0.0f;
			while (isNumeric(*p)) {
				frac *= 10.0f;
				frac = frac + (*p - '0');
				dec *= 10.0f;
				++p;
			}
			value = value + (frac / dec);
		}
		if (endPtr) {
			*endPtr = (char *)(p);
		}
		return value * sign;
	}

}

static void loadFile(const char* fileName, File* file) {
	FILE *fp = fopen(fileName, "r");
	if (fp) {
		//file->loaded = true;
		fseek(fp, 0, SEEK_END);
		int sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		//LOG << "size: " << sz;
		file->data = new char[sz + 1];
		fread(file->data, 1, sz, fp);
		file->data[sz] = '\0';
		fclose(fp);
		file->size = sz;
	}
}

void Tokenizer::parseFile(const char* fileName, bool skipComments) {
	File f;
	loadFile(fileName, &f);
	if (f.size > 0) {
		parse(f.data, skipComments);
	}
}

void Tokenizer::parse(const char* text, bool skipComments) {
	_data = text;
	int cnt = 0;
	const char* p = text;
	while (*p != 0) {
		Token token(Token::EMPTY);
		if (string::isDigit(p)) {
			char *out;
			token = Token(Token::NUMBER, string::strtof(p, &out));
			p = out;
		}
		else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_')) {
			const char *identifier = p;
			while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_') || (*p >= '0' && *p <= '9'))
				p++;
			token = Token(Token::NAME, identifier - text, p - identifier);
		}
		else if (*p == '"') {
			const char *identifier = p + 1;
			++p;
			while (*p != '"') {
				p++;
			}
			token = Token(Token::STRING, identifier - text, p - identifier);
			++p;
		}
		else if (*p == '/' && *(p+1) == '/' && skipComments) {
			++p;
			if (*p == '/') {
				++p;
				while (*p != '\n') {
					++p;
					if (*p == 0) {
						break;
					}
				}
				++p;
				if (*p == 0) {
					break;
				}
			}
		}
		else {
			switch (*p) {
				case '{': token = Token(Token::OPEN_BRACES); break;
				case '}': token = Token(Token::CLOSE_BRACES); break;
				case '(': token = Token(Token::OPEN_BRACKET); break;
				case ')': token = Token(Token::CLOSE_BRACKET); break;
				case '+': token = Token(Token::PLUS); break;
				case '-': token = Token(Token::MINUS); break;
				case '*': token = Token(Token::MULTIPLY); break;
				case '/': token = Token(Token::SLASH); break;
				case ' ': case '\t': case '\r': break;
				case '\n': token = Token(Token::NEWLINE); break;
				case ':': token = Token(Token::SEPARATOR); break;
				case '=': token = Token(Token::ASSIGN); break;
				case ';': token = Token(Token::SEMICOLON); break;
				case ',': token = Token(Token::DELIMITER); break;
			}
			++p;
		}
		if (token.type != Token::EMPTY) {
			_tokens.push_back(token);
		}
	}
}

const char* Tokenizer::name(int index) const {
	const Token& token = _tokens[index];
	switch (token.type) {
		case Token::NAME: return "NAME"; break;
		case Token::NUMBER: return "NUMBER"; break;
		case Token::DELIMITER: return "DELIMITER"; break;
		case Token::SEPARATOR: return "SEPARATOR"; break;
		case Token::OPEN_BRACES: return "OPEN_BRACES"; break;
		case Token::CLOSE_BRACES: return "CLOSE_BRACES"; break;
		case Token::STRING: return "STRING"; break;
		case Token::ASSIGN: return "ASSIGN"; break;
		case Token::SEMICOLON: return "SEMICOLON"; break;
		case Token::OPEN_BRACKET: return "OPEN_BRACKET"; break;
		case Token::CLOSE_BRACKET: return "CLOSE_BRACKET"; break;
		case Token::SLASH: return "SLASH"; break;
		case Token::NEWLINE: return "NEWLINE"; break;
		case Token::PLUS: return "PLUS"; break;
		case Token::MINUS: return "MINUS"; break;
		case Token::MULTIPLY: return "MULITPLY"; break;
		default: return "UNKNOWN"; break;
	}
}

bool Tokenizer::checkType(int index, Token::TokenType type) const {
	if (index < _tokens.size()) {
		return _tokens[index].type == type;
	}
	return false;
}

void Tokenizer::saveTokens(const char* fileName) {
	FILE* fp = fopen(fileName, "w");
	for (int i = 0; i < _tokens.size(); ++i) {
		const Token& t = _tokens[i];
		fprintf(fp, "%s : ", name(i));
		if (t.type == Token::NAME) {
			char tmp[128];
			strncpy(tmp, _data + t.index, t.size);
			tmp[t.size] = '\0';
			fprintf(fp, "%s", tmp);
		}
		else if (t.type == Token::NUMBER) {
			fprintf(fp, "%g", t.value);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

SJSONReader::SJSONReader() {
	int data_sizes[] = { sizeof(unsigned int), sizeof(int), sizeof(int), sizeof(int) };
	_data_buffer.init(data_sizes, 4);
}
// -------------------------------------------
// add category
// -------------------------------------------
int SJSONReader::add_category(const char* name) {
	int idx = _name_buffer.size;
	int l = strlen(name);
	if (_name_buffer.size + l > _name_buffer.capacity) {
		_name_buffer.resize(_name_buffer.capacity + 256);
	}
	_name_buffer.append(name, l);
	return idx;
}

void SJSONReader::buildName(const Stack<CategoryEntry>& stack, char* buffer) {
	std::string tmp;
	char b[128];
	for (int i = stack.size() - 1; i >= 0; --i) {
		const CategoryEntry& c = stack.at(i);
		strncpy(b, _name_buffer.data + c.text_index, c.text_length);
		b[c.text_length] = '\0';
		tmp.append(b);
		tmp.append(".");
	}
	strcpy(buffer, tmp.c_str());
}

// -------------------------------------------
// allocate data buffer
// -------------------------------------------
void SJSONReader::alloc(int elements) {
	if (_data_buffer.resize(elements)) {
		_data_keys = (ds::StaticHash*)_data_buffer.get_ptr(0);
		_data_indices = (int*)_data_buffer.get_ptr(1);
		_data_sizes = (int*)_data_buffer.get_ptr(2);
	}
}

// -------------------------------------------
// create property
// -------------------------------------------
int SJSONReader::create_property(const char* name) {
	if (_data_buffer.size + 1 > _data_buffer.capacity) {
		alloc(_data_buffer.size * 2 + 8);
	}
	_data_keys[_data_buffer.size] = ds::StaticHash(name);
	_data_indices[_data_buffer.size] = _values.num;
	_data_sizes[_data_buffer.size] = 0;
	++_data_buffer.size;
	return _data_buffer.size - 1;
}

// -------------------------------------------
// add value to property
// -------------------------------------------
void SJSONReader::add(int pIndex, float value) {
	float* v = (float*)_values.alloc(sizeof(float));
	*v = value;
	++_data_sizes[pIndex];
}

// -------------------------------------------
// add string to property
// -------------------------------------------
void SJSONReader::add(int pIndex, const char* c, int len) {
	int sz = ((len + 4) / 4) * 4;
	char* v = (char*)_values.alloc(sz * sizeof(char));
	for (int i = 0; i < len; ++i) {
		*v = c[i];
		++v;
	}
	*v = '\0';
	_data_sizes[pIndex] = sz / 4;
}

// -------------------------------------------
// add char to property
// -------------------------------------------
void SJSONReader::add(int pIndex, char c) {
	char* v = (char*)_values.alloc(sizeof(char));
	*v = c;
	++_data_sizes[pIndex];
}

// -------------------------------------------
// get index
// -------------------------------------------
int SJSONReader::get_index(const char* name, const char* prefix) const {
	ds::StaticHash key(name);
	if (prefix != 0) {
		char tmp[128];
		sprintf_s(tmp, 128, "%s.%s", prefix, name);
		key = ds::StaticHash(tmp);
	}
	for (int i = 0; i < _data_buffer.size; ++i) {
		if (_data_keys[i].get() == key.get()) {
			return i;
		}
	}
	return -1;
}

// -------------------------------------------
// get float value
// -------------------------------------------
float SJSONReader::get(int index) const {
	char* p = _values.data + index * sizeof(float);
	float* v = (float*)(p);
	return *v;
}

// -------------------------------------------
// get float
// -------------------------------------------
bool SJSONReader::get(const char* name, float* ret, const char* prefix) const {
	int idx = get_index(name, prefix);
	if (idx != -1) {
		*ret = get(_data_indices[idx]);
		return true;
	}
	return false;
}

// -------------------------------------------
// get int
// -------------------------------------------
bool SJSONReader::get(const char* name, int* ret, const char* prefix) const {
	int idx = get_index(name, prefix);
	if (idx != -1) {
		*ret = static_cast<int>(get(_data_indices[idx]));
		return true;
	}
	return false;
}

// -------------------------------------------
// get int
// -------------------------------------------
bool SJSONReader::get(const char* name, uint16_t* ret, const char* prefix) const {
	int idx = get_index(name, prefix);
	if (idx != -1) {
		*ret = static_cast<int>(get(_data_indices[idx]));
		return true;
	}
	return false;
}

bool SJSONReader::get(const char* name, ds::Color* ret, const char* prefix) const {
	int idx = get_index(name, prefix);
	if (idx != -1) {
		ret->r = get(_data_indices[idx]) / 255.0f;
		ret->g = get(_data_indices[idx] + 1) / 255.0f;
		ret->b = get(_data_indices[idx] + 2) / 255.0f;
		ret->a = get(_data_indices[idx] + 3) / 255.0f;
		return true;
	}
	return false;
}

bool SJSONReader::get(const char* name, ds::vec2* ret, const char* prefix) const {
	int idx = get_index(name, prefix);
	if (idx != -1) {
		ret->x = get(_data_indices[idx]);
		ret->y = get(_data_indices[idx] + 1);
		return true;
	}
	return false;
}

// -------------------------------------------
// get rect
// -------------------------------------------
bool SJSONReader::get(const char* name, ds::vec4* ret, const char* prefix) const {
	int idx = get_index(name, prefix);
	if (idx != -1) {
		ret->x = get(_data_indices[idx]);
		ret->y = get(_data_indices[idx] + 1);
		ret->z = get(_data_indices[idx] + 2);
		ret->w = get(_data_indices[idx] + 3);
		return true;
	}
	return false;
}

// -------------------------------------------
// contains
// -------------------------------------------
bool SJSONReader::contains(const char* name) const {
	return get_index(name) != -1;
}

// -------------------------------------------
// parse
// -------------------------------------------
bool SJSONReader::parse(const char* fileName) {
	File f(SID(fileName));
	loadFile(fileName, &f);
	Tokenizer tokenizer;
	tokenizer.parse(f.data);
	//tokenizer.saveTokens("tokens.log");
	char name[128];
	char fullName[256];
	int n = 0;
	int category_index = 0;
	int cat = -1;
	Stack<CategoryEntry> cat_stack;
	char cat_name[256];
	while (n < tokenizer.size()) {
		Token& t = tokenizer.get(n);
		if (t.type == Token::NAME) {
			if (tokenizer.checkType(n + 1,Token::OPEN_BRACES)) {
				++n;
				strncpy(name, f.data + t.index, t.size);
				name[t.size] = '\0';
				CategoryEntry cat;
				cat.text_index = add_category(name);
				cat.text_length = strlen(name);
				cat_stack.push(cat);
				buildName(cat_stack, cat_name);
				++n;
			}
			else if (tokenizer.checkType(n + 1,Token::SEPARATOR)) {
				++n;
				strncpy(name, f.data + t.index, t.size);
				name[t.size] = '\0';
				sprintf(fullName, "%s%s", cat_name, name);
				int p = create_property(fullName);
				++n;
				Token& v = tokenizer.get(n);
				if (v.type == Token::STRING) {
					int start = v.index;
					int end = v.index + v.size;
					add(p, f.data + start, v.size);
					++n;
				}
				else if (v.type == Token::NAME) {
					strncpy(name, f.data + v.index, v.size);
					name[v.size] = '\0';
					if (strcmp(name, "true") == 0) {
						add(p, 1.0f);
					}
					else {
						add(p, 0.0f);
					}
					++n;
				}
				else {
					bool parsing = true;
					while (parsing) {
						if (v.type == Token::NUMBER) {
							add(p, v.value);
							if (!tokenizer.checkType(n + 1,Token::DELIMITER)) {
								parsing = false;
							}
						}
						++n;
						v = tokenizer.get(n);

					}
				}
			}
		}
		else if (t.type == Token::CLOSE_BRACES) {
			if (!cat_stack.empty()) {
				cat_stack.pop();
			}
			buildName(cat_stack, cat_name);
			++n;
		}
		else {
			++n;
		}
		t = tokenizer.get(n);
	}
	return true;
}



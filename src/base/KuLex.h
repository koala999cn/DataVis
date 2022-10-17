#pragma once
#include <vector>
#include <stdlib.h>
#include <string>


class KuLex
{
public:

	/********** Baisc categories. **********/

	/* space \t */
	static int isBlank(char ch) { return ::isblank(ch); }
	static int isBlach(wchar_t ch) { return ::iswblank(ch); }

	/* nonprinting characters */
	static int isCntrl(char ch) { return ::iscntrl(ch); }
	static int isCntrl(wchar_t ch) { return ::iswcntrl(ch); }

	/* 0-9 */
	static int isDigit(char ch) { return ::isdigit(ch); }
	static int isDigit(wchar_t ch) { return ::iswdigit(ch); }

	/* a-z */
	static int isLower(char ch) { return ::islower(ch); }
	static int isLower(wchar_t ch) { return ::iswlower(ch); }

	/* any printing character including ' ' */
	static int isPrint(char ch) { return ::isprint(ch); }
	static int isPrint(wchar_t ch) { return ::iswprint(ch); }

	/* all punctuation */
	static int isPunct(char ch) { return ::ispunct(ch); }
	static int isPunct(wchar_t ch) { return ::iswpunct(ch); }

	/* space \t \n \r \f \v */
	static int isSpace(char ch) { return ::isspace(unsigned char(ch)); }
	static int isSpace(wchar_t ch) { return ::iswspace(ch); }

	/* A-Z */
	static int isUpper(char ch) { return ::isupper(ch); }
	static int isUpper(wchar_t ch) { return ::iswupper(ch); }

	/* 0-9A-Fa-f */
	static int isXDigit(char ch) { return ::isxdigit(ch); }
	static int isXDigit(wchar_t ch) { return ::iswxdigit(ch); }

	/********** Extra useful categories.  **********/
	
	/* A-Za-z_ */
	static int isIdst(char ch) { return ch == '_' || isAlpha(ch); }
	static int isIdst(wchar_t ch) { return ch == L'_' || isAlpha(ch); }

	/* \n \r */
	static int isVSpace(char ch) { return ch == '\r' || ch == '\n'; }
	static int isVSpace(wchar_t ch) { return ch == L'\r' || ch == L'\n'; }

	/* space \t \f \v \0 */
	static int isNvSpace(char ch) { return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v' || ch == '\0'; }
	static int isNvSpace(wchar_t ch) { return ch == L' ' || ch == L'\t' || ch == L'\f' || ch == L'\v' || ch == L'\0'; }

	/********** Combinations of the above.  **********/

	/* A-Za-z */
	static int isAlpha(char ch) { return ::isalpha(ch); } // _sch_isupper|_sch_islower
	static int isAlpha(wchar_t ch) { return ::iswalpha(ch); }

	/* A-Za-z0-9 */
	static int isAlNum(char ch) { return ::isalnum(ch); } // _sch_isalpha|_sch_isdigit
	static int isAlNum(wchar_t ch) { return ::iswalnum(ch); } //

	/* A-Za-z0-9_ */
	static int isIdNum(char ch) { return isIdst(ch) || isDigit(ch); } // _sch_isidst|_sch_isdigit
	static int isIdNum(wchar_t ch) { return isIdst(ch) || isDigit(ch); }

	/* isprint and not space */
	static int isGraph(char ch) { return ::isgraph(ch); } // _sch_isalnum|_sch_ispunct
	static int isGraph(wchar_t ch) { return ::iswgraph(ch); }

	/* isspace + \0 */
	static int isSpaceOrNul(char ch) { return ch == '\0' || isSpace(ch); } // _sch_isvsp|_sch_isnvsp
	static int isSpaceOrNul(wchar_t ch) { return ch == L'\0' || isSpace(ch); }

	/* basic charset of ISO C (plus ` and @)  */
	static int isIsoBasic(char ch) { return isPrint(ch) || isSpaceOrNul(ch); } // _sch_isprint|_sch_iscppsp
	static int isIsoBasic(wchar_t ch) { return isPrint(ch) || isSpaceOrNul(ch); }

	/// isIdNum的变体，增加了对$的支持
	static int isIdChar(char ch, bool bDollarOk) { return isIdNum(ch) || (ch == '$' && bDollarOk); }
	static int isIdChar(wchar_t ch, bool bDollarOk) { return isIdNum(ch) || (ch == L'$' && bDollarOk); }

	/// isIdst的变体，增加了对$的支持
	static int isIdStart(char ch, bool bDollarOk) { return isIdst(ch) || (ch == '$' && bDollarOk); }
	static int isIdStart(wchar_t ch, bool bDollarOk) { return isIdst(ch) || (ch == L'$' && bDollarOk); }


	// 是否数字，包括整数和浮点数，兼容nan、inf
	static bool isNumeric(const char *tag);

	// 是否整数
	static bool isInteger(const char* tag);


	// return -1 if c is not a valid xdigit
	static int hexValue(char c);


	template<typename CHAR, typename PRED>
	static void skipUntil(std::basic_istream<CHAR>& is, PRED pred, bool unget = true) {
		while (is) {
			if (pred(is.get())) {
				if (unget) is.unget();
				break;
			}
		}
	}


	template<typename CHAR, typename PRED>
	static std::basic_string<CHAR> getUntil(std::basic_istream<CHAR>& is, PRED pred, bool unget = true) {
		std::basic_string<CHAR> res;
		while (is) {
			auto ch = is.get();
			if (pred(ch)) {
				if (unget) is.unget();
				break;
			}
			
			res.push_back(ch);
		}

		return res;
	}


	template<typename CHAR = char>
	static void skipUntil(std::basic_istream<CHAR>& is, CHAR ch, bool unget = true) {
		skipUntil(is, [ch](CHAR c) { return c == ch; }, unget);
	}


	template<typename CHAR>
	static std::basic_string<CHAR> getUntil(std::basic_istream<CHAR>& is, CHAR ch, bool unget = true) {
		return getUntil(is, [ch](CHAR c) { return c == ch; }, unget);
	}


	template<typename CHAR = char>
	static void skipLine(std::basic_istream<CHAR>& is) {
		std::basic_string<CHAR> line;
		std::getline(is, line);
	}


	template<typename CHAR = char>
	static void skipWhitespace(std::basic_istream<CHAR>& is) {
		skipUntil(is, [](CHAR c){ return !isSpace(c); }, true);
	}


	template<typename CHAR = char>
	static void skipBlank(std::basic_istream<CHAR>& is) {
		skipUntil<CHAR>(is, [](CHAR c){ return !isBlank(c); }, true);
	}


	// 目前token定义为3种形式：一是连续字符串（无空白），二是被"或'引起来的字符串（中间可有空白字符），
	// 三是idname和非idname
	// @forceSpace，若为真，则仅把space当做token的分割符，无法识别被非idname字符分割的idname
	template<typename CHAR = char>
	static std::basic_string<CHAR> getToken(std::basic_istream<CHAR>& is, bool forceSpace);


	static std::pair<bool, int> parseInt(const char* s);
	static std::pair<bool, int> parseInt(const std::string& s) { return parseInt(s.c_str()); }

	static std::pair<bool, std::int64_t> parseInt64(const char* s);
	static std::pair<bool, std::int64_t> parseInt64(const std::string& s) { return parseInt64(s.c_str()); }

	static std::pair<bool, double> parseFloat(const char* s);
	static std::pair<bool, double> parseFloat(const std::string& s) { return parseFloat(s.c_str()); }


	/* Convert string tag to writeable format */
	/// @quote: 拟在字符串tag头尾追加的引号. quote = '\\' 或 quote <= 0时，不追加引号
	/// @noNumEscapes: if true, prevent writing in \012 format
	static std::string rewriteString(const char *tag, char quote = '\\', bool noNumEscapes = false);

	/* rewriteString的逆操作 */
	static void parseString(char* tag);


private:
	KuLex() = delete;
};


template<typename CHAR>
std::basic_string<CHAR> KuLex::getToken(std::basic_istream<CHAR>& is, bool forceSpace)
{
	std::basic_string<CHAR> tk;

	skipWhitespace(is);

	auto c = is.peek();

	if (c == '"' || c == '\'') {
		is.get(); // skip " or '
		return getUntil(is, c, false);
	}

	if (forceSpace)
		return getUntil(is, [](CHAR ch) { return isSpace(ch); }, false);

	return getUntil(is, [](CHAR ch) { 
		return !isIdNum(static_cast<CHAR>(c)) && c != '.' && c != '+' && c != '-'; }, false);
}


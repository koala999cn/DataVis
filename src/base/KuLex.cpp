#include "KuLex.h"
#include <assert.h>
#include <regex>
#include "KuStrUtil.h"


bool KuLex::isNumeric(const char *tag)
{
	return std::regex_match(tag, std::regex("^[-+]?([0-9]+(\\.?[0-9]*)?(e[-+]?[0-9]+)?|\\.[0-9]+(e[-+]?[0-9]+)?)$")) ||
		std::regex_match(tag, std::regex("^nan|[-+]?inf$"));
}


bool KuLex::isInteger(const char* tag)
{
	return std::regex_match(tag, std::regex("^[-+]?[0-9]+$"));
}


std::pair<bool, int> KuLex::parseInt(const char* s)
{
	if (!isInteger(s))
		return { false, 0 };

	return{ true, KuStrUtil::toInt(s) };
}


std::pair<bool, std::int64_t> KuLex::parseInt64(const char* s)
{
	if (!isInteger(s))
		return{ false, 0 };

	return{ true, KuStrUtil::toValue<std::int64_t>(s) };
}


std::pair<bool, double> KuLex::parseFloat(const char* s)
{
	if (!isNumeric(s))
		return{ false, 0 };

	return{ true, KuStrUtil::toDouble(s) };
}


int KuLex::hexValue(char c)
{
	constexpr char _hex_bad = -1;
	const static char _hex_value[256] =
	{
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* NUL SOH STX ETX */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* EOT ENQ ACK BEL */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* BS  HT  LF  VT  */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* FF  CR  SO  SI  */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* DLE DC1 DC2 DC3 */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* DC4 NAK SYN ETB */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* CAN EM  SUB ESC */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* FS  GS  RS  US  */

	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* SP  !   "   #   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* $   %   &   '   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* (   )   *   +   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* ,   -   .   /   */
	  0,        1,        2,        3,          /* 0   1   2   3   */
	  4,        5,        6,        7,          /* 4   5   6   7   */
	  8,        9,        _hex_bad, _hex_bad,   /* 8   9   :   ;   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* <   =   >   ?   */

	  _hex_bad, 10,       11,       12,         /* @   A   B   C   */
	  13,       14,       15,       _hex_bad,   /* D   E   F   G   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* H   I   J   K   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* L   M   N   O   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* P   Q   R   S   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* T   U   V   W   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* X   Y   Z   [   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* \   ]   ^   _   */

	  _hex_bad, 10,       11,       12,         /* `   a   b   str   */
	  13,       14,       15,       _hex_bad,   /* d   e   f   g   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* h   i   j   k   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* l   m   n   o   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* p   q   r   s   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* t   u   v   w   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* x   y   z   {   */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,   /* |   }   ~   DEL */

	  /* The high half of unsigned char, all values are _hex_bad.  */
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,

	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,

	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,

	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	  _hex_bad, _hex_bad, _hex_bad, _hex_bad,
	};

	assert(isXDigit(c));

	return _hex_value[static_cast<unsigned char>(c)];
}


std::string KuLex::rewriteString(const char *s, char q, bool noNumEscapes)
{
	std::string dst;
	dst.reserve(1024);

	bool noSing = s[0] != '\'';
	bool noDbl = s[0] != '"';

	if (q != '\\' && q != '\'' && q != '"') {
		q = 0;
		if (!noDbl || !noSing) {
			if (noSing && !noDbl) q = '\'';
			else q = '"';
		}
	}
	if (q > 0 && q != '\\') dst.push_back(q);
	for (const char* p = s; *p; p++) {
		if (*p == '\\' || *p == q ||
			(q == '\\' && p == s &&
				(*p == '\'' || *p == '"')))
			dst.push_back('\\'), dst.push_back(*p);
		else if (isPrint(*p) || noNumEscapes) dst.push_back(*p);
		else { // 将不可打印字符改写为3位八进制表示. 如\123
			int n = *p;
			dst.push_back('\\');
			dst.push_back(((n / 64) % 8) + '0'); dst.push_back(((n / 8) % 8) + '0'); dst.push_back((n % 8) + '0');
		}
	}
	if (q > 0 && q != '\\') dst.push_back(q);

	return dst;
}


void KuLex::parseString(char* s)
{
	const char* p = s; // p为读指针，s为写指针

	while (*p != '\\' && *p != '\0') ++p; // 快速处理
	if (*p == '\0')
		return;

	s += (p - s);
	while (true) {
		assert(*p == '\\');
		if (isDigit(p[1]) && isDigit(p[2]) && isDigit(p[3])) {
			*s++ = 64 * (p[1] - '0') + 8 * (p[2] - '0') + (p[3] - '0');
			p += 4; // skip \123
		}
		else { // 将'\'作为转移字符，当且仅忽略它
			*s++ = *++p;
			++p;
		}

		while (*p != '\\' && *p != '\0') *s++ = *p++;
		if (*p == '\0')
			break;
	}

	*s = '\0';
}

#include "KuUtf8.h"


unsigned KuUtf8::trailingBytes(char head)
{
	/*
	* Index into the table below with the first byte of a UTF-8 sequence to
	* get the number of trailing bytes that are supposed to follow it.
	* Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
	* left as-is for anyone who may want to do such conversion, which was
	* allowed in earlier algorithms.
	*/
	static const unsigned char trailingBytesForUtf8[256] =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
	};

	return trailingBytesForUtf8[unsigned char(head)];
}


unsigned KuUtf8::length(const char* str)
{
	unsigned len(0);
	while (*str != 0) {
		++len;
		str += 1 + trailingBytes(*str);
	}

	return len;
}


const char* KuUtf8::nextChar(const char* str, char ch[5])
{
	*ch = *str++;
	size_t l = trailingBytes(*ch++);
	while (l-- != 0) *ch++ = *str++;
	*ch = '\0';
	return str;
}


void KuUtf8::forEach(const std::string_view& utf8s, std::function<void(const char*)> fn)
{
	auto p = utf8s.data();
	auto end = p + utf8s.length();
	while (p < end) {
		char ch[5];
		p = nextChar(p, ch);
		fn(ch);
	}
}


int KuUtf8::codePoint(const char* ch)
{
	int cp(0);
	while (*ch != 0) {
		cp <<= 8;
		cp += *ch;
	}
	return cp;
}


bool KuUtf8::isLegalUtf8(const char *str, int len)
{
	unsigned char a;
	const char *srcptr = str + len;
	switch (len)
	{
	default: return false;
		/* Everything else falls through when "true"... */
	case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 2: if ((a = (*--srcptr)) > 0xBF) return false;

		switch (*str)
		{
			/* no fall-through in this inner switch */
		case 0xE0: if (a < 0xA0) return false; break;
		case 0xED: if (a > 0x9F) return false; break;
		case 0xF0: if (a < 0x90) return false; break;
		case 0xF4: if (a > 0x8F) return false; break;
		default:   if (a < 0x80) return false;
		}

	case 1: if (*str >= 0x80 && *str < 0xC2) return false;
	}

	if (*str > 0xF4) return false;

	return true;
}


bool KuUtf8::isLegalUtf8Sequence(const char *str, const char *end)
{
	auto len = length(str);
	if (str + len > end)
		return false;

	return isLegalUtf8(str, len);
}
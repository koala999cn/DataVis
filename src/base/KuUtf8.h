#pragma once
#include <string_view>
#include <functional>


class KuUtf8
{
public:

	// 根据首字节，判断后续utf8编码字节数（不含首字节）
	static unsigned trailingBytes(char head);

	// 计算str所编码的utf8字符数
	static unsigned length(const char* str); 

	// 提取str中的一个utf-8编码字符到ch，返回下一个编码字符的地址
	static const char* nextChar(const char* str, char ch[5]);

	static void forEach(const std::string_view& utf8s, std::function<void(const char*)> fn);

	static int codePoint(const char* ch);

	/*
	* Utility routine to tell whether a sequence of bytes is legal UTF-8.
	* This must be called with the length pre-determined by the first byte.
	* If not calling this from ConvertUTF8to*, then the length can be set by:
	*  length = trailingBytesForUTF8[*source]+1;
	* and the sequence is illegal right away if there aren't that many bytes
	* available.
	* If presented with a length > 4, this returns false.  The Unicode
	* definition of UTF-8 goes up to 4-byte sequences.
	*/
	static bool isLegalUtf8(const char *str, int len);

	/*
	* Exported function to return whether a UTF-8 sequence is legal or not.
	* This is not used here; it'tag just exported.
	*/
	static bool isLegalUtf8Sequence(const char *str, const char *end);


private:
	KuUtf8() { }
	~KuUtf8() { }
};


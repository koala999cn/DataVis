#include "KuStrUtil.h"
#include <sstream>
#include <assert.h>
#include <regex>


void KuStrUtil::toUpper(char* str)
{
	while (*str != 0) {
		*str = std::toupper(*str);
		++str;
	}
}


void KuStrUtil::toUpper(std::string& str) 
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) -> unsigned char { return std::toupper(c); });
}


void KuStrUtil::toLower(char* str) 
{
	while (*str != 0) {
		*str = std::tolower(*str);
		++str;
	}
}


void KuStrUtil::toLower(std::string& str) 
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) -> unsigned char { return std::tolower(c); });
}


bool KuStrUtil::isFloat(char* str)
{
	return std::regex_match(str, std::regex("^[-+]?[0-9]*\\.?[0-9]+$"));
}


bool KuStrUtil::beginWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase)
{
	if (nstr < nwith)
		return false;

	return nocase ? _strnicmp(str, with, nwith) == 0 : strncmp(str, with, nwith) == 0;
}


bool KuStrUtil::beginWith(const char* str, const char* with, bool nocase)
{
	return beginWith(str, length(str), with, length(with), nocase);
}


bool KuStrUtil::beginWith(const std::string& str, const std::string& with, bool nocase)
{
	return beginWith(str.c_str(), str.length(), with.c_str(), with.length(), nocase);
}


bool KuStrUtil::endWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase)
{
	if (nstr < nwith)
		return false;

	str += nstr - nwith;
	return nocase ? _strnicmp(str, with, nwith) == 0 : strncmp(str, with, nwith) == 0;
}


bool KuStrUtil::endWith(const char* str, const char* with, bool nocase)
{
	return endWith(str, length(str), with, length(with), nocase);
}


bool KuStrUtil::endWith(const std::string& str, const std::string& with, bool nocase)
{
	return endWith(str.c_str(), str.length(), with.c_str(), with.length(), nocase);
}


const char* KuStrUtil::findFirstOf(const char* str, char ch)
{
	assert(str != nullptr);
	return std::strchr(str, ch);
}


const char* KuStrUtil::findFirstNotOf(const char* str, char ch)
{
	assert(str != nullptr);

	if (*str == '\0' && ch == '\0')
		return nullptr;

	while (*str == ch) ++str;

	return *str == '\0' ? nullptr : str;
}


const char* KuStrUtil::findFirstOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	auto pos = std::strcspn(str, which);
	return str[pos] == '\0' ? nullptr : str + pos;
}


const char* KuStrUtil::findFirstNotOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	for (; *str != '\0'; str++)
		if (std::strchr(which, *str) == nullptr)
			return str;

	return nullptr;
}


const char* KuStrUtil::findLastOf(const char* str, char ch)
{
	assert(str != nullptr);
	return std::strrchr(str, ch);
}


const char* KuStrUtil::findLastNotOf(const char* str, char ch)
{
	assert(str != nullptr);

	if (*str == '\0' && ch == '\0')
		return nullptr;

	for (auto buf = str + length(str) - 1; buf >= str; buf--)
		if (*buf != ch)
			return buf;

	return nullptr;
}


const char* KuStrUtil::findLastOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	for(auto buf = str + length(str) - 1; buf >= str; buf--) 
		if (std::strchr(which, *buf) != nullptr)
			return buf;

	return nullptr;
}


const char* KuStrUtil::findLastNotOneOf(const char* str, const char* which)
{
	assert(str != nullptr);
	for (auto buf = str + length(str) - 1; buf >= str; buf--)
		if (std::strchr(which, *buf) == nullptr)
			return buf;

	return nullptr;
}


bool KuStrUtil::replaceSubstr(char* str, const char* substr, const char* with)
{
    assert(length(substr) == length(with));
	bool replaced = false;
	while (str = std::strstr(str, substr)) {
		replaced = true;
		const char* ch = with;
		while (*ch != '\0')
			*str++ = *ch++;
	}

	return replaced;
}


bool KuStrUtil::replaceSubstr(std::string& str, const std::string& substr, const std::string& with)
{
	std::string::size_type pos = 0;
	bool replaced = false;
	while ((pos = str.find(substr, pos)) != std::string::npos) {
		replaced = true;
		str.replace(pos, substr.length(), with);
		pos += with.length();
	}

	return replaced;
}


void KuStrUtil::replaceChar(char* str, char ch, char with)
{
	while (str = const_cast<char*>(findFirstOf(str, ch)))
			*str++ = with;
}


void KuStrUtil::replaceChar(std::string& str, char ch, char with)
{
	std::replace(str.begin(), str.end(), ch, with);
}


void KuStrUtil::eraseFirstOf(char* str, char ch)
{
	auto p = const_cast<char*>(findFirstOf(str, ch));
	if (p) while (*p != '\0') *p = *++p;
}


void KuStrUtil::eraseAll(char* str, char ch)
{
	auto p = str;
	while (*str != '\0') {
		while (*str++ == ch); // skip ch
		*p++ = *str++; // copy non-ch
	}
}


void KuStrUtil::eraseAll(std::string& str, const std::string& substr)
{
	replaceSubstr(str, substr, "");
}


const char* KuStrUtil::trimLeftInPlace(const char* str, const char* spaces)
{
	auto pos = findFirstNotOneOf(str, spaces);
	return pos ? pos : str + length(str);
}


void KuStrUtil::trimRightInPlace(char* str, const char* spaces)
{
	auto pos = const_cast<char*>(findLastNotOneOf(str, spaces));
	if (pos != nullptr)
		*++pos = '\0';
}


char* KuStrUtil::trimInPlace(char* str, const char* spaces)
{
	trimRightInPlace(str, spaces);
	return const_cast<char*>(trimLeftInPlace(str, spaces));
}


void KuStrUtil::trim(std::string& str, const char* spaces)
{
	auto pos = str.find_last_not_of(spaces);
	if (pos != std::string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(spaces);
		if (pos != std::string::npos) str.erase(0, pos);
	}
	else {
		str.erase(str.begin(), str.end());
	}
}


std::vector<std::string> KuStrUtil::split(const std::string& full, const std::string& delims, bool skip_empty_strings)
{
    std::vector<std::string> tokens;
	size_t start = 0, found = 0, end = full.size();
	while (found != std::string::npos) {
		found = full.find_first_of(delims, start);
		// start != end condition is for when the delimiter is at the end
        if (!skip_empty_strings || (found != start && start != end))
			tokens.push_back(full.substr(start, found - start));
		start = found + 1;
	}

    return tokens;
}


std::string KuStrUtil::join(const std::vector<std::string> &input, char c)
{
	std::string out;
	if (!input.empty()) {
		out = input[0];
		for (size_t i = 1; i < input.size(); i++)
			out += c + input[i];
	}

	return out;
}

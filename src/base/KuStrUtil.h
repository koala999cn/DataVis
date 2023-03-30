#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <sstream>
#include <optional>
#include <charconv>
#include <iomanip> // Header file needed to use setprecision


class KuStrUtil
{
public:

	// 字符串长度
    template<typename CHAR>
    static auto length(CHAR* str) {
        return std::char_traits<CHAR>::length(str);
    }


	// 大写转换
	static void toUpper(char* str);
	static void toUpper(std::string& str);

	// 小写转换
	static void toLower(char* str);
	static void toLower(std::string& str);

	// str是否以with开头
	// @nocase: true表示进行大小写无关的比较
	static bool beginWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase = false);
	static bool beginWith(const char* str, const char* with, bool nocase = false);
	static bool beginWith(const std::string& str, const std::string& with, bool nocase = false);

	// str是否以with结尾
	static bool endWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase = false);
	static bool endWith(const char* str, const char* with, bool nocase = false);
	static bool endWith(const std::string& str, const std::string& with, bool nocase = false);

	// 统计字符穿中有多少个ch字符
	static int count(const std::string_view& str, char ch);

	// 查找str中的第一个ch的指针，未找到返回nullptr
	static const char* findFirstOf(const char* str, char ch);

	// 查找str中的第一个非ch的指针，未找到返回nullptr
	static const char* findFirstNotOf(const char* str, char ch);

	// 查找str中第一个匹配which中任意字符的指针位置，未找到返回nullptr
	static const char* findFirstOneOf(const char* str, const char* which);

	// 查找str中第一个不匹配which中任意字符的指针位置，未找到返回nullptr
	static const char* findFirstNotOneOf(const char* str, const char* which);

	// 查找str中的最后一个ch的指针，未找到返回nullptr
	static const char* findLastOf(const char* str, char ch);

	// 查找str中的最后一个非ch的指针，未找到返回nullptr
	static const char* findLastNotOf(const char* str, char ch);

	// 查找str中最后一个匹配which中任意字符的指针位置，未找到返回nullptr
	static const char* findLastOneOf(const char* str, const char* which);

	// 查找str中最后一个不匹配which中任意字符的指针位置，未找到返回nullptr
	static const char* findLastNotOneOf(const char* str, const char* which);


	// 等长子字符串就地替换. 返回true表示至少执行过一次替换
	// assert(length(substr) == length(with))
	static bool replaceSubstr(char* str, const char* substr, const char* with); 

	// 新旧字串可以不等长. 返回true表示至少执行过一次替换
	static bool replaceSubstr(std::string& str, const std::string& substr, const std::string& with);

	// Replace a given character with another character in a string
	// @str: String to use have characters replaced
	// @ch: Character to search string for
	// @with: Character to replace found ch with
	static void replaceChar(char* str, char ch, char with);
	static void replaceChar(std::string& str, char ch, char with);


	// 删除字符串str中的第一个ch字符
	static void eraseFirstOf(char* str, char ch);

	// 删除字符串str中的所有ch字符
	static void eraseAll(char* str, char ch);

	// 删除字符串str中的所有substr
	static void eraseAll(std::string& str, const std::string& substr);


	constexpr static const char* k_spaceChars = " \t\n\r\f\v";

	// 裁剪str前缀的空白字符，即返回第一个非空白字符的指针
	static const char* trimLeftInPlace(const char* str, const char* spaces = k_spaceChars); 

	// 裁剪str后缀的空白字符，即将str后缀的第一个空白字符置为'\0'
	static void trimRightInPlace(char* str, const char* spaces = k_spaceChars); 

	// 裁剪str前后的空白字符，综合trimLeftInPlace与trimRightInPlace操作
	static char* trimInPlace(char* str, const char* spaces = k_spaceChars);

	// std::string版本的空白字符裁剪
	static void trim(std::string& str, const char* spaces = k_spaceChars);

	static std::string_view trim(const std::string_view& sv, const char* spaces = k_spaceChars);

    static std::vector<std::string_view> split(const std::string_view& full, const std::string& delims, bool skipEmpty = true);

	static std::vector<std::string_view> splitWithQuote(const std::string_view& full, const std::string& delims, bool skipEmpty = true);

	static std::vector<std::string> splitRegex(const std::string& full, const std::string& regex, bool skipEmpty = true);

	static std::string join(const std::vector<std::string>& input, char c);

	
	// Converts a vector of T into a string delimited by a character c
	// @input: Vector of T to be converted
	// @c: Character to use as a delimiter
	template<typename T, typename CHAR = char>
	static std::basic_string<CHAR> join(const std::vector<T>& input, CHAR c) {
		std::basic_string<CHAR> out;
		if (!input.empty()) {
			out = toString<T, CHAR>(input[0]);
			for (size_t i = 1; i < input.size(); i++)
				out += c + toString<T, CHAR>(input[i]);
		}

		return out;
	}


	// 数值转换为字符串
	template<typename T, typename CHAR = char>
	static std::basic_string<CHAR> toString(T val, int prec = std::numeric_limits<T>::digits10) {
		std::basic_ostringstream<CHAR> stream;
		stream << std::setprecision(prec) << val;
		return stream.str();
	}


	// 转换为二进制字符串
	template<typename T, typename CHAR = char>
	static std::basic_string<CHAR> toBinaryString(T val) {
		std::basic_string<CHAR> output("");
		for (int i = sizeof(val) * 8 - 1; i >= 0; --i) {
			if (val & (1 << i)) 
				output += CHAR('1');
			else 
				output += CHAR('0');
		}
		return output;
	}

	// @STRICT: 若true，则只有当str可全部解析为T时才返回有效数据
	template<typename T, bool STRICT = true>
	static std::optional<T> toValue(const std::string_view& str) {
		auto last = str.data() + str.size();
		if constexpr (STRICT) {
			if (T value; std::from_chars(str.data(), last, value).ptr == last)
				return value;
		}
		else {
			if (T value; std::from_chars(str.data(), last, value).ec == std::errc{})
				return value;
		}
		
		return std::nullopt;
	}

	template<bool STRICT = true>
	static auto toInt(const std::string_view& str) { return toValue<int, STRICT>(str); }

	template<bool STRICT = true>
	static auto toFloat(const std::string_view& str) { return toValue<float, STRICT>(str); }

	template<bool STRICT = true>
	static auto toDouble(const std::string_view& str) { return toValue<double, STRICT>(str); }


	// 对矢量和矩阵的格式化，可用于调试输出
	template<typename T>
	static std::string dump(const T* v, size_t dim, char delim = '\t', int max_elements_to_show = -1) {
		if (max_elements_to_show <= 0)
			max_elements_to_show = dim;
		else if (max_elements_to_show > static_cast<int>(dim))
			max_elements_to_show = dim;

		std::string str;
		for (int i = 0; i < max_elements_to_show; i++) {
			str += toString(v[i]);
			str += delim;
		}

		if (max_elements_to_show != dim)
			str += "...";
		else
			str.pop_back(); // remove ending delim

		return str;
	}

	template<typename T>
	static std::string dump(const std::vector<T>& v, char delim = '\t', int max_elements_to_show = -1) {
		return dump(v.data(), v.size(), delim, max_elements_to_show);
	}

	template<typename T>
	static std::string dump(const T* m, int rows, int cols, char delim = '\t', int max_row_to_show = -1, int max_col_to_show = -1, const char* prefix = "") {
		if (max_row_to_show <= 0 || max_row_to_show > rows)
			max_row_to_show = rows;

		std::string str;
		for (int i = 0; i < max_row_to_show; i++) {
			str += prefix;
			str += dump(m[i * cols], cols, delim, max_col_to_show);
			str += '\n';
		}

		if (max_row_to_show != rows) {
			str += prefix;
			str += "...";
		}
		else
			str.pop_back(); // remove ending '\n'

		return str;
	}

	template<typename T>
	static std::string dump(const std::vector<std::vector<T>>& m, char delim = '\t', int max_row_to_show = -1, int max_col_to_show = -1, const char* prefix = "") {
		if (max_row_to_show <= 0)
			max_row_to_show = m.size();
		else if (max_row_to_show > static_cast<int>(m.size()))
			max_row_to_show = m.size();

		std::string str;
		for (int i = 0; i < max_row_to_show; i++) {
			str += prefix;
			str += dump(m[i], delim, max_col_to_show);
			str += '\n';
		}

		if (max_row_to_show != m.size()) {
			str += prefix;
			str += "...";
		}
		else
			str.pop_back(); // remove ending '\n'

		return str;
	}


private:
	KuStrUtil() = default;
	~KuStrUtil() = default;
};


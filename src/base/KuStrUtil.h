#pragma once
#include <string>
#include <vector>
#include <algorithm>


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


	// 是否为浮点数
	static bool isFloat(char* str);
    static bool isFloat(const std::string& str) { return isFloat(str.c_str()); }


	// str是否以with开头
	// @nocase: true表示进行大小写无关的比较
	static bool beginWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase = false);
	static bool beginWith(const char* str, const char* with, bool nocase = false);
	static bool beginWith(const std::string& str, const std::string& with, bool nocase = false);

	// str是否以with结尾
	static bool endWith(const char* str, unsigned nstr, const char* with, unsigned nwith, bool nocase = false);
	static bool endWith(const char* str, const char* with, bool nocase = false);
	static bool endWith(const std::string& str, const std::string& with, bool nocase = false);


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


    static std::vector<std::string> split(const std::string& full, const std::string& delims, bool skip_empty_token = true);

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
	static std::basic_string<CHAR> toString(T val) {
		std::basic_ostringstream<CHAR> stream;
		stream << val;
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


	// 字符串转化为数值
	template<typename T, typename CHAR = char>
	static T toValue(const CHAR* str) {
		std::basic_istringstream<CHAR> stream(str);
		T val;
		stream >> val;

		return val;
	}

	template<typename CHAR = char>
	int toInt(const CHAR* str) { return toValue<int, CHAR>(str); }

	template<typename CHAR = char>
	float toFloat(const CHAR* str) { return toValue<float, CHAR>(str); }

	template<typename CHAR = char>
	double toDouble(const CHAR* str) { return toValue<double, CHAR>(str); }


	// 对矢量和矩阵的格式化，可用于调试输出
	template<typename T>
	static std::string formatVector(const T* v, size_t dim, int max_elements_to_show = 10) {
		if (max_elements_to_show <= 0)
			max_elements_to_show = dim;
		else if (max_elements_to_show > static_cast<int>(dim))
			max_elements_to_show = dim;

		std::string str;
		for (int i = 0; i < max_elements_to_show; i++) {
			str += toString(v[i]);
			str += '\t';
		}

		if (max_elements_to_show != dim)
			str += "...";
		else
			str.pop_back(); // remove ending '\t'

		return str;
	}

	template<typename T>
	static std::string formatVector(const std::vector<T>& v, int max_elements_to_show = 10) {
		return formatVector(v.data(), v.size(), max_elements_to_show);
	}

	template<typename T>
	static std::string formatMatrix(const T** m, int rows, int cols, int max_row_to_show = 5, int max_col_to_show = 10, const char* prefix = "") {
		if (max_row_to_show <= 0 || max_row_to_show > rows)
			max_row_to_show = rows;

		std::string str;
		for (int i = 0; i < max_row_to_show; i++) {
			str += prefix;
			str += formatVector(m[i], cols, max_col_to_show);
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
	static std::string formatMatrix(const std::vector<std::vector<T>>& m, int max_row_to_show = 5, int max_col_to_show = 10, const char* prefix = "") {
		if (max_row_to_show <= 0)
			max_row_to_show = m.size();
		else if (max_row_to_show > static_cast<int>(m.size()))
			max_row_to_show = m.size();

		std::string str;
		for (int i = 0; i < max_row_to_show; i++) {
			str += prefix;
			str += formatVector(m[i], max_col_to_show);
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
	
	template<typename T>
	static std::string formatVectorAsMatrix(const std::vector<T>& v, size_t cols, int max_row_to_show = 5, int max_col_to_show = 10, const char* prefix = "") {
		int rows = v.size() / cols;

		if (max_row_to_show <= 0)
			max_row_to_show = rows;
		else if (max_row_to_show > rows)
			max_row_to_show = rows;

		std::string str;
		const double* buf = v.data();
		for (int i = 0; i < max_row_to_show; i++, buf += cols) {
			str += prefix;
			str += FormatVector(buf, cols, max_col_to_show);
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


private:
	KuStrUtil() {}
	~KuStrUtil() {}
};


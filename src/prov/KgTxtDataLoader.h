#pragma once
#include <vector>
#include <string>

// 加载txt数据文件的支持类，提供丰富选项，可兼容多种格式

class KvData;

class KgTxtDataLoader
{
public:

	enum KeIllegalMode // 遇到非数字token时的处理方式
	{
		k_illegal_fail,
		k_illegal_ignore,
		k_illegal_as_empty,
		k_illegal_as_nan,
		k_illegal_as_zero
	};

	enum KeEmptyMode // 遇到空token时的处理方式
	{
		k_empty_skip,
		k_empty_as_nan,
		k_empty_as_zero
	};

	KgTxtDataLoader();

	// @mat: 从path加载的二维数组
	bool load(const char* path, std::vector<std::vector<double>>& mat) const;

	// 返回空字符串的数值
	double nullValue() const;

	KeIllegalMode illegalMode() const {
		return illegalMode_;
	}

	void setIllegalMode(KeIllegalMode mode) {
		illegalMode_ = mode;
	}

	KeEmptyMode emptyMode() const {
		return emptyMode_;
	}

	void setEmptyMode(KeEmptyMode mode) {
		emptyMode_ = mode;
	}

	const std::string& delim() const { return rexpDelim_; }
	void setDelim(std::string delim) { rexpDelim_ = delim; }

	const std::string& NA() const { return rexpNA_; }
	void setNA(std::string na) { rexpNA_ = na; }

	const std::string& comment() const { return rexpComment_; }
	void setComment(std::string com) { rexpComment_ = com; }

private:
	//bool skipEmptyRow_, skipEmptyCol_;
	KeIllegalMode illegalMode_;
	KeEmptyMode emptyMode_;
	//bool skipHeadingIndex_; // 忽略首行或首列的1, 2, 3, ...顺序序列数据. 暂未实现
	// bool rowMajor_; // 默认列主序，rowMajor_为真将对加载的二维数据进行转置
	std::string rexpNA_; // NA的正则表达式
	std::string rexpDelim_; // 分隔符的正则表达式
	std::string rexpComment_; // 注释的正则表达式，必须位于行开头

	// TODO:
	mutable std::vector<std::string> colName_, rowName; // 文件解析中间结果，保存可能存在的行/列名称
};


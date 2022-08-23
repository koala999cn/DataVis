#pragma once
#include <vector>
#include <string>

// ����txt�����ļ���֧���࣬�ṩ�ḻѡ��ɼ��ݶ��ָ�ʽ

class KvData;

class KgTxtDataLoader
{
public:

	enum KeIllegalMode // ����������tokenʱ�Ĵ���ʽ
	{
		k_illegal_fail,
		k_illegal_ignore,
		k_illegal_as_empty,
		k_illegal_as_nan,
		k_illegal_as_zero
	};

	enum KeEmptyMode // ������tokenʱ�Ĵ���ʽ
	{
		k_empty_skip,
		k_empty_as_nan,
		k_empty_as_zero
	};

	KgTxtDataLoader();

	// @mat: ��path���صĶ�ά����
	bool load(const char* path, std::vector<std::vector<double>>& mat) const;

	// ���ؿ��ַ�������ֵ
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
	//bool skipHeadingIndex_; // �������л����е�1, 2, 3, ...˳����������. ��δʵ��
	// bool rowMajor_; // Ĭ��������rowMajor_Ϊ�潫�Լ��صĶ�ά���ݽ���ת��
	std::string rexpNA_; // NA��������ʽ
	std::string rexpDelim_; // �ָ�����������ʽ
	std::string rexpComment_; // ע�͵�������ʽ������λ���п�ͷ

	// TODO:
	mutable std::vector<std::string> colName_, rowName; // �ļ������м�����������ܴ��ڵ���/������
};


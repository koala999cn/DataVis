#pragma once
#include <vector>
#include <string>

// ����txt�����ļ���֧���࣬�ṩ�ḻѡ��ɼ��ݶ��ָ�ʽ

class KvData;

class KgTxtDataLoader
{
public:
	using vectord = std::vector<double>;
	using matrixd = std::vector<vectord>;

	enum KeIllegalMode // ����������tokenʱ�Ĵ�����ʽ
	{
		k_illegal_fail,
		k_illegal_ignore,
		k_illegal_as_empty,
		k_illegal_as_nan,
		k_illegal_as_zero
	};

	enum KeEmptyMode // ������tokenʱ�Ĵ�����ʽ
	{
		k_empty_skip,
		k_empty_as_nan,
		k_empty_as_zero
	};

	KgTxtDataLoader();

	// @mat: ��path���صĶ�ά����
	bool load(const char* path, matrixd& mat) const;

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

	// ����mat����С�������������
	static std::pair<unsigned, unsigned> colsRange(const matrixd& mat);

	static matrixd transpose(const matrixd& mat);

	// ǿ�ƶ���mat���е�����
	// @cols: ���ж��뵽��������-1��ʾ���뵽mat�����������0��ʾ���뵽mat����С����
	// @missingVal: ȱʧ���ݵ��趨ֵ
	static void forceAligned(matrixd& mat, unsigned cols = -1, double missingVal = std::numeric_limits<double>::quiet_NaN());

	static vectord column(const matrixd& mat, unsigned idx);

	static bool isEvenlySpaced(const vectord& v);

private:
	//bool skipEmptyRow_, skipEmptyCol_;
	KeIllegalMode illegalMode_;
	KeEmptyMode emptyMode_;
	//bool skipHeadingIndex_; // �������л����е�1, 2, 3, ...˳����������. ��δʵ��
	// bool rowMajor_; // Ĭ��������rowMajor_Ϊ�潫�Լ��صĶ�ά���ݽ���ת��
	std::string rexpNA_; // NA���������ʽ
	std::string rexpDelim_; // �ָ������������ʽ
	std::string rexpComment_; // ע�͵��������ʽ������λ���п�ͷ

	// TODO:
	mutable std::vector<std::string> colName_, rowName; // �ļ������м�����������ܴ��ڵ���/������
};

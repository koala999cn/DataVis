#pragma once
#include <vector>
#include <memory>
#include <string>
#include "KtuMath.h"

class KvData;

class KuMatrixUtil
{
public:
	template<typename T>
	using matrix = std::vector<std::vector<T>>;

	using vectord = std::vector<double>;
	using matrixd = matrix<double>;

    using vectorstr = std::vector<std::string>;
    using matrixstr = matrix<std::string>;

	// 返回mat的最小列数和最大列数
	template<typename T>
	static std::pair<unsigned, unsigned> colsRange(const matrix<T>& mat);

    template<typename T>
	static matrix<T> transpose(const matrix<T>& mat);

    // 提取矩阵的特定列
    template<typename T>
	static std::vector<T> extractColumn(const matrix<T>& mat, unsigned idx);

    // 强制对齐mat各行的列数
    // @cols: 各行对齐到的列数，-1表示对齐到mat的最大列数，0表示对齐到mat的最小列数
    // @blankFill: 缺位元素的填充值
    template<typename T>
    static void forceAligned(matrix<T>& mat, const T& blankFill, unsigned cols = -1);

    // 判断序列数据v是否等间隔
    template<typename T>
	static bool isEvenlySpaced(const std::vector<T>& v);

private:
	KuMatrixUtil();
};


template<typename T>
std::pair<unsigned, unsigned> KuMatrixUtil::colsRange(const matrix<T>& mat)
{
    unsigned minCols(0), maxCols(0);
    if (!mat.empty()) {
        minCols = maxCols = mat[0].size();
        for (unsigned i = 1; i < mat.size(); i++) {
            if (mat[i].size() > maxCols)
                maxCols = mat[i].size();

            if (mat[i].size() < minCols)
                minCols = mat[i].size();
        }
    }

    return { minCols, maxCols };
}


template<typename T>
KuMatrixUtil::matrix<T> KuMatrixUtil::transpose(const matrix<T>& mat)
{
    matrix<T> trans;
    if (!mat.empty()) {
        trans.resize(mat.front().size(), std::vector<T>(mat.size()));
        for (unsigned r = 0; r < mat.size(); r++)
            for (unsigned c = 0; c < trans.size(); c++)
                trans[c][r] = mat[r][c];
    }

    return trans;
}


template<typename T>
std::vector<T> KuMatrixUtil::extractColumn(const matrix<T>& mat, unsigned idx)
{
    std::vector<T> v(mat.size());
    for (unsigned i = 0; i < mat.size(); i++)
        v[i] = mat[i][idx];

    return v;
}


template<typename T>
void KuMatrixUtil::forceAligned(matrix<T>& mat, const T& blankFill, unsigned cols)
{
    if (cols == -1)
        cols = colsRange(mat).second;
    else if (cols == 0)
        cols = colsRange(mat).first;

    for (auto& row : mat)
        row.resize(cols, blankFill);
}


template<typename T>
bool KuMatrixUtil::isEvenlySpaced(const std::vector<T>& v)
{
    if (v.size() < 2)
        return true;

    auto dx = v[1] - v[0];

    for (unsigned i = 1; i < v.size(); i++) {
        auto delta = v[i] - v[i - 1];
        if (!KtuMath<T>::almostEqual(dx, delta))
            return false;

        dx = delta;
    }

    return true;
}

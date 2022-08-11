#include "KgTxtDataLoader.h"
#include <fstream>
#include <string>
#include <regex>
#include <assert.h>
#include "base/KuStrUtil.h"


KgTxtDataLoader::KgTxtDataLoader()
{
    illegalMode_ = k_illegal_ignore;
    emptyMode_ = k_empty_skip;
    //rowMajor_ = false;
    rexpNA_ = "na|nan|n/a|-";
    rexpDelim_ = "\\s+";
    rexpComment_ = "^\\s*#";
}


bool KgTxtDataLoader::load(const char* path, matrixd& mat) const
{
    mat.clear();

    std::ifstream ifs(path);
    std::string line;
    while (std::getline(ifs, line)) {
       
        // ÏÈ¼ì²âcomment
        if (!rexpComment_.empty()) {
            const std::regex re(rexpComment_);
            std::smatch sm;
            if (std::regex_search(line, re))
                continue; // Ìø¹ý×¢ÊÍÐÐ
        }

        auto tokens = KuStrUtil::splitRegex(line, rexpDelim_, emptyMode_ == k_empty_skip);
        if (tokens.empty())
            continue; // always skip empty line

        std::vector<double> row;
        row.reserve(tokens.size());
        for (auto& tok : tokens) {

            // ¿Õ×Ö·û´®¼ì²â
            if (tok.empty()) {
                if (emptyMode_ != k_empty_skip)
                    row.push_back(nullValue());
                continue;
            }

            // NA×Ö·û´®¼ì²â
            const std::regex re(rexpNA_);
            std::smatch sm;
            if (std::regex_match(line, re)) {
                row.push_back(std::numeric_limits<double>::quiet_NaN());
                continue;
            }

            // ³¢ÊÔ½âÎöÊý×Ö
            double val;
            if (!KuStrUtil::toValue(tok.c_str(), val)) {
                switch (illegalMode_) {
                case k_illegal_fail: return false;
                case k_illegal_ignore: continue;

                case k_illegal_as_nan:
                    val = std::numeric_limits<double>::quiet_NaN();
                    break;
                case k_illegal_as_zero:
                    val = 0;
                    break;
                case k_illegal_as_empty:
                    val = nullValue();
                    break;
                }
            }

            row.push_back(val);
        }

        if (!row.empty())
            mat.push_back(row);
    }

    //if (rowMajor_) {
   //     mat = transpose(mat);
   //     std::swap(colName_, rowName);
   // }

    return true;
}


std::pair<unsigned, unsigned> KgTxtDataLoader::colsRange(const matrixd& mat)
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


KgTxtDataLoader::matrixd KgTxtDataLoader::transpose(const matrixd& mat)
{
    matrixd trans;
    if (!mat.empty()) {
        trans.resize(mat.front().size(), vectord(mat.size()));
        for (unsigned r = 0; r < mat.size(); r++)
            for (unsigned c = 0; c < trans.size(); c++)
                trans[c][r] = mat[r][c];
    }

    return trans;
}


double KgTxtDataLoader::nullValue() const
{
    return emptyMode_ == k_empty_as_zero ? 0 : std::numeric_limits<double>::quiet_NaN();
}


void KgTxtDataLoader::forceAligned(matrixd& mat, unsigned cols, double missingVal)
{
    if (cols == -1)
        cols = colsRange(mat).second;
    else if(cols == 0)
        cols = colsRange(mat).first;

    for (auto& row : mat)
        row.resize(cols, missingVal);
}


KgTxtDataLoader::vectord KgTxtDataLoader::column(const matrixd& mat, unsigned idx)
{
    vectord v(mat.size());
    for (unsigned i = 0; i < mat.size(); i++)
        v[i] = mat[i][idx];

    return v;
}


bool KgTxtDataLoader::isEvenlySpaced(const vectord& v)
{
    if (v.size() < 2)
        return true;

    auto dx = v[1] - v[0];

    for (unsigned i = 1; i < v.size(); i++) {
        auto delta = v[i] - v[i - 1];
        if (std::abs(dx - delta) > 1e-6) // TODO: tol¿ÉÉèÖÃ
            return false;
        dx = delta;
    }

    return true;
}

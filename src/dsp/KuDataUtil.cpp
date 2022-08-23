#include "KuDataUtil.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtScattered.h"


std::pair<unsigned, unsigned> KuDataUtil::colsRange(const matrixd& mat)
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


KuDataUtil::matrixd KuDataUtil::transpose(const matrixd& mat)
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


KuDataUtil::vectord KuDataUtil::column(const matrixd& mat, unsigned idx)
{
    vectord v(mat.size());
    for (unsigned i = 0; i < mat.size(); i++)
        v[i] = mat[i][idx];

    return v;
}


void KuDataUtil::forceAligned(matrixd& mat, unsigned cols, double missingVal)
{
    if (cols == -1)
        cols = colsRange(mat).second;
    else if (cols == 0)
        cols = colsRange(mat).first;

    for (auto& row : mat)
        row.resize(cols, missingVal);
}


bool KuDataUtil::isEvenlySpaced(const vectord& v)
{
    if (v.size() < 2)
        return true;

    auto dx = v[1] - v[0];

    for (unsigned i = 1; i < v.size(); i++) {
        auto delta = v[i] - v[i - 1];
        if (std::abs(dx - delta) > 1e-6) // TODO: tol可设置
            return false;
        dx = delta;
    }

    return true;
}


std::shared_ptr<KvData> KuDataUtil::makeSeries(const matrixd& mat)
{
    // 暂时使用sampled1d表示series数据

    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat[0].size(), mat.size());
    samp->reset(0, 0, 1);

    for (unsigned c = 0; c < samp->channels(); c++)
        samp->setChannel(0, c, mat[c].data());

    return samp;
}



std::shared_ptr<KvData> KuDataUtil::makeMatrix(const matrixd& mat)
{
    auto samp2d = std::make_shared<KcSampled2d>();
    samp2d->resize(mat.size(), mat[0].size());
    samp2d->reset(0, 0, 1);
    samp2d->reset(1, 0, 1);

    for (kIndex idx = 0; samp2d->size(0); idx++)
        samp2d->setChannel(&idx, 0, mat[idx].data()); // 始终单通道

    return samp2d;
}


std::shared_ptr<KvData> KuDataUtil::makeSampled1d(const matrixd& mat)
{
    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat[0].size(), mat.size() - 1);
    samp->reset(0, mat[0][0], mat[0][1] - mat[0][0]);

    for (unsigned c = 0; c < samp->channels(); c++)
        samp->setChannel(0, c, mat[c + 1].data());

    return samp;
}


std::shared_ptr<KvData> KuDataUtil::makeSampled2d(const matrixd& mat)
{
    // mat[0][0]为占位符，无效

    auto dx = mat[2][0] - mat[1][0];
    auto dy = mat[0][2] - mat[0][1];

    auto samp2d = std::make_shared<KcSampled2d>();
    samp2d->resize(mat.size() - 1, mat[0].size() - 1);
    samp2d->reset(0, mat[1][0], dx);
    samp2d->reset(1, mat[0][1], dy);

    for (kIndex idx = 0; samp2d->size(0); idx++)
        samp2d->setChannel(&idx, 0, mat[idx + 1].data() + 1); // 始终单通道, 忽略mat数据的首行首列

    return samp2d;
}


std::shared_ptr<KvData> KuDataUtil::makeScattered(const matrixd& mat, unsigned dim)
{
    assert(dim == 2 || dim == 3);
    assert(mat.size() % dim == 0);

    kIndex chs = mat.size() / dim;
    if (dim == 2) {
        auto scattered = std::make_shared<KtScattered<1>>();
        scattered->resize(nullptr, chs);
        scattered->reserve(mat[0].size());

        for (unsigned i = 0; i < mat[0].size(); i++) {
            std::vector<typename KtScattered<1>::element_type> points;
            for (unsigned c = 0; c < chs; c++)
                points.push_back({ mat[c * 2][i], mat[c * 2 + 1][i] });

            scattered->pushBack(points.data());
        }

        return scattered;
    }
    else {
        auto scattered = std::make_shared<KtScattered<2>>();
        scattered->resize(nullptr, chs);
        scattered->reserve(mat[0].size());

        for (unsigned i = 0; i < mat[0].size(); i++) {
            std::vector<typename KtScattered<2>::element_type> points;
            for (unsigned c = 0; c < chs; c++)
                points.push_back({ mat[c * 3][i], mat[c * 3 + 1][i], mat[c * 3 + 2][i] });

            scattered->pushBack(points.data());
        }

        return scattered;
    }
}


std::vector<KuDataUtil::KeDataType> KuDataUtil::validTypes(const matrixd& mat, bool colMajor)
{
    if (mat.empty())
        return {};

    auto rows = mat.size();
    auto cols = mat[0].size();

    if (rows == 0 && cols == 0)
        return {};

    if (!colMajor)
        std::swap(rows, cols);

    std::vector<KeDataType> types;

    types.push_back(k_series);

    if (rows > 8 && cols > 8)
        types.insert(types.begin(), k_matrix);
    else
        types.push_back(k_matrix);

    if (cols > 1) {

        if (cols % 2 == 0) 
            types.insert(types.begin(), k_scattered_1d);

        if (cols % 3 == 0) 
            types.insert(types.begin(), k_scattered_2d);
   
        bool evenRow = KuDataUtil::isEvenlySpaced(mat[0]);
        bool evenCol = KuDataUtil::isEvenlySpaced(KuDataUtil::column(mat, 0));
        if (colMajor && evenCol || !colMajor && evenRow)
            types.insert(types.begin(), k_sampled_1d);

        if (rows > 1 && evenRow && evenCol) 
            types.insert(types.begin(), k_sampled_2d); 
    }

    return types;
}


std::shared_ptr<KvData> KuDataUtil::makeData(const matrixd& mat, KeDataType type)
{
    std::shared_ptr<KvData> data;

    switch (type)
    {
    case k_series:
        data = makeSeries(mat);
        break;

    case k_matrix:
        data = makeMatrix(mat);
        break;

    case k_scattered_1d:
        data = makeScattered(mat, 2);
        break;

    case k_scattered_2d:
        data = makeScattered(mat, 3);
        break;

    case k_sampled_1d:
        data = makeSampled1d(mat);
        break;

    case k_sampled_2d:
        data = makeSampled2d(mat);
        break;

    default:
        assert(false);
    }

    return data;
}

const char* KuDataUtil::typeStr(KeDataType type)
{
    const char* text[] = {
        "series",
        "matrix",
        "scattered-1d",
        "scattered-2d",
        "sampled-1d",
        "sampled-2d"
    };

    return text[type];
}
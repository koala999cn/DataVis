#include "KuDataUtil.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtScattered.h"
#include "KuMatrixUtil.h"


std::vector<int> KuDataUtil::validTypes(const matrixd& mat, bool colMajor)
{
    if (mat.empty())
        return {};

    auto rows = mat.size();
    auto cols = mat[0].size();

    if (rows == 0 && cols == 0)
        return {};

    if (!colMajor)
        std::swap(rows, cols);

    std::vector<int> types;

    types.push_back(k_series);

    if (rows > 8 && cols > 8)
        types.insert(types.begin(), k_matrix);
    else
        types.push_back(k_matrix);

    if (cols > 1) {

        if (cols % 3 == 0)
            types.insert(types.begin(), k_scattered_2d);

        if (cols % 2 == 0)
            types.insert(types.begin(), k_scattered_1d);

        bool evenRow = KuMatrixUtil::isEvenlySpaced(mat[0]);
        bool evenCol = KuMatrixUtil::isEvenlySpaced(KuMatrixUtil::extractColumn(mat, 0));
        if (colMajor && evenCol || !colMajor && evenRow)
            types.insert(types.begin(), k_sampled_1d);

        if (rows > 1 && evenRow && evenCol)
            types.insert(types.begin(), k_sampled_2d);
    }

    return types;
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


std::shared_ptr<KvData> KuDataUtil::makeData(const matrixd& mat, int type)
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

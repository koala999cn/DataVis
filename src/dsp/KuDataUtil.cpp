#include "KuDataUtil.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtScattered.h"
#include "KuMatrixUtil.h"


std::vector<int> KuDataUtil::validTypes(const matrixd& mat, bool transpose)
{
    if (mat.empty() || mat.front().empty())
        return {};

    auto rows = mat.size();
    auto cols = mat[0].size();

    if (transpose)
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
        if (!transpose && evenCol || transpose && evenRow)
            types.insert(types.begin(), k_sampled_1d);

        if (rows > 2 && cols > 2 && evenRow && evenCol)
            types.insert(types.begin(), k_sampled_2d);
    }

    return types;
}


std::shared_ptr<KvData> KuDataUtil::makeSeries(const matrixd& mat)
{
    // 暂时使用sampled1d表示series数据

    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat.size(), mat[0].size());
    samp->reset(0, 0, 1);

    for (unsigned c = 0; c < samp->channels(); c++) {
        auto d = KuMatrixUtil::extractColumn(mat, c);
        samp->setChannel(nullptr, c, d.data());
    }

    return samp;
}



std::shared_ptr<KvData> KuDataUtil::makeMatrix(const matrixd& mat)
{
    auto samp2d = std::make_shared<KcSampled2d>(1, 1);
    samp2d->resize(mat[0].size(), mat.size());

    for (kIndex idx = 0; idx < samp2d->size(0); idx++) {
        auto d = KuMatrixUtil::extractColumn(mat, idx);
        samp2d->setChannel(&idx, 0, d.data()); // 始终单通道
    }

    return samp2d;
}


std::shared_ptr<KvData> KuDataUtil::makeSampled1d(const matrixd& mat)
{
    assert(!mat.empty() && !mat.front().empty());

    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat.size(), mat[0].size() - 1); // 少一列，第1列为采样时
    samp->reset(0, mat[0][0], mat[1][0] - mat[0][0]);

    for (unsigned c = 0; c < samp->channels(); c++) {
        auto d = KuMatrixUtil::extractColumn(mat, c + 1);
        samp->setChannel(0, c, d.data());
    }

    return samp;
}


std::shared_ptr<KvData> KuDataUtil::makeSampled2d(const matrixd& mat)
{
    // mat[0][0]为占位符，无效

    auto dx = mat[0][2] - mat[0][1];
    auto dy = mat[2][0] - mat[1][0];
    
    auto samp2d = std::make_shared<KcSampled2d>(dx, dy);
    samp2d->resize(mat[0].size() - 1, mat.size() - 1);
    samp2d->reset(0, mat[0][1], dx);
    samp2d->reset(1, mat[1][0], dy);

    for (kIndex idx = 0; idx < samp2d->size(0); idx++) {
        auto d = KuMatrixUtil::extractColumn(mat, idx + 1);
        samp2d->setChannel(&idx, 0, d.data() + 1); // 始终单通道, 忽略mat数据的首行首列
    }

    return samp2d;
}


std::shared_ptr<KvData> KuDataUtil::makeScattered(const matrixd& mat, unsigned dim)
{
    assert(dim == 2 || dim == 3);
    assert(mat[0].size() % dim == 0);

    kIndex chs = mat[0].size() / dim;
    if (dim == 2) {
        auto scattered = std::make_shared<KtScattered<1>>();
        scattered->resize(nullptr, chs);
        scattered->reserve(mat.size());

        for (unsigned i = 0; i < mat.size(); i++) {
            std::vector<typename KtScattered<1>::element_type> points;
            for (unsigned c = 0; c < chs; c++)
                points.push_back({ mat[i][c * 2], mat[i][c * 2 + 1] });

            scattered->pushBack(points.data());
        }

        return scattered;
    }
    else {
        auto scattered = std::make_shared<KtScattered<2>>();
        scattered->resize(nullptr, chs);
        scattered->reserve(mat.size());

        for (unsigned i = 0; i < mat.size(); i++) {
            std::vector<typename KtScattered<2>::element_type> points;
            for (unsigned c = 0; c < chs; c++)
                points.push_back({ mat[i][c * 3], mat[i][c * 3 + 1], mat[i][c * 3 + 2] });

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


std::shared_ptr<KvData> KuDataUtil::cloneSampled1d(std::shared_ptr<const KvData> samp)
{
    auto samp1d = std::dynamic_pointer_cast<const KvSampled>(samp);
    assert(samp1d && samp1d->dim() == 1);

    auto copyed = std::make_shared<KcSampled1d>();
    copyed->reset(0, samp1d->range(0).low(), samp1d->step(0)); // TODO: x0_ref
    copyed->resize(samp1d->size(), samp1d->channels());
    
    auto buf = copyed->data();
    for (kIndex i = 0; i < samp1d->size(); i++)
        for (kIndex ch = 0; ch < samp1d->channels(); ch++)
            *buf++ = samp1d->valueAt(i, ch);

    return copyed;
}


bool KuDataUtil::isMatrix(const KvData& d)
{
    if (d.dim() != 2)
        return false;

    return d.isContinued() || ((const KvDiscreted&)d).isSampled();
}


bool KuDataUtil::sameShape(const KvData& d1, const KvData& d2, bool sameStep)
{
    if (d1.isContinued() != d2.isContinued() 
        || d1.dim() != d2.dim()
        || d1.size() != d2.size())
        return false;

    auto pd1 = dynamic_cast<const KvDiscreted*>(&d1);
    if (pd1 == nullptr)
        return true; // 连续数据

    auto pd2 = dynamic_cast<const KvDiscreted*>(&d2);
    if (pd1->isSampled() != pd2->isSampled())
        return false;

    for (unsigned i = 0; i < pd1->dim(); i++) {
        if (pd1->size(i) != pd2->size(i))
            return false;

        if (sameStep && (pd1->step(i) != pd2->step(i)))
            return false;
    }

    return true;
}


std::shared_ptr<KvDiscreted> KuDataUtil::shapeLike(const KvDiscreted& disc)
{
    std::shared_ptr<KvDiscreted> res;

    if (disc.isSampled()) {
        auto samp = dynamic_cast<const KvSampled*>(&disc);
        assert(samp);

        std::shared_ptr<KvSampled> ress;

        switch (samp->dim()) {
        case 1:
            ress = std::make_shared<KtSampledArray<1>>();
            break;
        case 2:
            ress = std::make_shared<KtSampledArray<2>>();
            break;
        case 3:
            ress = std::make_shared<KtSampledArray<3>>();
            break;
        case 4:
            ress = std::make_shared<KtSampledArray<4>>();
            break;
        case 5:
            ress = std::make_shared<KtSampledArray<5>>();
            break;
        case 6:
            ress = std::make_shared<KtSampledArray<6>>();
            break;
        case 7:
            ress = std::make_shared<KtSampledArray<7>>();
            break;
        case 8:
            ress = std::make_shared<KtSampledArray<8>>();
            break;
        case 9:
            ress = std::make_shared<KtSampledArray<9>>();
            break;
        default:
            assert(false);
            return nullptr;
        }

        for (unsigned i = 0; i < samp->dim(); i++)
            ress->reset(i, samp->range(i).low(), samp->step(i)); // TODO: x0_ref
        res = ress;
    }
    else {
        switch (disc.dim()) {
        case 1:
            res = std::make_shared<KtScattered<1>>();
            break;
        case 2:
            res = std::make_shared<KtScattered<2>>();
            break;
        case 3:
            res = std::make_shared<KtScattered<3>>();
            break;
        case 4:
            res = std::make_shared<KtScattered<4>>();
            break;
        case 5:
            res = std::make_shared<KtScattered<5>>();
            break;
        case 6:
            res = std::make_shared<KtScattered<6>>();
            break;
        case 7:
            res = std::make_shared<KtScattered<7>>();
            break;
        case 8:
            res = std::make_shared<KtScattered<8>>();
            break;
        case 9:
            res = std::make_shared<KtScattered<9>>();
            break;
        default:
            assert(false);
            return nullptr;
        }
    }

    auto sh = shape(disc);
    res->resize(sh.data(), disc.channels());
    return res;
}


std::vector<kIndex> KuDataUtil::shape(const KvDiscreted& disc)
{
    std::vector<kIndex> s(disc.dim());
    for (kIndex d = 0; d < disc.dim(); d++)
        s[d] = disc.size(d);
    return s;
}


kIndex KuDataUtil::index2n(const std::vector<kIndex>& shape, const kIndex idx[])
{
    if (shape.empty())
        return idx[0];

    kIndex n(idx[shape.size() - 1]);
    kIndex stride = shape.back();
    for (kIndex i = shape.size() - 2; i != -1; i--) {
        n += idx[i] * stride;
        stride *= shape[i];
    }
    return n;
}


std::vector<kIndex> KuDataUtil::n2index(const std::vector<kIndex>& shape, kIndex n)
{
    auto dim = shape.size();

    // 快速通道
    if (dim <= 1)
        return { n };
    else if (dim == 2)
        return { n / shape[1], n % shape[1] };

    // 通用算法
    std::vector<kIndex> idx(dim);
    for (kIndex i = dim - 1; i != 0; i--) {
        idx[i] = n % shape[i];
        n /= shape[i];
    }

    idx[0] = n;

    return idx;
}


void KuDataUtil::nextIndex(const std::vector<kIndex>& shape, kIndex idx[])
{
    for (kIndex i = shape.size() - 1; i != -1; i--) {
        if (++idx[i] < shape[i])
            break;
        idx[i] = 0; // 进位
    }
}


void KuDataUtil::prevIndex(const std::vector<kIndex>& shape, kIndex idx[])
{
    for (kIndex i = shape.size() - 1; i != -1; i--) {
        if (--idx[i] != -1)
            break;
        idx[i] = shape[i] - 1; // 退位
    }
}


#include "KgRand.h"
void KuDataUtil::indexTest()
{
    KgRand r;
    for (int dim = 1; dim < 10; dim++) {
        std::vector<kIndex> shape(dim);
        for (int i = 0; i < dim; i++)
            shape[i] = r.rand(1, 10);

        kIndex size = KuMath::product(shape.data(), dim);
        for (kIndex i = 0; i < size; i++) {
            auto idx = n2index(shape, i);
            auto n = index2n(shape, idx.data());
            assert(n == i);

            prevIndex(shape, idx.data());
            auto prev = index2n(shape, idx.data());
            if (n == 0) {
                assert(prev == size - 1);
            }
            else {
                assert(prev == n - 1);

                if (i != size - 1) {
                    nextIndex(shape, idx.data());
                    assert(index2n(shape, idx.data()) == n);
                }
            }
        }
    }
}


KuDataUtil::KpValueGetter1d KuDataUtil::valueGetter1d(const std::shared_ptr<const KvData>& data)
{
    KpValueGetter1d g;

    if (data->isDiscreted()) {

        auto disc = std::dynamic_pointer_cast<const KvDiscreted>(data);
        assert(disc);
        g.samples = disc->size();
        g.channels = disc->channels();
        g.getter = [disc](unsigned ch, unsigned idx) {
            return disc->valueAt(idx, ch);
        };

        auto samp1d = std::dynamic_pointer_cast<const KcSampled1d>(disc);
        if (samp1d) {
            g.sampleStride = samp1d->stride(0);
            g.channelStride = samp1d->stride(1);
            g.data = samp1d->data();
        }
    }

    return g;
}


KuDataUtil::KpValueGetter2d KuDataUtil::valueGetter2d(const std::shared_ptr<const KvData>& data)
{
    KpValueGetter2d g;

    auto samp = std::dynamic_pointer_cast<const KvSampled>(data); 
    if (samp && samp->dim() == 2) {

        g.channels = samp->channels();
        g.xsize = samp->size(0);
        g.ysize = samp->size(1);

        g.getter = [samp](unsigned ch, unsigned row, unsigned col) {
            return samp->value(row, col, ch);
        };

        auto samp2d = std::dynamic_pointer_cast<const KcSampled2d>(samp);
        if (samp2d) {
            g.xstride = samp2d->stride(0);
            g.ystride = samp2d->stride(1);
            g.channelStride = samp2d->stride(2);
            g.data = samp2d->data();
        }
    }

    return g;
}


bool KuDataUtil::hasPointGetter2d(const KvData& d)
{
    if (d.dim() < 2)
        return false;

    return d.isContinued() || dynamic_cast<const KvSampled*>(&d);
}


unsigned KuDataUtil::pointGetter1dSize(const std::shared_ptr<const KvDiscreted>& disc)
{
    return disc->size(disc->dim() - 1);
}


unsigned KuDataUtil::pointGetter1dCount(const std::shared_ptr<const KvDiscreted>& disc)
{
    return disc->size() / pointGetter1dSize(disc);
}


unsigned KuDataUtil::pointGetter2dCount(const std::shared_ptr<const KvDiscreted>& disc)
{
    if (!hasPointGetter2d(*disc))
        return 0;

    unsigned c(1);
    for (kIndex i = 0; i < disc->dim() - 2; i++)
        c *= disc->size(i);

    return c;
}


KuDataUtil::KpPointGetter1d KuDataUtil::pointGetter1dAt(const std::shared_ptr<const KvDiscreted>& disc, unsigned ch, unsigned idx)
{
    KpPointGetter1d g;
    if (!hasPointGetter2d(*disc)) {
        assert(idx == 0);
        g.getter = [disc, ch](unsigned ix) { return disc->pointAt(ix, ch); };
        g.size = unsigned(disc->size(0));
    }
    else {
        auto g2 = pointGetter2dAt(disc, ch, idx / disc->size(disc->dim() - 2));
        auto ix = idx % disc->size(disc->dim() - 2);
        g.getter = [g2, ix](unsigned iy) { return g2.getter(ix, iy); };
        g.size = g2.ysize;
    }

    return g;
}


KuDataUtil::KpPointGetter2d KuDataUtil::pointGetter2dAt(const std::shared_ptr<const KvDiscreted>& disc, unsigned ch, unsigned idx)
{
    assert(hasPointGetter2d(*disc));
    KpPointGetter2d g;

    auto samp = std::dynamic_pointer_cast<const KvSampled>(disc);
    
    if (samp->dim() == 2) {
        g.getter = [samp, ch](unsigned ix, unsigned iy) {
            return samp->point(ix, iy, ch);
        };
    }
    else {
        auto s = shape(*samp);
        s.pop_back(), s.pop_back();
        auto v = n2index(s, idx);
        g.getter = [samp, ch, v](unsigned ix, unsigned iy) {
            std::vector<kIndex> idx(v);
            idx.push_back(ix), idx.push_back(iy);
            return samp->point(idx.data(), ch);
        };
    }

    g.xsize = unsigned(samp->size(samp->dim() - 2));
    g.ysize = unsigned(samp->size(samp->dim() - 1));

    return g;
}


KuDataUtil::KpPointGetter1d KuDataUtil::linesAt(const std::shared_ptr<const KvDiscreted>& disc, unsigned ch)
{
    auto lineSize = pointGetter1dSize(disc);
    std::vector<getter1d> lines;

    for (unsigned i = 0; i < pointGetter1dCount(disc); i++) {
        auto g = pointGetter1dAt(disc, ch, i);
        assert(lineSize == g.size);
        lines.push_back(g.getter);
    }

    return linesSet(lines, lineSize);
}


KuDataUtil::KpPointGetter1d KuDataUtil::pointsAt(const std::shared_ptr<const KvDiscreted>& disc, unsigned ch)
{
    KuDataUtil::KpPointGetter1d g;
    g.size = disc->size();
    g.getter = [disc, ch](unsigned idx) {
        return disc->pointAt(idx, ch);
    };

    return g;
}


KuDataUtil::KpPointGetter1d KuDataUtil::linesSet(const std::vector<getter1d>& getters, unsigned size)
{
    KuDataUtil::KpPointGetter1d g;
    std::vector<kReal> nan({ KuMath::nan<kReal>() });
    g.size = getters.size() * (size + 1) - 1; // 有getters.size()-1个nan
    g.getter = [getters, size, nan](unsigned idx) {
        auto i = idx % (size + 1);
        return i == size ? nan : getters[idx / (size + 1)](i);
    };

    return g;
}

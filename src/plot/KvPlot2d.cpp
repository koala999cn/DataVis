#include "KvPlot2d.h"
#include "KvCoord.h"
#include "KvPaint.h"


KvPlot2d::KvPlot2d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
    : KvPlot(paint, coord, 2)
{
    setMargins(15, 15, 15, 15);
}


void KvPlot2d::autoProject_()
{
    auto lower = paint().localToWorldP(coord().lower());
    auto upper = paint().localToWorldP(coord().upper());

    // ȷ��zֵ�������0�����ط�ʹ��0ΪĬ�ϵ�zֵ����
    if (lower.z() > 0)
        lower.z() = 0;
    else if (upper.z() < 0)
        upper.z() = 0;

    if (upper.z() == lower.z())
        upper.z() = lower.z() + 1; // ��ֹz��߶�Ϊ0�����򹹽�͸�Ӿ�����nanֵ

    auto proj = KtMatrix4<float_t>::projectOrtho(lower, upper);

    // ����z��λ�ã���near/farƽ�������㹻�ռ�
    // NB: !!! VERY IMPORTANT !!!
    proj = proj * mat4::buildTanslation({ 0, 0, lower.z() - 1 });

    setProjMatrix(proj);
}

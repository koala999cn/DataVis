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

    // ��ֹ����߶�Ϊ0�����򹹽�͸�Ӿ�����nanֵ
    // TODO: �������ط�ͳһ����ȷ������ĳ߶Ȳ�Ϊ0
    if (upper.x() == lower.x())
        lower.x() -= 1, upper.x() += 1;
    if (upper.y() == lower.y())
        lower.y() -= 1, upper.y() += 1;
    if (upper.z() == lower.z())
        upper.z() = lower.z() + 1; 

    auto depth = upper.z() - lower.z();
    lower.z() = 5 * depth;
    upper.z() = 400 * depth;
    auto proj = KtMatrix4<float_t>::projectOrtho(lower, upper);

    // ����z��λ�ã���near/farƽ�������㹻�ռ�
    // NB: !!! VERY IMPORTANT !!!
    proj = proj * mat4::buildTanslation({ 0, 0, -7 * depth });

    setProjMatrix(proj);
}

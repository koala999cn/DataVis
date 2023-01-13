#include "KvPlot3d.h"
#include "KvCoord.h"
#include "KvPaint.h"


KvPlot3d::KvPlot3d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: KvPlot(paint, coord, 3)
{
    mat3d<> rot;
    rot.fromEulerAngleXYZ(0.316, -0.595, 0);
    orient_ = quatd(rot);
}


void KvPlot3d::autoProject_()
{
    // local阵保存着坐标轴反转和交换信息，应在此结果上进行投影计算
    auto lower = paint().localToWorldP(coord().lower());
    auto upper = paint().localToWorldP(coord().upper());
    auto center = lower + (upper - lower) / 2;
    double radius = (upper - lower).length() / 2;

    auto zoom = zoom_; // 全局缩放标量，不作处理
    auto scale = paint().localToWorldV(scale_); // 有可能交换了坐标轴，此处要交换回来，否则会缩放被交换的其他坐标轴
    paint().pushCoord(KvPaint::k_coord_world);
    assert(shift_.z() == 0);
    auto shift = paint().unprojectv(shift_); // 偏移使用全局坐标，否则在反转和交换坐标轴的情况下，有违用户操作常识
    paint().popCoord();
    if (!isotropic_) {
        zoom *= 2 * radius / sqrt(3.);
        auto factor = upper - lower;
        for (unsigned i = 0; i < 3; i++)
            if (factor.at(i) == 0)
                factor.at(i) = 1;
        scale /= factor;
    }
    scale *= zoom;

    // 先平移至AABB中心点，再缩放，再旋转，最后处理用户设定的平移
    mat4 viewMat = mat4::buildTanslation(shift * scale) // NB: 若不作scale，尺寸短的坐标轴移动非常慢
        * mat4::buildRotation(orient_)
        * mat4::buildScale(scale)
        * mat4::buildTanslation(-center);

    if (radius == 0)
        radius = 1;

    mat4 projMat;
    if (ortho_)
        projMat = mat4::projectOrtho(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
    else
        projMat = mat4::projectFrustum(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);

    // 调整z轴位置，给near/far平面留出足够空间
    viewMat = mat4::buildTanslation({ 0, 0, -7 * radius }) * viewMat;

    setViewMatrix(viewMat);
    setProjMatrix(projMat);
}

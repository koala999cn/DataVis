#include "KvPlot3d.h"
#include "KvCoord.h"


KvPlot3d::KvPlot3d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: KvPlot(paint, coord)
{
    mat3d<> rot;
    rot.fromEulerAngleXYZ(0.316, -0.595, 0);
    orient_ = quatd(rot);
}


void KvPlot3d::autoProject_()
{
    auto lower = coord().lower();
    auto upper = coord().upper();
    auto center = lower + (upper - lower) / 2;
    double radius = (upper - lower).length() / 2;

    auto zoom = zoom_;
    auto scale = scale_;
    auto shift = shift_;
    if (!isometric_) {
        zoom *= 2 * radius / sqrt(3.);
        auto factor = upper - lower;
        for (unsigned i = 0; i < 3; i++)
            if (factor.at(i) == 0)
                factor.at(i) = 1;
        scale /= factor;
    }
    scale *= zoom;

    // 先平移至AABB中心点，再缩放，再旋转，最后处理用户设定的平移
    mat4 viewMat = mat4::buildTanslation(shift)
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
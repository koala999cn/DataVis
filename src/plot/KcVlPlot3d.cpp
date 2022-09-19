#include "KcVlPlot3d.h"
#include <vlWin32/Win32Window.hpp>
#include "KcCoordSystem.h"
#include "KvPlottable.h"
#include "KglPaint.h"
#include "KcPlotApplet_.h"


KcVlPlot3d::KcVlPlot3d(QWidget* parent)
{
    paint_ = std::make_unique<KglPaint>();

    /* create a native Qt6 window */
    widget_ = new vlWin32::Win32Window;

    /* create the applet to be run */
    applet_ = new kPrivate::KcPlotApplet_(this, paint_.get());
    applet_->initialize();

    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    widget_->addEventListener(applet_.get());

    /* target the window so we can render on it */
    applet_->rendering()->as<vl::Rendering>()->renderer()->setFramebuffer(widget_->framebuffer());
    //applet_->trackball()->setEnabled(false);
    //applet_->ghostCameraManipulator()->setEnabled(false);

    /* setup the OpenGL context format */
    vl::OpenGLContextFormat format;
    format.setDoubleBuffer(true);
    format.setRGBABits(8, 8, 8, 0);
    format.setDepthBufferBits(24);
    format.setStencilBufferBits(8);
    format.setMultisampleSamples(16);
    format.setMultisample(false);
    format.setFullscreen(false);

    /* Initialize the OpenGL context and window properties */
    int x = 0;
    int y = 0;
    int width = 512;
    int height = 512;
    widget_->initWin32GLWindow(NULL, NULL, "Visualization Library on Win32 - Rotating Cube", format, x, y, width, height);

    setBackground({ 1, 1, 1, 1 });

    coord_->setExtents({ 0, 0, 0 }, { 10, 20, 30 });
    autoProject_();
}


KcVlPlot3d::~KcVlPlot3d()
{
    applet_ = nullptr; // 停止渲染
    widget_ = nullptr;
}

void KcVlPlot3d::show(bool b)
{
    if (b)
        widget_->show();
    else
        widget_->hide();
}


bool KcVlPlot3d::visible() const
{
    return ::IsWindowVisible(widget_->hwnd()) && !::IsIconic(widget_->hwnd());
}


void* KcVlPlot3d::widget() const
{
    return (void*)widget_.get();
}


void KcVlPlot3d::updateImpl_()
{
    paint_->startDrawing();

    // 绘制背景?
    auto bkclr = background();
    paint_->clearColor(vl::fvec4(bkclr.r(), bkclr.g(), bkclr.b(), bkclr.a()));
    paint_->clearDepth(0);

    coordSystem().draw(paint_.get());

    for (int idx = 0; idx < plottableCount(); idx++)
        plottable(idx)->draw(paint_.get());

    paint_->endDrawing();
}

color4f KcVlPlot3d::background() const
{
    auto vp = applet_->rendering()->as<vl::Rendering>()->camera()->viewport();
    auto clr = vp->clearColor();
    return { clr.r(), clr.g(), clr.b(), clr.a() };
}


void KcVlPlot3d::setBackground(const color4f& clr)
{
    auto vp = applet_->rendering()->as<vl::Rendering>()->camera()->viewport();
    vp->setClearColor(clr.r(), clr.g(), clr.b(), clr.a());
}


void KcVlPlot3d::autoProject_()
{
    auto camera = applet_->rendering()->as<vl::Rendering>()->camera();
    auto lower = coordSystem().lower();
    auto upper = coordSystem().upper();
    auto center = lower + (upper - lower) / 2;
    double radius = (center - lower).length();

    auto zoom = getZoom();
    auto scale = getScale();
    auto rot = getRotate();
    auto shift = getShift();
    if (!isIsometric()) {
        zoom *= 2 * radius / sqrt(3.);
        scale /= (upper - lower);
    }
    scale *= zoom;

    vl::Transform tr;
    //tr.rotate(-90, 1, 0, 0); // 旋转+z轴由向外为向上, +y轴由向上为向内
    tr.translate(vl::vec3(shift - center)); // 把物理坐标AABB的中心点调整为摄像机坐标的原点
    tr.scale(scale.x(), scale.y(), scale.z());
    tr.rotate(rot.x(), 1, 0, 0);
    tr.rotate(rot.y(), 0, 1, 0);
    tr.rotate(rot.z(), 0, 0, 1);
    tr.translate(0, 0, -7 * radius); // 调整z轴位置，给near/far平面留出足够空间
    camera->setViewMatrix(tr.localMatrix());

    if (radius == 0)
        radius = 1;

    if (ortho_)
        camera->setProjectionOrtho(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
    else
        camera->setProjectionFrustum(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
}


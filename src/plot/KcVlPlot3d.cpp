#include "KcVlPlot3d.h"
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlCore/Time.hpp>
#include <vlGraphics/Light.hpp>
#include <vlQt6/Qt6Widget.hpp>
#include "KcCoordSystem.h"
#include "KvPlottable.h"
#include "KglPaint.h"


namespace kPrivate
{
    using namespace vl;

    class KcPlotSceneManager_ : public SceneManager
    {
        VL_INSTRUMENT_CLASS(KcPlotSceneManager_, SceneManager)

    public:
        KcPlotSceneManager_(KglPaint* paint) : paint_(paint) {}


        void extractVisibleActors(ActorCollection& queue, const Camera*) override
        {
            if (cullingEnabled())
                // FIXME: implement 2d/3d culling?
                extractActors(queue);
            else
                extractActors(queue);
        }

        void extractActors(ActorCollection& queue) override
        {
            int actor_rank = 0;
            for (size_t i = 0; i < paint_->actors()->size(); ++i) {
                paint_->actors()->at(i)->setRenderRank(actor_rank++);
                queue.push_back(paint_->actors()->at(i));
            }
        }

    private:
        KglPaint* paint_;
    };

    class KcPlotApplet_: public vl::Applet
    {
    public:
        KcPlotApplet_(KcVlPlot3d* plot3d, KglPaint* paint) 
            : plot3d_(plot3d), paint_(paint) {}

        // called once after the OpenGL window has been opened
        void initEvent() override;

        // called every frame
        void updateScene() override;

        void resizeEvent(int w, int h) override;

    private:
        vl::ref<vl::Transform> tr_; // plot的全局旋转矩阵，用于支持plot的自动旋转功能
        KcVlPlot3d* plot3d_;
        KglPaint* paint_;
    };

    void KcPlotApplet_::initEvent()
    {
        // allocate the Transform
        tr_ = new vl::Transform;
        // bind the Transform with the transform tree of the rendring pipeline
        rendering()->as<vl::Rendering>()->transform()->addChild(tr_.get());

        vl::ref<KcPlotSceneManager_> scene_manager = new KcPlotSceneManager_(paint_);
        rendering()->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
    }

    void KcPlotApplet_::updateScene()
    {
        // rotates the cube around the Y axis 45 degrees per second
        //vl::real degrees = vl::Time::currentTime() * 45.0f;
        //vl::mat4 matrix = vl::mat4::getRotation(degrees, 0, 1, 0);
        //tr_->setLocalMatrix(matrix);

        plot3d_->update();
    }

    void KcPlotApplet_::resizeEvent(int w, int h)
    {
        rendering()->as<vl::Rendering>()->camera()->viewport()->setWidth(w);
        rendering()->as<vl::Rendering>()->camera()->viewport()->setHeight(h);
        //rendering()->as<vl::Rendering>()->camera()->setProjectionOrtho(-0.5f);
    }
}


KcVlPlot3d::KcVlPlot3d(QWidget* parent)
    : KvPlot(new KcCoordSystem)
{
    paint_ = std::make_unique<KglPaint>();

    /* create a native Qt6 window */
    widget_ = new vlQt6::Qt6Widget(parent);

    /* create the applet to be run */
    applet_ = new kPrivate::KcPlotApplet_(this, paint_.get()); 
    applet_->initialize();

    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    widget_->addEventListener(applet_.get());

    /* target the window so we can render on it */
    applet_->rendering()->as<vl::Rendering>()->renderer()->setFramebuffer(widget_->framebuffer());

    /* define the camera position and orientation */
    autoProject();

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
    int width = 96;
    int height = 96;
    widget_->initQt6Widget("kPlot", format, x, y, width, height);

    setBackground({1, 1, 1, 1});
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
    return widget_->isVisible();
}


void* KcVlPlot3d::widget() const
{
    return (void*)widget_.get();
}


void KcVlPlot3d::update(bool immediately)
{
    paint_->startDrawing();

    // 绘制背景?
    auto bkclr = background();
    paint_->clearColor(vl::fvec4(bkclr.r(), bkclr.g(), bkclr.b(), bkclr.a()));

    coordSystem()->draw(paint_.get());

    for (int idx = 0; idx < numPlottables(); idx++)
        plottable(idx)->draw(paint_.get());

    paint_->endDrawing();

    //if (immediately) // TODO: 优化
    //    widget_->update();
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


void KcVlPlot3d::autoProject()
{
    auto camera = applet_->rendering()->as<vl::Rendering>()->camera();
    auto lower = coordSystem()->lower();
    auto upper = coordSystem()->upper();
    auto center = coordSystem()->center();
    double radius = coordSystem()->diag() / 2;

    point3d scale(1 / (upper.x() - lower.x()), 1 / (upper.y() - lower.y()), 1 / (upper.z() - lower.z()));
    double zoom = 2 * radius / sqrt(3.);
    scale *= zoom;
    vl::vec3 shift(0, 0, 0);

    vl::Transform tr;
    tr.rotate(-90, 1, 0, 0); // 旋转+z轴由向外为向上, +y轴由向上为向内
    tr.translate(shift.x() - center.x(), shift.y() - center.y(), shift.z() - center.z()); // 把物理坐标AABB的中心点调整为摄像机坐标的原点
    tr.scale(scale.x(), scale.y(), scale.z());

    tr.translate(0, 0, -7 * radius); // 调整摄像机z轴位置，给near/far平面留出足够空间
    camera->setViewMatrix(tr.localMatrix());

    if (radius == 0)
        radius = 1;

    if (ortho_)
        camera->setProjectionOrtho(-radius, +radius, -radius, +radius, 0, 40 * radius);
    else
        camera->setProjectionFrustum(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
}


#include "KcVlPlotWidget.h"
#include <vlQt6/Qt6Widget.hpp>
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlCore/Time.hpp>
#include <vlGraphics/Light.hpp>


namespace kPrivate
{
    struct KpVlPlotWidgetImpl_
    {
        vl::ref<vl::Applet> applet;
        vl::ref<vlQt6::Qt6Widget> widget;
    };

    class KcPlotApp_ : public vl::Applet
    {
    public:
        // called once after the OpenGL window has been opened
        void initEvent() override;

        // called every frame
        void updateScene() override;

    protected:
        vl::ref<vl::Transform> globalTransform_; // plot的全局旋转矩阵，用于支持plot的自动旋转功能
    };

    void KcPlotApp_::initEvent()
    {
        // allocate the Transform
        globalTransform_ = new vl::Transform;
        // bind the Transform with the transform tree of the rendring pipeline
        rendering()->as<vl::Rendering>()->transform()->addChild(globalTransform_.get());

        // create the cube's Geometry and compute its normals to support lighting
        vl::ref<vl::Geometry> cube = vl::makeBox(vl::vec3(0, 0, 0), 10, 10, 10);
        cube->computeNormals();

        // setup the effect to be used to render the cube
        vl::ref<vl::Effect> effect = new vl::Effect;
        // enable depth test and lighting
        effect->shader()->enable(vl::EN_DEPTH_TEST);
        // add a Light to the scene, since no Transform is associated to the Light it will follow the camera
        effect->shader()->setRenderState(new vl::Light, 0);
        // enable the standard OpenGL lighting
        effect->shader()->enable(vl::EN_LIGHTING);
        // set the front and back material color of the cube
        // "gocMaterial" stands for "get-or-create Material"
        effect->shader()->gocMaterial()->setDiffuse(vl::crimson);

        // install our scene manager, we use the SceneManagerActorTree which is the most generic
        vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
        rendering()->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
        // add the cube to the scene using the previously defined effect and transform
        scene_manager->tree()->addActor(cube.get(), effect.get(), globalTransform_.get());
    }

    void KcPlotApp_::updateScene()
    {
        // rotates the cube around the Y axis 45 degrees per second
        vl::real degrees = vl::Time::currentTime() * 45.0f;
        vl::mat4 matrix = vl::mat4::getRotation(degrees, 0, 1, 0);
        globalTransform_->setLocalMatrix(matrix);
    }
}


KcVlPlotWidget::KcVlPlotWidget(widget_t* parent)
{
    using namespace vl;

    auto d = new kPrivate::KpVlPlotWidgetImpl_;
    d_ptr_ = d;

    /* setup the OpenGL context format */
    OpenGLContextFormat format;
    format.setDoubleBuffer(true);
    format.setRGBABits(8, 8, 8, 0);
    format.setDepthBufferBits(24);
    format.setStencilBufferBits(8);
    format.setMultisampleSamples(16);
    format.setMultisample(false);
    format.setFullscreen(false);

    /* create the applet to be run */
    d->applet = new kPrivate::KcPlotApp_;
    d->applet->initialize();

    /* create a native Qt6 window */
    d->widget = new vlQt6::Qt6Widget((QWidget*)parent);

    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    d->widget->addEventListener(d->applet.get());

    /* target the window so we can render on it */
    d->applet->rendering()->as<Rendering>()->renderer()->setFramebuffer(d->widget->framebuffer());

    /* black background */
    d->applet->rendering()->as<Rendering>()->camera()->viewport()->setClearColor(black);

    /* define the camera position and orientation */
    vec3 eye = vec3(0, 10, 35);  // camera position
    vec3 center = vec3(0, 0, 0); // point the camera is looking at
    vec3 up = vec3(0, 1, 0);     // up direction
    mat4 view_mat = mat4::getLookAt(eye, center, up);
    d->applet->rendering()->as<Rendering>()->camera()->setViewMatrix(view_mat);

    /* Initialize the OpenGL context and window properties */
    int x = 0;
    int y = 0;
    int width = 96;
    int height = 96;
    d->widget->initQt6Widget("kPlot", format, x, y, width, height);

    /* show the window */
    //d->widget->show();
}


KcVlPlotWidget::~KcVlPlotWidget()
{
    delete static_cast<kPrivate::KpVlPlotWidgetImpl_*>(d_ptr_);
}


KcVlPlotWidget::widget_t* KcVlPlotWidget::widget() const
{
    auto d = static_cast<kPrivate::KpVlPlotWidgetImpl_*>(d_ptr_);
    return (widget_t*)d->widget.get();
}


void KcVlPlotWidget::show(bool b)
{
    auto w = (vlQt6::Qt6Widget*)widget();
    if (b)
        w->show();
    else
        w->hide();
}


bool KcVlPlotWidget::visible() const
{
    auto w = (vlQt6::Qt6Widget*)widget();
    return w->isVisible();
}


void KcVlPlotWidget::draw(KvRenderer*) const
{

}


void KcVlPlotWidget::update(bool immediately)
{

}

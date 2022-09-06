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
#include "KcVlCoordSystem.h"
#include "KvPlottable.h"


namespace kPrivate
{
    class KcVlPlotApplet_: public vl::Applet
    {
    public:
        // called once after the OpenGL window has been opened
        void initEvent() override;

        // called every frame
        void updateScene() override;

    protected:
        vl::ref<vl::Transform> globalTransform_; // plot的全局旋转矩阵，用于支持plot的自动旋转功能
    };

    void KcVlPlotApplet_::initEvent()
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

        // install our scene manager, we use the SceneManagerActorTree which is the most generic
        vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
        rendering()->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
        // add the cube to the scene using the previously defined effect and transform
        //scene_manager->tree()->addActor(cube.get(), effect.get(), globalTransform_.get());
    }

    void KcVlPlotApplet_::updateScene()
    {
        // rotates the cube around the Y axis 45 degrees per second
        //vl::real degrees = vl::Time::currentTime() * 45.0f;
        //vl::mat4 matrix = vl::mat4::getRotation(degrees, 0, 1, 0);
        //globalTransform_->setLocalMatrix(matrix);
    }
}


KcVlPlot3d::KcVlPlot3d(QWidget* parent)
    : KvPlot(new KcVlCoordSystem)
{
    /* create a native Qt6 window */
    widget_ = new vlQt6::Qt6Widget(parent);

    /* create the applet to be run */
    applet_ = new kPrivate::KcVlPlotApplet_; 
    applet_->initialize();

    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    widget_->addEventListener(applet_.get());

    /* target the window so we can render on it */
    applet_->rendering()->as<vl::Rendering>()->renderer()->setFramebuffer(widget_->framebuffer());

    /* black background */
    applet_->rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor(vl::black);

    /* define the camera position and orientation */
    vl::vec3 eye = vl::vec3(0, 10, 35);  // camera position
    vl::vec3 center = vl::vec3(0, 0, 0); // point the camera is looking at
    vl::vec3 up = vl::vec3(0, 1, 0);     // up direction
    vl::mat4 view_mat = vl::mat4::getLookAt(eye, center, up);
    applet_->rendering()->as<vl::Rendering>()->camera()->setViewMatrix(view_mat);

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

    /* show the window */
    //d->widget->show();
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


void KcVlPlot3d::update(bool immediately)
{
    coordSystem()->update(this);

    for (int idx = 0; idx < numPlottables(); idx++)
        plottable(idx)->update(this);
}

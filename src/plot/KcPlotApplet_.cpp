#include "KcPlotApplet_.h"
#include "KvPlot.h"
#include "KglPaint.h"
#include "KcCoordSystem.h"


namespace kPrivate
{
    class KcPlotSceneManager_ : public vl::SceneManager
    {
        VL_INSTRUMENT_CLASS(KcPlotSceneManager_, vl::SceneManager)

    public:
        KcPlotSceneManager_(KglPaint* paint) : paint_(paint) {}


        void extractVisibleActors(vl::ActorCollection& queue, const vl::Camera*) override
        {
            if (cullingEnabled())
                // FIXME: implement 2d/3d culling?
                extractActors(queue);
            else
                extractActors(queue);
        }

        void extractActors(vl::ActorCollection& queue) override
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


    KcPlotApplet_::KcPlotApplet_(KvPlot* plot3d, KglPaint* paint)
        : plot3d_(plot3d), paint_(paint)
    {
        btn_ = vl::NoButton;
        zoomSpeed_ = 1;
        rotateSpeed_ = 1;
        shiftSpeed_ = 1;

        posX_ = posY_ = 0;
    }


    vl::Viewport* KcPlotApplet_::viewport_()
    {
        return rendering()->as<vl::Rendering>()->camera()->viewport();
    }


    void KcPlotApplet_::mapToViewport_(int& x, int& y)
    {
        // set x/y relative to the viewport
        x -= viewport_()->x();
        y -= openglContext()->framebuffer()->height() - 1 - (viewport_()->y() + viewport_()->height() - 1);
    }


    void KcPlotApplet_::initEvent()
    {
        auto r = rendering()->as<vl::Rendering>();
        r->sceneManagers()->push_back(new kPrivate::KcPlotSceneManager_(paint_));
        r->renderer()->setClearFlags(vl::CF_CLEAR_COLOR_DEPTH);
    }


    void KcPlotApplet_::updateScene()
    {
        if (plot3d_->visible())
            plot3d_->update();
    }


    void KcPlotApplet_::resizeEvent(int w, int h)
    {
        viewport_()->setWidth(w);
        viewport_()->setHeight(h);
        rendering()->as<vl::Rendering>()->camera()->setProjectionOrtho(-0.5f);
    }


    void KcPlotApplet_::mouseDownEvent(vl::EMouseButton btn, int x, int y)
    {
        // if already busy ignore the event
        if (btn_ != vl::NoButton)
            return;

        // enter new mode
        btn_ = btn;

        VL_CHECK(openglContext()->framebuffer());

        mapToViewport_(x, y);

        // check that the click is in the viewport
        int w = viewport_()->width();
        int h = viewport_()->height();

        if (x < 0 || y < 0 || x >= w || y >= h)
            return;

        posX_ = x, posY_ = y;
    }


    void KcPlotApplet_::mouseUpEvent(vl::EMouseButton btn, int x, int y)
    {
        btn_ = vl::NoButton;
        posX_ = x, posY_ = y;
    }


    void KcPlotApplet_::mouseMoveEvent(int x, int y)
    {
        VL_CHECK(openglContext()->framebuffer())

            mapToViewport_(x, y);

        auto dx = posX_ - x;
        auto dy = -(posY_ - y);

        if (btn_ == vl::LeftButton) {
            doRotate_(dx, dy);
            updateScene();
            posX_ = x, posY_ = y;
        }
        else if (btn_ == vl::RightButton) {
            doShift_(dx, dy);
            updateScene();
            posX_ = x, posY_ = y;
        }
    }


    void KcPlotApplet_::doRotate_(int dx, int dy)
    {
        double relx = 90 * dx * rotateSpeed_ / viewport_()->width();
        double relyz = 90 * dy * rotateSpeed_ / viewport_()->height();

        plot3d_->rotate({ relyz, -relx, -relx });
    }


    void KcPlotApplet_::doShift_(int dx, int dy)
    {
        double relx = dx * shiftSpeed_ / viewport_()->width();
        double rely = dy * shiftSpeed_ / viewport_()->height();

        auto box = plot3d_->coordSystem().boundingBox();
        auto delta = (box.upper() - box.lower()) * point3d(relx, rely, 0);
        plot3d_->shift(-delta);
    }


    void KcPlotApplet_::mouseWheelEvent(int delta)
    {
        // 自己实现zoom功能. trackball靠调整摄像机的远近进行zoom，在ortho模式投影下无效

        double factor = 1 + double(delta) / WHEEL_DELTA;
        factor *= zoomSpeed_;

        if (btn_ == vl::NoButton) {
            plot3d_->zoom(factor);
            updateScene();
        }
        else if (btn_ == vl::LeftButton) {
            plot3d_->coordSystem().zoom(factor);
            updateScene();
        }
    }

} // namespace kPrivate
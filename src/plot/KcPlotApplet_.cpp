#include "KcPlotApplet_.h"
#include "KvPlot.h"
#include "KglPaint.h"
#include "KcCoordSystem.h"
#include "KtuMath.h"
#include "KtMatrix3.h"


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

        VL_CHECK(openglContext()->framebuffer());

        if (viewport_()->isPointInside(x, y, openglContext()->framebuffer()->height())) {
            // enter new mode
            btn_ = btn;
            posX_ = x, posY_ = y;
        }
    }


    void KcPlotApplet_::mouseUpEvent(vl::EMouseButton btn, int x, int y)
    {
        btn_ = vl::NoButton;
    }


    void KcPlotApplet_::mouseMoveEvent(int x, int y)
    {
        VL_CHECK(openglContext()->framebuffer());

        if (btn_ == vl::LeftButton) {
            doRotate_(x, y);
            updateScene();
            posX_ = x, posY_ = y;
        }
        else if (btn_ == vl::RightButton) {
            doShift_(x, y);
            updateScene();
            posX_ = x, posY_ = y;
        }
    }


    void KcPlotApplet_::doRotate_(int x, int y)
    {
        //auto dx = x - posX_;
        //auto dy = y - posY_;

        // 鼠标每移动一个viewport长度，旋转90度
        //double relx = dx * rotateSpeed_ / viewport_()->width() * 90;
        //double relyz = dy * rotateSpeed_ / viewport_()->height() * 90;

        //plot3d_->rotate({ relyz, relx, relx });

        //return;

        // 参考lv实现的trackball旋转算法

        auto a = projectToTrackball_(posX_, posY_);
        auto b = projectToTrackball_(x, y);
        auto dir = a.cross(b);
        dir.normalize();
        auto dot_a_b = a.dot(b);
        dot_a_b = KtuMath<double>::clamp(dot_a_b, -1.0, +1.0);
        auto angle = acos(dot_a_b) * rotateSpeed_;

        angle *= 180 / KtuMath<double>::pi; // rad2deg
        plot3d_->rotate(dir * angle * 6); // FIXME: 暂时用dir在各轴的投影分量模拟trackball旋转，效果凑合
                                          // TODO: (旋转+平移后，出现旋转错乱情况)
    }


    void KcPlotApplet_::doShift_(int x, int y)
    {
        auto dx = x - posX_;
        auto dy = y - posY_;

        double relx = dx * shiftSpeed_ / viewport_()->width();
        double rely = dy * shiftSpeed_ / viewport_()->height();

        auto box = plot3d_->coordSystem().boundingBox();
        auto delta = (box.upper() - box.lower()) * point3d(relx, -rely, 0); // 屏幕的y轴坐标与视图的y轴坐标反向，此处取-rely
        plot3d_->shift(delta);
    }


    void KcPlotApplet_::mouseWheelEvent(int delta)
    {
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


    vec3d KcPlotApplet_::projectToTrackball_(int x, int y)
    {
        int w = viewport_()->width();
        int h = viewport_()->height();

        vec3d c(w / 2.0f, h / 2.0f, 0);

        // 设定整个屏幕为trackball的展开，计算trackball的半径
        double sphere_x = w * 0.5;
        double sphere_y = h * 0.5;

        // 将x, y归一化
        vec3d v(x, y, 0);
        v -= c;
        v.x() /= sphere_x;
        v.y() /= sphere_y;
        v.y() = -v.y();

        // 算法关键在于求z
        // 若sqrt(x*x+y*y) <= r/sqrt(2), z = sqrt(r*r-x*x-y*y)
        // 否则, z = r*r/2/sqrt(x*x+y*y)
        // 在归一化情况下，r=1
        double xy2 = v.x() * v.x() + v.y() * v.y();
        if (xy2 <= 0.5)
            v.z() = std::sqrt(1 - xy2);
        else
            v.z() = 0.5 / std::sqrt(xy2);

        v.normalize();
        return v;
    }

} // namespace kPrivate

#pragma once
#include <vlGraphics/Applet.hpp>


// 捕获处理窗口事件，实现plot的旋转、缩放、平移等操作
// 旋转: 鼠标左键+移动
// 缩放: 鼠标滚轮
// 平移: 鼠标右键+移动
// 坐标系缩放: 鼠标左键+滚轮

class KvPlot;
class KglPaint;

namespace kPrivate {

    class KcPlotApplet_ : public vl::Applet
    {
    public:
        KcPlotApplet_(KvPlot* plot3d, KglPaint* paint);

        // called once after the OpenGL window has been opened
        void initEvent() override;

        // called every frame
        void updateScene() override;

        void resizeEvent(int w, int h) override;

        void mouseMoveEvent(int, int) override;

        void mouseUpEvent(vl::EMouseButton, int, int) override;

        void mouseDownEvent(vl::EMouseButton, int, int) override;

        void mouseWheelEvent(int) override;

    private:
        vl::Viewport* viewport_();
        void mapToViewport_(int& x, int& y);

        void doRotate_(int dx, int dy);
        void doShift_(int dx, int dy);

    private:
        KvPlot* plot3d_;
        KglPaint* paint_;
        vl::EMouseButton btn_; // 标记当前鼠标哪个按键被触发

        int posX_, posY_; // 鼠标的前序位置

        double zoomSpeed_;
        double rotateSpeed_;
        double shiftSpeed_;
    };

} // namespace kPrivate
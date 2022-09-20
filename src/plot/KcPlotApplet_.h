#pragma once
#include <vlGraphics/Applet.hpp>
#include "KtVector3.h"


// ���������¼���ʵ��plot����ת�����š�ƽ�ƵȲ���
// ��ת: ������+�ƶ�
// ����: ������
// ƽ��: ����Ҽ�+�ƶ�
// ����ϵ����: ������+����

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

        void doRotate_(int x, int y);
        void doShift_(int x, int y);

        // trackball����������������Ļ������㷽��ʸ����������ת����
        vec3d computeVector_(int x, int y);

    private:
        KvPlot* plot3d_;
        KglPaint* paint_;
        vl::EMouseButton btn_; // ��ǵ�ǰ����ĸ�����������

        int posX_, posY_; // ����ǰ��λ��

        double zoomSpeed_;
        double rotateSpeed_;
        double shiftSpeed_;
    };

} // namespace kPrivate
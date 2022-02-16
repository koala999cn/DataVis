#pragma once
#include <QWidget>

// 用于麦克风输入音量实时显示的组件

class QPaintEvent;

class QtVolumnBar : public QWidget
{
    Q_OBJECT

public:
    QtVolumnBar(QWidget *parent);

    // 设置左右两个声道的音量值[0, 1]
    // 如果volRight < 0，则volRight = volLeft
    void setVolumn(float volLeft, float volRight = -1) {
        volLeft_ = volLeft, volRight_ = volRight;
        if(volRight < 0) volRight_ = volLeft;
    }

    // 设置前景色，从左至右线性渐变
    void setForeground(QColor left, QColor right) {
        leftColor_ = left, rightColor_ = right;
    }

    void setBlockSize(int width, int space) {
        widthColorBlock_ = width, spaceColorBlock_ = space;
    }


protected:
    void paintEvent(QPaintEvent *);

private:
    float volLeft_, volRight_; // [0, 1]
    QColor leftColor_, rightColor_;
    int widthColorBlock_, spaceColorBlock_;
};

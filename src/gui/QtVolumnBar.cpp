#include "QtVolumnBar.h"
#include <QPainter>


QtVolumnBar::QtVolumnBar(QWidget *parent)
    : QWidget(parent)
{
    // 设置当前窗口无标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // 设置当前窗口背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    setForeground(QColor(145, 218, 204), QColor(47, 141, 237));
    setBlockSize(4, 2);
    setVolumn(0.8f);
}


void QtVolumnBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient linear(0, 0, width(), height()); // TODO: 支持垂直模式

    linear.setColorAt(0, leftColor_);
    linear.setColorAt(1, rightColor_);

    linear.setSpread(QGradient::PadSpread);
    painter.setPen(Qt::transparent); //QPen(QColor(255, 255, 255, 0), 1));

    painter.setBrush(QBrush(linear));

    // draw color blocks
    int widthDraw = int(width() * std::min(volLeft_, volRight_) + 0.5f);
    int x = 0;
    for (; x < widthDraw; x += (widthColorBlock_ + spaceColorBlock_)) {
        int widthBlock = widthDraw - x;
        if(widthBlock > widthColorBlock_)
            widthBlock = widthColorBlock_;
        painter.drawRect(QRect(x, 0, widthBlock, height()));
    }

    widthDraw = int(width() * std::max(volLeft_, volRight_) + 0.5f);
    int y0 = volLeft_ > volRight_ ? 0 : height() / 2;
    int y1 = volLeft_ > volRight_ ? height() / 2 : height();
    for (; x < widthDraw; x += (widthColorBlock_ + spaceColorBlock_)) {
        int widthBlock = widthDraw - x;
        if(widthBlock > widthColorBlock_)
            widthBlock = widthColorBlock_;
        painter.drawRect(QRect(x, y0, widthBlock, y1 - y0));
    }

    // draw block-frame
    //painter.setPen(Qt::lightGray);
    //painter.setBrush(Qt::NoBrush);
    //for (; x < width(); x += (widthColorBlock_ + spaceColorBlock_))
    //    painter.drawRect(QRect(x, 0, widthColorBlock_, height()));
}

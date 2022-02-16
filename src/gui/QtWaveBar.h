#pragma once
#include <QWidget>
#include <vector>


// 用于显示波形谱
class QtWaveBar : public QWidget
{
    Q_OBJECT

public:
    explicit QtWaveBar(QWidget *parent = nullptr);

    void setForeground(QColor color) {
        barColor_ = color;
    }

    void setBarSize(int width, int space) {
        barWidth_ = width, barSpace_ = space;
    }

    int getBarCount() const;

    void setBarRanges(const std::vector<std::pair<float, float>>& ranges) {
        barRanges_ = ranges;
    }

protected:
    void paintEvent(QPaintEvent *);


private:
    QColor barColor_;
    int barWidth_, barSpace_;
    std::vector<std::pair<float, float>> barRanges_;
};


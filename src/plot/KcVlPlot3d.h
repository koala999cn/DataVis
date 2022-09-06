#pragma once
#include "KvPlot.h"
#include <vlCore/Object.hpp>

class QWidget;
namespace vlQt6 { class Qt6Widget; }
namespace vl { class Applet; }

class KcVlPlot3d : public KvPlot
{
public:
    KcVlPlot3d(QWidget* parent = nullptr);
    ~KcVlPlot3d();

    vl::Applet* applet() { return applet_.get(); }

    void show(bool b) override;

    bool visible() const override;

    void* widget() const override;

    void update(bool immediately = true) override;

private:
    vl::ref<vl::Applet> applet_;
    vl::ref<vlQt6::Qt6Widget> widget_;
};

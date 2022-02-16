#pragma once

#include "kddockwidgets/DockWidget.h"
#include "kddockwidgets/MainWindow.h"
#include <memory>

class KvData1d;
class KcPlotWidget;
class KcAudioRender;


namespace Ui {
class MainFrame;
}

class KcMainFrame : public KDDockWidgets::MainWindow
{
    Q_OBJECT

public:
    explicit KcMainFrame(const QString &uniqueName);
    ~KcMainFrame();

    bool init();

    void plotData(const KvData1d* data, KcPlotWidget* widget = nullptr);


private slots:
    void openDataFile();
    void openAudioFile();
    void openAudioCapture();
    void openFormula();

private:
    Ui::MainFrame *ui;
    QMenu *m_toggleMenu = nullptr;
    KDDockWidgets::DockWidget::List dockWidgets_;
    std::shared_ptr<KvData1d> data_;
    std::shared_ptr<KcAudioRender> render_; // 应用程序的全局播放设备

private:

    bool setupMenu_();
    bool createLauout_();

    std::shared_ptr<KvData1d> loadData_(const QString& filePath);
    KcPlotWidget* createPlotDock_(const QString& plotName);
};


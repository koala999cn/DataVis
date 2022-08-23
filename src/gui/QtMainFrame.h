#pragma once
#include <memory>
#include "kddockwidgets/DockWidget.h"
#include "kddockwidgets/MainWindow.h"


class KvData;
class KvDataProvider;
class KvDataOperator;
class KvDataRender;


class QtMainFrame : public KDDockWidgets::MainWindow
{
    Q_OBJECT

public:
    QtMainFrame();
    ~QtMainFrame();

private slots:
    void openTxtData();
    void openXlsFile();
    void openAudioFile();
    void openFormula();
    void openStochastic();
    void openAudioCapture();
    void openAudioInput();

private:
    bool setupMenu_();
    bool initLauout_();

    QMenu* setupPvMenu_();
    QMenu* setupVwMenu_();
    QMenu* setupOpMenu_();
    QMenu* setupRdMenu_();

    // 连接app全局信号槽
    void connectAppEvents_();

private:
    KDDockWidgets::DockWidget* workDock_;
    KDDockWidgets::DockWidget* propDock_;
};

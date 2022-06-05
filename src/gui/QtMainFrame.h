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
    void openDataFile();
    void openAudioFile();
    void openFormula();
    void openExcitor();
    void openAudioCapture();
    void openAudioInput();

private:
    bool setupMenu_();
    bool initLauout_();

    std::shared_ptr<KvData> loadData_(const QString& filePath);

    // 连接app全局信号槽
    void connectAppEvents_();

private:
    KDDockWidgets::DockWidget* workDock_;
    KDDockWidgets::DockWidget* propDock_;
};

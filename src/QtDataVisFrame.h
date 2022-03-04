#pragma once
#include <memory>
#include "kddockwidgets/DockWidget.h"
#include "kddockwidgets/MainWindow.h"


class KvData;
class KvDataProvider;
class KvDataOperator;
class KvDataRender;


class QtDataVisFrame : public KDDockWidgets::MainWindow
{
    Q_OBJECT

public:
    QtDataVisFrame();
    ~QtDataVisFrame();

private slots:
    void openDataFile();
    void openAudioFile();
    void openFormula();
    void openAudioCapture();
    void openAudioInput();

private:
    bool setupMenu_();
    bool initLauout_();

    /*
    void insertDataPlot_(int type);

    // TODO: 以下三个函数是否可合为一个
    void insertDataProvider_(KvDataProvider* dp);
    void insertDataOperator_(KvDataOperator* op);
    void insertDataRender_(KvDataRender* dr);*/

    std::shared_ptr<KvData> loadData_(const QString& filePath);

    // 连接app全局信号槽
    void connectAppEvents_();

private:
    KDDockWidgets::DockWidget* workDock_;
    KDDockWidgets::DockWidget* propDock_;
    //KDDockWidgets::DockWidget* dockPlotProp_; // 绘图设置
};

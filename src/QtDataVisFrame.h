#pragma once
#include <memory>
#include "kddockwidgets/DockWidget.h"
#include "kddockwidgets/MainWindow.h"


class KvData;
class KcPlotWidget;
class KcAudioRender;
class KvInputSource;
class QTreeWidgetItem;
class KcDataSnapshot;


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

    void newInputSource_(KvInputSource* is);
    std::shared_ptr<KvData> loadData_(const QString& filePath);

    // 连接app全局信号槽
    void connectAppEvents_();

private:
    KDDockWidgets::DockWidget* dockDataSource_; // 数据源列表
    KDDockWidgets::DockWidget* dockDataProp_; // 数据属性
    KDDockWidgets::DockWidget* dockPlotProp_; // 绘图设置
};

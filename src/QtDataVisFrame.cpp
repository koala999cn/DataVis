#include <fstream>
#include <string>
#include <QFileDialog>
#include "QtDataVisFrame.h"
#include "KcFormulaDlg.h"
#include "KcAudioCaptureDlg.h"
#include "base/KuStrUtil.h"
#include "dsp/KvData1d.h"
#include "dsp/KcFormulaData1d.h"
#include "dsp/KcSampled1d.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioRender.h"
#include "gui/QtAudioUtils.h"
#include "QtDataTreeView.h"
#include "QtnPropertyWidgetX.h"
#include "kddockwidgets/Config.h"
#include "KcDataSnapshot.h"
#include "KcDockPlotter.h"
#include "QtAppEventHub.h"
#include "KcAudioInputStream.h"


using namespace KDDockWidgets;

QtDataVisFrame::QtDataVisFrame()
    : MainWindow("DataVis"/*, MainWindowOption_HasCentralFrame*/)
{
    initLauout_();
}


QtDataVisFrame::~QtDataVisFrame()
{
    delete dockDataSource_;
    delete dockDataProp_;
    delete dockPlotProp_;
}


bool QtDataVisFrame::setupMenu_()
{
    auto menubar = menuBar();

    /// 准备<文件>菜单
    auto fileMenu = new QMenu(u8"载入(&F)", this);
    menubar->addMenu(fileMenu);

    QAction* dataFile = fileMenu->addAction(u8"数据文件(&D)...");
    connect(dataFile, &QAction::triggered, this, &QtDataVisFrame::openDataFile);

    QAction* audioFile = fileMenu->addAction(u8"音频文件(&A)...");
    connect(audioFile, &QAction::triggered, this, &QtDataVisFrame::openAudioFile);

    fileMenu->addSeparator();


    QAction* formula = fileMenu->addAction(u8"数学表达式(&E)...");
    connect(formula, &QAction::triggered, this, &QtDataVisFrame::openFormula);

    fileMenu->addSeparator();


    QAction* audioCapture = fileMenu->addAction(u8"录制音频(&C)...");
    connect(audioCapture, &QAction::triggered, this, &QtDataVisFrame::openAudioCapture);

    QAction* audioDevice = fileMenu->addAction(tr(u8"录音设备(&I)"));
    connect(audioDevice, &QAction::triggered, this, &QtDataVisFrame::openAudioInput);

    fileMenu->addSeparator();


    QAction* quit = fileMenu->addAction(u8"退出(&X)");
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);


    /// 准备<视图>菜单
    auto viewMenu = new QMenu(u8"视图(&F)", this);
    menubar->addMenu(viewMenu);
    viewMenu->addAction(dockDataSource_->toggleAction());
    viewMenu->addAction(dockDataProp_->toggleAction());
    viewMenu->addAction(dockPlotProp_->toggleAction());

    viewMenu->addSeparator();


    auto layoutEqually = viewMenu->addAction(u8"平均布局(&E)");
    connect(layoutEqually, &QAction::triggered, this, &MainWindow::layoutEqually);

    auto indicatorSupport = viewMenu->addAction(u8"停靠指示(&D)");
    indicatorSupport->setCheckable(true);
    indicatorSupport->setChecked(true);
    connect(indicatorSupport, &QAction::toggled, this, [](bool b) {
        KDDockWidgets::Config::self().setDropIndicatorsInhibited(!b);
        });

    viewMenu->addSeparator();

    auto closeAll = viewMenu->addAction(u8"关闭全部(&A)");
    connect(closeAll, &QAction::triggered, this, [this] { closeDockWidgets(true); });

    return true;
}


bool QtDataVisFrame::initLauout_()
{
    setAffinities({ "source-dock", "prop-dock", "plot-dock" });

    auto dataTreeView = new QtDataTreeView(this);
    dockDataSource_ = new DockWidget{ u8"输入源" };
    dockDataSource_->setAffinities({ "source-dock" });
    dockDataSource_->setWidget(dataTreeView);
    addDockWidget(dockDataSource_, Location_OnLeft);
    dockDataSource_->show(); // TODO: 如何控制close按钮的行为，关闭还是隐藏？

    // 用户选择不同的显示类型触发：同步绘图属性视图
    connect(dataTreeView, &QtDataTreeView::outputTypeChanged, this, [this](KcDockPlotter* plot) {
        if (dockPlotProp_->isVisible()) {
            QtnPropertyWidgetX* view = dynamic_cast<QtnPropertyWidgetX*>(dockPlotProp_->widget());
            view->sync(plot);
        }
        });

    // 当前tree-item发生变化触发：同步数据属性和绘图属性视图
    connect(dataTreeView, &QtDataTreeView::currentItemChanged, this,
        [this, dataTreeView](QTreeWidgetItem* current, QTreeWidgetItem* previous) {

        if (dockPlotProp_->isVisible()) {
            QtnPropertyWidgetX* view = dynamic_cast<QtnPropertyWidgetX*>(dockPlotProp_->widget());
            view->sync(dataTreeView->getAttachedPlot(current));
        }

        if (dockDataProp_->isVisible()) {
            QtnPropertyWidgetX* view = dynamic_cast<QtnPropertyWidgetX*>(dockDataProp_->widget());
            view->sync(dataTreeView->getAttachedSource(current));
        }
        });


    // 用户双击tree-item触发：弹出显示窗口
    connect(dataTreeView, &QtDataTreeView::itemDoubleClicked, this, [dataTreeView](QTreeWidgetItem* item, int) {
        auto plot = dataTreeView->getAttachedPlot(item);
        if (plot && plot->type() >= 0) plot->show();
        });



    auto dataPropView = new QtnPropertyWidgetX(this);
    dockDataProp_ = new DockWidget{ u8"属性" };
    dockDataProp_->setAffinities({ "prop-dock" });
    dockDataProp_->setWidget(dataPropView);
    addDockWidget(dockDataProp_, Location_OnRight);
    dockDataProp_->show();
    connect(dataPropView, &QtnPropertyWidgetX::propertyChanged, this, [dataTreeView](int id, const QVariant& val) {
        auto source = dataTreeView->getAttachedSource(dataTreeView->currentItem());
        if(source) source->onPropertyChanged(id, val);
        // TODO: 视情同步更新plotPropView
        });


    auto plotPropView = new QtnPropertyWidgetX(this);
    dockPlotProp_ = new DockWidget{ u8"绘图" };
    dockPlotProp_->setAffinities({ "prop-dock" });
    dockPlotProp_->setWidget(plotPropView);
    addDockWidget(dockPlotProp_, Location_OnBottom, dockDataProp_);
    //dockDataProp_->addDockWidgetAsTab(dockPlotProp_);
    dockPlotProp_->show();
    connect(plotPropView, &QtnPropertyWidgetX::propertyChanged, this, [dataTreeView](int id, const QVariant& val) {
        auto plot = dataTreeView->getAttachedPlot(dataTreeView->currentItem());
        if(plot) plot->onPropertyChanged(id, val);
        });

    return setupMenu_();
}


void QtDataVisFrame::openDataFile()
{
    auto path = QFileDialog::getOpenFileName(this);

    if (!path.isEmpty()) {
        auto data = loadData_(path);
        if (data) {
            auto fi = QFileInfo(path);
            auto ds = new KcDataSnapshot(fi.fileName(), data, KcDataSnapshot::k_sampled);
            newInputSource_(ds);
        }
    }
}


void QtDataVisFrame::openAudioFile()
{
    auto path = QtAudioUtils::getOpenPath();
    if (path.isEmpty()) return;

    auto audio = std::make_shared<KcAudio>();
    auto r = audio->load(path.toLocal8Bit().constData()); // TODO: ???
    if (!r.empty()) {
        QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(r)); // TODO: 
        return;
    }

    auto fi = QFileInfo(path);
    auto ds = new KcDataSnapshot(fi.fileName(), audio, KcDataSnapshot::k_sampled);
    newInputSource_(ds);
}


void QtDataVisFrame::openAudioCapture()
{
    KcAudioCaptureDlg dlg;
    dlg.setEmbeddingMode(true);
    if (dlg.exec() == QDialog::Accepted) {
        auto ds = new KcDataSnapshot(u8"录音片段", dlg.audio_, KcDataSnapshot::k_sampled);
        newInputSource_(ds);
    }
}


void QtDataVisFrame::openAudioInput()
{
    newInputSource_(new KcAudioInputStream);
}


void QtDataVisFrame::openFormula()
{
    KcFormulaDlg dlg;
    if (dlg.exec() == QDialog::Accepted) {
        auto ds = new KcDataSnapshot(dlg.exprText(), dlg.data, KcDataSnapshot::k_continued);
        newInputSource_(ds);
    }
}


std::shared_ptr<KvData> QtDataVisFrame::loadData_(const QString& filePath)
{
    std::shared_ptr<KvData> data; // the result to returned

    std::ifstream ifs(filePath.toStdString().c_str());
    if (!ifs.is_open()) {
        QMessageBox::information(this, u8"错误", u8"无法打开数据文件！");
        return nullptr;
    }

    // load data from ifs
    std::string line;

    // parse the first line
    std::getline(ifs, line);
    auto tokens = KuStrUtil::split(line, "\t ,");
    auto cols = tokens.size();
    if (cols == 0 || cols > 2) {
        QMessageBox::information(this, u8"错误", u8"不支持的数据文件格式！");
        return nullptr;
    }

    if (cols == 1) {
        std::vector<kReal> v;
        do {
            v.push_back(std::stod(tokens[0]));
        } while (std::getline(ifs, line) && (tokens = KuStrUtil::split(line, "\t ,")).size() == 1);

        data = std::make_shared<KcSampled1d>(std::move(v));
    }
    else {
        assert(false); // TODO:
    }


    return data;
}


void QtDataVisFrame::newInputSource_(KvInputSource* is)
{
    // 创建与is关联的output
    auto plot = new KcDockPlotter(is, this);
    auto dataTreeView = dynamic_cast<QtDataTreeView*>(dockDataSource_->widget());
    assert(dataTreeView);

    dataTreeView->insertItem(is, plot);
}


void QtDataVisFrame::connectAppEvents_()
{

}

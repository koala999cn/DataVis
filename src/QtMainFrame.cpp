#include <fstream>
#include <string>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include "QtMainFrame.h"
#include "KcFormulaDlg.h"
#include "KcAudioCaptureDlg.h"
#include "base/KuStrUtil.h"
#include "dsp/KvData1d.h"
#include "dsp/KcFormulaData1d.h"
#include "dsp/KcSampled1d.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioRender.h"
#include "gui/QtAudioUtils.h"
#include "QtWorkspaceWidget.h"
#include "QtnPropertyWidgetX.h"
#include "kddockwidgets/Config.h"
#include "KcDataSnapshot.h"
#include "KcPlot1d.h"
#include "KcPlot2d.h"
#include "QtAppEventHub.h"
#include "KcAudioInputStream.h"
#include "op/KcFftOp.h"
#include "op/KcHistoOp.h"
#include "op/KcFramingOp.h"


using namespace KDDockWidgets;

namespace kPrivate
{
    template<typename T, typename...ARGS>
    T* insertObject(DockWidget* workDock, bool asRootChild, ARGS...args)
    {
        auto tree = dynamic_cast<QtWorkspaceWidget*>(workDock->widget());
        auto obj = new T(args...);
        tree->insertObject(obj, asRootChild);

        return obj;
    }

    // 带parent的构建
    template<typename T, typename...ARGS>
    T* insertObjectP(DockWidget* workDock, bool asRootChild, ARGS...args)
    {
        auto tree = dynamic_cast<QtWorkspaceWidget*>(workDock->widget());
        auto parent = asRootChild ? tree->rootObject() : tree->currentObject();
        return insertObject<T>(workDock, asRootChild,
            dynamic_cast<KvDataProvider*>(parent),
            args...);
    }
}


QtMainFrame::QtMainFrame()
    : MainWindow("DataVis"/*, MainWindowOption_HasCentralFrame*/)
{
    initLauout_();
    setupMenu_(); // 须在initLauout_之后调用
}


QtMainFrame::~QtMainFrame()
{
    delete workDock_;
    delete propDock_;
}


bool QtMainFrame::setupMenu_()
{
    auto menubar = menuBar();

    /// prepare <Source> menu
    auto fileMenu = new QMenu(u8"Source(&S)", this);
    menubar->addMenu(fileMenu);

    QAction* dataFile = fileMenu->addAction(u8"Data File(&D)...");
    connect(dataFile, &QAction::triggered, this, &QtMainFrame::openDataFile);

    QAction* audioFile = fileMenu->addAction(u8"Audio File(&A)...");
    connect(audioFile, &QAction::triggered, this, &QtMainFrame::openAudioFile);

    fileMenu->addSeparator();


    QAction* formula = fileMenu->addAction(u8"Math Expression(&E)...");
    connect(formula, &QAction::triggered, this, &QtMainFrame::openFormula);

    fileMenu->addSeparator();


    QAction* audioCapture = fileMenu->addAction(u8"Audio Capture(&C)...");
    connect(audioCapture, &QAction::triggered, this, &QtMainFrame::openAudioCapture);

    QAction* audioDevice = fileMenu->addAction(tr(u8"Audio Input Device(&I)"));
    connect(audioDevice, &QAction::triggered, this, &QtMainFrame::openAudioInput);

    fileMenu->addSeparator();


    QAction* quit = fileMenu->addAction(u8"Exit(&X)");
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);


    /// prepare <View> menu
    auto viewMenu = new QMenu(u8"View(&V)", this);
    menubar->addMenu(viewMenu);
    viewMenu->addAction(workDock_->toggleAction());
    viewMenu->addAction(propDock_->toggleAction());

    viewMenu->addSeparator();


    auto layoutEqually = viewMenu->addAction(u8"Layout Equally(&E)");
    connect(layoutEqually, &QAction::triggered, this, &MainWindow::layoutEqually);

    auto indicatorSupport = viewMenu->addAction(u8"Drop Indicators(&D)");
    indicatorSupport->setCheckable(true);
    indicatorSupport->setChecked(true);
    connect(indicatorSupport, &QAction::toggled, [](bool b) {
        KDDockWidgets::Config::self().setDropIndicatorsInhibited(!b);
        });

    viewMenu->addSeparator();

    auto closeAll = viewMenu->addAction(u8"Close All(&A)"); // TODO: stop rendering
    connect(closeAll, &QAction::triggered, [this] { closeDockWidgets(true); });


    /// prepare <Operator> menu
    auto opMenu = new QMenu(u8"Operator(&O)", this);
    menubar->addMenu(opMenu);

    QAction* fft = opMenu->addAction(u8"Fft(&F)");
    connect(fft, &QAction::triggered, [this] { kPrivate::insertObjectP<KcFftOp>(workDock_, false); });

    QAction* hist = opMenu->addAction(u8"Histo(&H)");
    connect(hist, &QAction::triggered, [this] { kPrivate::insertObjectP<KcHistoOp>(workDock_, false); });

    QAction* framing = opMenu->addAction(u8"Framing(&M)");
    connect(framing, &QAction::triggered, [this] { kPrivate::insertObjectP<KcFramingOp>(workDock_, false); });


    connect(opMenu, &QMenu::aboutToShow, [=] {
        auto treeView = dynamic_cast<QtWorkspaceWidget*>(workDock_->widget());
        auto obj = dynamic_cast<KvDataProvider*>(treeView->currentObject());
        fft->setEnabled(obj && obj->step(obj->dim() - 1) != KvData::k_nonuniform_step);
        hist->setEnabled(obj && obj->dim() == 1);
        framing->setEnabled(obj && obj->dim() == 1);
        });


    /// prepare <Render> menu
    auto renderMenu = new QMenu(u8"Render(&R)", this);
    menubar->addMenu(renderMenu);

    QAction* scatter = renderMenu->addAction(u8"Scatter Plot(&P)");
    connect(scatter, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcPlot1d>(workDock_, false, KcPlot1d::KeType::k_scatter);
        });

    QAction* line = renderMenu->addAction(u8"Line Plot(&L)");
    connect(line, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcPlot1d>(workDock_, false, KcPlot1d::KeType::k_line);
        });

    QAction* bar = renderMenu->addAction(u8"Bars Plot(&B)");
    connect(bar, &QAction::triggered, [this] { 
        kPrivate::insertObjectP<KcPlot1d>(workDock_, false, KcPlot1d::KeType::k_bars);
        });

    QAction* color_map = renderMenu->addAction(u8"Color Map(&M)");
    connect(color_map, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcPlot2d>(workDock_, false);
        });

    connect(renderMenu, &QMenu::aboutToShow, [=] {
        auto treeView = dynamic_cast<QtWorkspaceWidget*>(workDock_->widget());
        auto obj = dynamic_cast<KvDataProvider*>(treeView->currentObject());
        scatter->setEnabled(obj && obj->dim() == 1);
        line->setEnabled(obj && obj->dim() == 1);
        bar->setEnabled(obj && obj->dim() == 1);
        color_map->setEnabled(obj && obj->dim() == 2);
        });

    return true;
}


bool QtMainFrame::initLauout_()
{
    setAffinities({ "workspace", "property", "render" });

    auto workWidget = new QtWorkspaceWidget(this);
    workDock_ = new DockWidget{ u8"Workspace" };
    workDock_->setAffinities({ "workspace" });
    workDock_->setWidget(workWidget);
    addDockWidget(workDock_, Location_OnLeft);
    workDock_->show();


    auto propWidget = new QtnPropertyWidgetX(this);
    propDock_ = new DockWidget{ u8"Property" };
    propDock_->setAffinities({ "property" });
    propDock_->setWidget(propWidget);
    addDockWidget(propDock_, Location_OnRight);
    propDock_->show();

    // 处理用户编辑而导致的对象属性变化
    connect(propWidget, &QtnPropertyWidgetX::propertyChanged, this, 
        [workWidget](int id, const QVariant& val) {
            auto obj = workWidget->getObject(workWidget->currentItem());
            if(obj) obj->setProperty(id, val);
            });

    propWidget->connect(kAppEventHub, &QtAppEventHub::objectActivated, 
          propWidget, &QtnPropertyWidgetX::sync);

    // 处理对象自行发起的属性变化信号，通常用于同步属性页的显示
    this->connect(kAppEventHub, &QtAppEventHub::objectPropertyChanged, this, 
        [propWidget](KvPropertiedObject* obj, int propId, const QVariant& newVal) {
            if (propWidget->currentObject() == obj) {
                propWidget->setValue(propId, newVal);
            }
        });

    return true;
}


void QtMainFrame::openDataFile()
{
    auto path = QFileDialog::getOpenFileName(this);

    if (!path.isEmpty()) {
        auto data = loadData_(path);
        if (data) 
            kPrivate::insertObject<KcDataSnapshot>(workDock_, true, 
                QFileInfo(path).fileName(), data, KcDataSnapshot::k_sampled);
    }
}


void QtMainFrame::openAudioFile()
{
    auto path = QtAudioUtils::getOpenPath();
    if (path.isEmpty()) return;

    auto audio = std::make_shared<KcAudio>();
    auto r = audio->load(path.toLocal8Bit().constData()); // TODO: ???
    if (!r.empty()) {
        QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(r)); // TODO: 
        return;
    }

    kPrivate::insertObject<KcDataSnapshot>(workDock_, true,
        QFileInfo(path).fileName(), audio, KcDataSnapshot::k_sampled);
}


void QtMainFrame::openAudioCapture()
{
    KcAudioCaptureDlg dlg;
    dlg.setEmbeddingMode(true);
    if (dlg.exec() == QDialog::Accepted) 
        kPrivate::insertObject<KcDataSnapshot>(workDock_, true,
            tr("audio slice"), dlg.audio_, KcDataSnapshot::k_sampled);
}


void QtMainFrame::openAudioInput()
{
    kPrivate::insertObject<KcAudioInputStream>(workDock_, true);
}


void QtMainFrame::openFormula()
{
    KcFormulaDlg dlg;
    if (dlg.exec() == QDialog::Accepted) 
        kPrivate::insertObject<KcDataSnapshot>(workDock_, true,
            dlg.exprText(), dlg.data, KcDataSnapshot::k_continued);
}


std::shared_ptr<KvData> QtMainFrame::loadData_(const QString& filePath)
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


void QtMainFrame::connectAppEvents_()
{

}

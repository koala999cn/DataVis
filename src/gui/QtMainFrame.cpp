#include <fstream>
#include <string>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include "QtMainFrame.h"
#include "QtFormulaDlg.h"
#include "QtAudioCaptureDlg.h"
#include "base/KuStrUtil.h"
#include "dsp/KcSampled1d.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioRender.h"
#include "QtAudioUtils.h"
#include "QtWorkspaceWidget.h"
#include "QtnPropertyWidgetX.h"
#include "kddockwidgets/Config.h"
#include "prov/KcPvData.h"
#include "prov/KcPvAudioInput.h"
#include "prov/KcPvExcitationSource.h"
#include "render/KcRdPlot1d.h"
#include "render/KcRdPlot2d.h"
#include "render/KcRdBar3d.h"
#include "render/KcRdAudioPlayer.h"
#include "op/KcOpSpectrum.h"
#include "op/KcOpHist.h"
#include "op/KcOpHistC.h"
#include "op/KcOpFraming.h"
#include "op/KcOpWindowing.h"
#include "op/KcOpFilterBank.h"
#include "op/KcOpSampler.h"
#include "op/KcOpInterpolater.h"
#include "op/KcOpFIR.h"
#include "op/KcOpResampler.h"
#include "QtAppEventHub.h"


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

    auto excitor = fileMenu->addMenu(tr("Exctation Source(&S)"));
    auto stochastic = excitor->addAction(tr("Stochastic(S)")); // 随机过程
    connect(stochastic, &QAction::triggered, this, &QtMainFrame::openStochastic);

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

    QAction* fft = opMenu->addAction(u8"Spectrum(&S)");
    connect(fft, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpSpectrum>(workDock_, false); });

    QAction* hist = opMenu->addAction(u8"Histogram(&H)");
    connect(hist, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpHist>(workDock_, false); });

    QAction* histc = opMenu->addAction(u8"Hist Counter(&C)");
    connect(histc, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpHistC>(workDock_, false); });

    QAction* framing = opMenu->addAction(u8"Framing(&M)");
    connect(framing, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpFraming>(workDock_, false); });

    QAction* windowing = opMenu->addAction(u8"Windowing(&W)");
    connect(windowing, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpWindowing>(workDock_, false); });

    QAction* fbank = opMenu->addAction(u8"FBank(&B)");
    connect(fbank, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpFilterBank>(workDock_, false); });

    QAction* sampler = opMenu->addAction(u8"Sampler(&P)");
    connect(sampler, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpSampler>(workDock_, false); });

    QAction* interp = opMenu->addAction(u8"Interpolater(&I)");
    connect(interp, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpInterpolater>(workDock_, false); });

    QAction* fir = opMenu->addAction(u8"FIR(&F)");
    connect(fir, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpFIR>(workDock_, false); });

    QAction* resampler = opMenu->addAction(u8"Resampler(&R)");
    connect(resampler, &QAction::triggered, [this] { kPrivate::insertObjectP<KcOpResampler>(workDock_, false); });

    QAction* player = opMenu->addAction(u8"AudioPlayer(&P)");
    connect(player, &QAction::triggered, [this] { kPrivate::insertObjectP<KcRdAudioPlayer>(workDock_, false); });

    
    connect(opMenu, &QMenu::aboutToShow, [=] {
        auto treeView = dynamic_cast<QtWorkspaceWidget*>(workDock_->widget());
        auto obj = dynamic_cast<KvDataProvider*>(treeView->currentObject());
        fft->setEnabled(obj && obj->isSampled());
        hist->setEnabled(obj && obj->dim() == 1 && obj->isDiscreted());
        histc->setEnabled(obj&& obj->dim() == 1 && obj->isDiscreted());
        framing->setEnabled(obj && obj->isSampled());
        windowing->setEnabled(obj && obj->isSampled());
        fbank->setEnabled(obj && obj->isSampled());
        sampler->setEnabled(obj && obj->isContinued());
        interp->setEnabled(obj && obj->dim() == 1 && obj->isDiscreted());
        fir->setEnabled(obj && obj->dim() == 1 && obj->isSampled());
        resampler->setEnabled(obj&& obj->dim() == 1 && obj->isSampled());
        player->setEnabled(obj && obj->dim() == 1 && obj->isSampled());
        });


    /// prepare <Render> menu
    auto renderMenu = new QMenu(u8"Render(&R)", this);
    menubar->addMenu(renderMenu);

    QAction* scatter = renderMenu->addAction(u8"Scatter Plot(&P)");
    connect(scatter, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdPlot1d>(workDock_, false, KcRdPlot1d::KeType::k_scatter);
        });

    QAction* line = renderMenu->addAction(u8"Line Plot(&L)");
    connect(line, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdPlot1d>(workDock_, false, KcRdPlot1d::KeType::k_line);
        });

    QAction* bar = renderMenu->addAction(u8"Bars Plot(&B)");
    connect(bar, &QAction::triggered, [this] { 
        kPrivate::insertObjectP<KcRdPlot1d>(workDock_, false, KcRdPlot1d::KeType::k_bars);
        });

    QAction* color_map = renderMenu->addAction(u8"Color Map(&M)");
    connect(color_map, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdPlot2d>(workDock_, false);
        });

    QAction* bars3d = renderMenu->addAction(u8"Bars3d(&D)");
    connect(bars3d, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdBar3d>(workDock_, false);
        });

    connect(renderMenu, &QMenu::aboutToShow, [=] {
        auto treeView = dynamic_cast<QtWorkspaceWidget*>(workDock_->widget());
        auto obj = dynamic_cast<KvDataProvider*>(treeView->currentObject());
        scatter->setEnabled(obj && obj->dim() == 1);
        line->setEnabled(obj && obj->dim() == 1);
        bar->setEnabled(obj && obj->dim() == 1);
        color_map->setEnabled(obj && obj->dim() == 2);
        bars3d->setEnabled(obj && obj->isDiscreted() && obj->dim() <= 2);
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
    connect(propWidget, &QtnPropertyWidgetX::propertyChanged,  
        [workWidget](int id, const QVariant& val) {
            auto obj = workWidget->getObject(workWidget->currentItem());
            if(obj) obj->setProperty(id, val);
            });

    // 处理对象自行发起的属性变化信号，通常用于同步属性页的显示
    connect(kAppEventHub, &QtAppEventHub::objectPropertyChanged,  
        [propWidget](KvPropertiedObject* obj, int propId, const QVariant& newVal) {
            if (propWidget->currentObject() == obj) {
                propWidget->setValue(propId, newVal);
            }
        });

    propWidget->connect(kAppEventHub, &QtAppEventHub::objectActivated,
        propWidget, [propWidget](KvPropertiedObject* obj) {
            if (propWidget->currentObject() != obj) {
                propWidget->sync(obj);
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
            kPrivate::insertObject<KcPvData>(workDock_, true, QFileInfo(path).fileName(), data);
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

    kPrivate::insertObject<KcPvData>(workDock_, true, QFileInfo(path).fileName(), audio);
}


void QtMainFrame::openAudioCapture()
{
    QtAudioCaptureDlg dlg;
    dlg.setEmbeddingMode(true);
    if (dlg.exec() == QDialog::Accepted) 
        kPrivate::insertObject<KcPvData>(workDock_, true, tr("audio slice"), dlg.audio_);
}


void QtMainFrame::openAudioInput()
{
    kPrivate::insertObject<KcPvAudioInput>(workDock_, true);
}


void QtMainFrame::openFormula()
{
    QtFormulaDlg dlg;
    if (dlg.exec() == QDialog::Accepted) 
        kPrivate::insertObject<KcPvData>(workDock_, true, dlg.exprText(), dlg.data);
}


void QtMainFrame::openStochastic()
{
    kPrivate::insertObject<KcPvExcitationSource>(workDock_, true);
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

        auto data1d = std::make_shared<KcSampled1d>();
        data1d->resize(v.size(), 1);
        data1d->setChannel(nullptr, 0, v.data()); // TODO: 优化
        data = data1d;
    }
    else {
        assert(false); // TODO:
    }


    return data;
}


void QtMainFrame::connectAppEvents_()
{

}

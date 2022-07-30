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
#include "KcSampled1d.h"
#include "KtScattered.h"
#include "audio/KcAudio.h"
#include "QtAudioUtils.h"
#include "QtWorkspaceWidget.h"
#include "QtnPropertyWidgetX.h"
#include "QtTxtDataLoadDlg.h"
#include "kddockwidgets/Config.h"
#include "QtAppEventHub.h"
#include "provider.h"
#include "operator.h"
#include "render.h"


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
    menubar->addMenu(setupPvMenu_());

    /// prepare <View> menu
    menubar->addMenu(setupVwMenu_());

    /// prepare <Operator> menu
    menubar->addMenu(setupOpMenu_());

    /// prepare <Render> menu
    menubar->addMenu(setupRdMenu_());

    return true;
}


QMenu* QtMainFrame::setupPvMenu_()
{
    auto fileMenu = std::make_unique<QMenu>(u8"Source(&S)", this);

    QAction* txtData = fileMenu->addAction(u8"Text Data(&T)...");
    connect(txtData, &QAction::triggered, this, &QtMainFrame::openTxtData);

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

    return fileMenu.release();
}


QMenu* QtMainFrame::setupVwMenu_()
{
    auto viewMenu = std::make_unique<QMenu>(u8"View(&V)", this);
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

    return viewMenu.release();
}


QMenu* QtMainFrame::setupOpMenu_()
{
    auto opMenu = std::make_unique<QMenu>(u8"Operator(&O)", this);

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


    connect(opMenu.get(), &QMenu::aboutToShow, [=] {
        auto treeView = dynamic_cast<QtWorkspaceWidget*>(workDock_->widget());
        auto obj = dynamic_cast<KvDataProvider*>(treeView->currentObject());
        fft->setEnabled(obj && obj->isSampled());
        hist->setEnabled(obj && obj->dim() == 1 && obj->isDiscreted());
        histc->setEnabled(obj && obj->dim() == 1 && obj->isDiscreted());
        framing->setEnabled(obj && obj->isSampled());
        windowing->setEnabled(obj && obj->isSampled());
        fbank->setEnabled(obj && obj->isSampled());
        sampler->setEnabled(obj && obj->isContinued());
        interp->setEnabled(obj && obj->dim() == 1 && obj->isDiscreted());
        fir->setEnabled(obj && obj->dim() == 1 && obj->isSampled());
        resampler->setEnabled(obj && obj->dim() == 1 && obj->isSampled());
        });

    return opMenu.release();
}


QMenu* QtMainFrame::setupRdMenu_()
{
    auto renderMenu = std::make_unique<QMenu>(u8"Render(&R)", this);

    QAction* plot1d = renderMenu->addAction(u8"Plot1d(&L)");
    connect(plot1d, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdPlot1d>(workDock_, false);
        });

    QAction* color_map = renderMenu->addAction(u8"Color Map(&M)");
    connect(color_map, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdPlot2d>(workDock_, false);
        });

    QAction* bars3d = renderMenu->addAction(u8"Bars3d(&D)");
    connect(bars3d, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdBar3d>(workDock_, false);
        });

    QAction* scatter3d = renderMenu->addAction(u8"Scatter3d(&S)");
    connect(scatter3d, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdScatter3d>(workDock_, false);
        });

    QAction* surface3d = renderMenu->addAction(u8"Surface3d(&S)");
    connect(surface3d, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdSurface3d>(workDock_, false);
        });

    QAction* player = renderMenu->addAction(u8"AudioPlayer(&P)");
    connect(player, &QAction::triggered, [this] {
        kPrivate::insertObjectP<KcRdAudioPlayer>(workDock_, false);
        });

    connect(renderMenu.get(), &QMenu::aboutToShow, [=] {
        auto treeView = dynamic_cast<QtWorkspaceWidget*>(workDock_->widget());
        auto obj = dynamic_cast<KvDataProvider*>(treeView->currentObject());
        plot1d->setEnabled(obj && obj->dim() == 1);
        color_map->setEnabled(obj && obj->dim() == 2);
        bars3d->setEnabled(obj && obj->isSampled() && obj->dim() <= 2);
        scatter3d->setEnabled(obj && obj->isDiscreted() && obj->dim() <= 2);
        surface3d->setEnabled(obj && obj->dim() == 2); // 允许绘制连续曲面
        player->setEnabled(obj && obj->dim() == 1 && obj->isSampled());
        });

    return renderMenu.release();
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


void QtMainFrame::openTxtData()
{
    auto path = QFileDialog::getOpenFileName(this);

    if (!path.isEmpty()) {
        QtTxtDataLoadDlg dlg(path, this);
        if (dlg.exec() == QDialog::Accepted)
            kPrivate::insertObject<KcPvData>(workDock_, true, QFileInfo(path).fileName(), dlg.data);
    }
}


void QtMainFrame::openAudioFile()
{
    auto path = QtAudioUtils::getOpenPath(this);
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


void QtMainFrame::connectAppEvents_()
{

}

#include "gui/QtMainFrame.h"
#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include "kddockwidgets/Config.h"
#include "QtAppEventHub.h"
#include "QtnProperty/Delegates/PropertyDelegate.h"
#include "plot/KsPlotManager.h"


int main(int argc, char *argv[])
{
    QtnPropertyDelegate::setBranchIndicatorStyle(QtnPropertyDelegate::QtnBranchIndicatorStyleNative);

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("fusion")));
    //app.setStyle(new DarkStyle);
    //KDDockWidgets::Config::self().setFlags(KDDockWidgets::Config::Flag_AutoHideSupport);
    KDDockWidgets::Config::self().setSeparatorThickness(3);

    // 打开Flag_NativeTitleBar，否则浮动QOpenGLWidget时，标题栏渲染位置有问题
    KDDockWidgets::Config::self().setFlags(KDDockWidgets::Config::Flag_NativeTitleBar);
    
    KsPlotManager::singleton().init();

    auto mainWindow = new QtMainFrame;
    mainWindow->setWindowTitle(u8"DataVis");
    mainWindow->setWindowIcon(app.style()->standardIcon(QStyle::SP_DesktopIcon));
    mainWindow->showMaximized();

    app.connect(&app, &QApplication::aboutToQuit, [mainWindow]() {
        mainWindow->hide();
        mainWindow->close();
        delete mainWindow;
        singletonAppEventHub::destroy();
        KsPlotManager::singleton().shutdown();
    });

    app.activeWindow();
    return app.exec();
}

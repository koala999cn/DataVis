#include "gui/QtMainFrame.h"
#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include "kddockwidgets/Config.h"
#include "QtAppEventHub.h"
#include "QtnProperty/Delegates/PropertyDelegate.h"


int main(int argc, char *argv[])
{
    QtnPropertyDelegate::setBranchIndicatorStyle(QtnPropertyDelegate::QtnBranchIndicatorStyleNative);

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("fusion")));
    //app.setStyle(new DarkStyle);
    //KDDockWidgets::Config::self().setFlags(KDDockWidgets::Config::Flag_AutoHideSupport);
    KDDockWidgets::Config::self().setSeparatorThickness(3);

    auto mainWindow = new QtMainFrame;
    mainWindow->setWindowTitle(u8"DataVis");
    mainWindow->setWindowIcon(app.style()->standardIcon(QStyle::SP_DesktopIcon));
    mainWindow->showMaximized();

    app.connect(&app, &QApplication::aboutToQuit, kAppEventHub, &singletonAppEventHub::destroy);

    app.activeWindow();
    return app.exec();
}

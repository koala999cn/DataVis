#include "QtDataVisFrame.h"
#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include "kddockwidgets/Config.h"
#include "QtAppEventHub.h"
#include "QtnProperty/Delegates/PropertyDelegate.h"
#include "QtnPropertyDelegatePropertySetX.h"
#include "QtnProperty/Delegates/PropertyDelegateFactory.h"
#include "QtnProperty/GUI/PropertyQPen.h"


int main(int argc, char *argv[])
{
    auto& factory = QtnPropertyDelegateFactory::staticInstance();
    //factory.unregisterDelegate(&QtnPropertyQPenBase::staticMetaObject);
    //QtnPropertyDelegateQPenX::Register(factory);
    QtnPropertyDelegate::setBranchIndicatorStyle(QtnPropertyDelegate::QtnBranchIndicatorStyleNative);

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("fusion")));
    //app.setStyle(new DarkStyle);
    //KDDockWidgets::Config::self().setFlags(KDDockWidgets::Config::Flag_AutoHideSupport);
    KDDockWidgets::Config::self().setSeparatorThickness(3);

    QtDataVisFrame* mainWindow = new QtDataVisFrame;
    mainWindow->setWindowTitle("DataVis");
    mainWindow->setWindowIcon(app.style()->standardIcon(QStyle::SP_DesktopIcon));
    mainWindow->showMaximized();

    app.connect(&app, &QApplication::aboutToQuit, kAppEventHub, &singletonAppEventHub::destroy);

    return app.exec();
}

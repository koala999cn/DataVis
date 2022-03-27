#include "QtAppEventHub.h"
#include <QApplication>
#include "kddockwidgets/MainWindow.h"
#include "kddockwidgets/DockWidget.h"
#include "KvPropertiedObject.h"


namespace kPrivate
{
	static auto getDockName(KvPropertiedObject* obj)
	{
		return QString::number(reinterpret_cast<qlonglong>(obj), 16);
	}

	static auto getDockTitle(KvPropertiedObject* obj)
	{
		return obj->name();
	}

	static auto getDock(KvPropertiedObject* obj, bool createIfNonexist)
	{
		using namespace KDDockWidgets;

		DockWidgetBase* dock = nullptr;

		auto topLevelWidgets = QApplication::topLevelWidgets();
		for (QWidget* widget : topLevelWidgets) {
			auto dockFrame = dynamic_cast<MainWindow*>(widget);

			if (dockFrame) {
				auto dockName = getDockName(obj);
				dock = DockWidget::byName(dockName);

				if (dock == nullptr && createIfNonexist) {
					dock = new DockWidget(dockName, DockWidget::Option_DeleteOnClose);
					dock->setTitle(kPrivate::getDockTitle(obj));
					dock->setAffinities({ "render" });

					dockFrame->addDockWidget(dock, Location_None);
				}

				break;
			}
		}

		return dock;
	}
}


void QtAppEventHub::slotShowInDock(KvPropertiedObject* obj, QWidget* widget)
{
	using namespace KDDockWidgets;

	auto dock = kPrivate::getDock(obj, true);

	if (dock->widget() == nullptr) {
		dock->setWidget(widget);
		widget->setVisible(true);

		connect(dock, &DockWidget::isFocusedChanged, [=](bool focus) {
			if(focus) emit objectActivated(obj);
			});

		connect(dock, &DockWidget::aboutToDeleteOnClose, [=]() {
			dock->setWidget(nullptr); // 不释放关联的widget
			widget->setVisible(false);
			emit dockClosed(obj);
			});

		connect(obj, &QObject::objectNameChanged, dock, &DockWidget::setTitle);

		connect(obj, &QObject::destroyed, [=]() { slotCloseDock(obj); });
	}
	else {
		assert(dock->widget() == widget);
		assert(dock->title() == kPrivate::getDockTitle(obj));
	}
	
	dock->show();
	dock->raise();
}


void QtAppEventHub::slotCloseDock(KvPropertiedObject* obj)
{
	auto dock = kPrivate::getDock(obj, false);
	if (dock) {
		auto widget = dock->widget();
		if (widget) {
			dock->setWidget(nullptr); // 不释放关联的widget
			widget->setVisible(false);
		}

		delete dock;
		assert(kPrivate::getDock(obj, false) == nullptr);

		emit dockClosed(obj);
	}
}

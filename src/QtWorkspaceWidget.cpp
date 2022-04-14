#include "QtWorkspaceWidget.h"
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QApplication>
#include <QScreen>
#include <assert.h>
#include "QtAppEventHub.h"
#include "KvPropertiedObject.h"
#include "KvDataProvider.h"
#include "KvDataRender.h"
#include "op/KvDataOperator.h"
#include "KvData.h"


QtWorkspaceWidget::QtWorkspaceWidget(QWidget* parent)
	: QTreeWidget(parent)
{
	setFrameStyle(QFrame::NoFrame);
	setHeaderHidden(true);
	setColumnCount(1);
	//setHeaderLabels( { tr("name"), tr("type") } );
	root_ = new QTreeWidgetItem(this, { u8"Pipeline" });
	//auto comboBoxDelegate = new QtComboBoxDelegate(this);
	//QStringList plotTypes = KcDockPlotter::supportedTypeStrs();
	//plotTypes.push_front(u8"无");
	//comboBoxDelegate->setItems(plotTypes);
	//setItemDelegateForColumn(3, comboBoxDelegate);
	setEditTriggers(QAbstractItemView::SelectedClicked);
	setSelectionMode(QAbstractItemView::SingleSelection);
	//setFixedHeight(24);
	setUniformRowHeights(true);
	setColumnWidth(0, QApplication::primaryScreen()->availableGeometry().width() / 4);

	connectSignals_();
}


QtWorkspaceWidget::~QtWorkspaceWidget()
{
	deleteItem_(root_);
}


void QtWorkspaceWidget::insertItem_(QTreeWidgetItem* parent, KvPropertiedObject* obj)
{
	auto objp = getObject(parent);
	if (objp) {
		if (obj->parent() == nullptr)
			obj->setParent(objp);
		assert(obj->parent() == dynamic_cast<QObject*>(objp));
		auto objpp = dynamic_cast<KvDataProvider*>(objp);
		assert(objpp);

		if (dynamic_cast<KvDataRender*>(obj))
			obj->connect(dynamic_cast<KvDataProvider*>(objp), &KvDataProvider::onData, 
				dynamic_cast<KvDataRender*>(obj), &KvDataRender::render);
		else if (dynamic_cast<KvDataOperator*>(obj))
			obj->connect(dynamic_cast<KvDataProvider*>(objp), &KvDataProvider::onData, 
				dynamic_cast<KvDataOperator*>(obj), &KvDataOperator::process);
	}

	auto item = new QTreeWidgetItem(parent);
	item->setText(0, obj->name());
	item->setData(0, Qt::UserRole, reinterpret_cast<qlonglong>(obj));
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); // 以便调用editItem
	scrollToItem(item);
}

void QtWorkspaceWidget::insertObject(KvPropertiedObject* obj, bool asRootChild)
{
	insertItem_(asRootChild ? root_ : currentItem(), obj);
}


KvPropertiedObject* QtWorkspaceWidget::getObject(const QTreeWidgetItem* item) const
{
	assert(item != nullptr);

	auto var = item->data(0, Qt::UserRole);
	auto obj = reinterpret_cast<KvPropertiedObject*>(var.toLongLong());
	return obj;
}


KvPropertiedObject* QtWorkspaceWidget::currentObject() const
{
	return currentItem() ? getObject(currentItem()) : nullptr;
}


KvPropertiedObject* QtWorkspaceWidget::rootObject() const
{ 
	return rootItem() ? getObject(rootItem()) : nullptr;
}


QTreeWidgetItem* QtWorkspaceWidget::findItem_(const KvPropertiedObject* obj)
{
	if (getObject(root_) == obj)
		return root_;

	return findChildItem_(root_, obj);
}


QTreeWidgetItem* QtWorkspaceWidget::findChildItem_(const QTreeWidgetItem* item, const KvPropertiedObject* obj)
{
	for(int i = 0; i < item->childCount(); i++) {
		auto c = item->child(i);
		if (getObject(c) == obj)
			return c;

		auto subitem = findChildItem_(c, obj);
		if (subitem) {
			assert(getObject(subitem) == obj);
			return subitem;
		}
	}

	return nullptr;
}


void QtWorkspaceWidget::connectSignals_()
{
	connect(this, &QTreeWidget::itemChanged, this, &QtWorkspaceWidget::onItemChanged);

	connect(kAppEventHub, &QtAppEventHub::objectActivated, this, &QtWorkspaceWidget::selectObject);

	connect(this, &QTreeWidget::currentItemChanged, 
		[this](QTreeWidgetItem* current, QTreeWidgetItem*) {
			assert(current);
			auto obj = getObject(current);
			emit kAppEventHub->objectActivated(obj);
		});


	// 用户双击tree-item触发：弹出显示窗口
	connect(this, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem* item, int) {
	    auto obj = dynamic_cast<KvDataRender*>(getObject(item));
	    if ( obj && obj->canShown())
			obj->show(true);
	    });
}

void QtWorkspaceWidget::selectObject(const KvPropertiedObject* obj)
{
	if (obj) {
		auto item = findItem_(obj);
		if (item && currentItem() != item)
			setCurrentItem(item);
	}
	else {
		this->clearSelection();
	}
}


void QtWorkspaceWidget::contextMenuEvent(QContextMenuEvent*)
{
	QTreeWidgetItem* curItem = currentItem();  //获取当前被点击的节点
	if (curItem == nullptr || curItem == root_) return;

	QMenu menu(this);

	QAction renderItem(tr("render"), this); // 仅stream
	QAction stopItem(tr("stop"), this); // 仅stream
	QAction showItem(tr("show"), this); // 仅render

	auto obj = getObject(curItem);
	auto provider = dynamic_cast<KvDataProvider*>(obj);
	if (provider) {
		if (provider->isStream() && provider->running()) {
			connect(&stopItem, &QAction::triggered, provider, &KvDataProvider::stop);
			menu.addAction(&stopItem);
		}
		else {
			connect(&renderItem, &QAction::triggered, provider, &KvDataProvider::start);
			menu.addAction(&renderItem);
		}
	}
	else {
		auto render = dynamic_cast<KvDataRender*>(obj);
		if (render && render->canShown()) {
			showItem.setCheckable(true);
			showItem.setChecked(render->isVisible());
			connect(&showItem, &QAction::triggered, render, &KvDataRender::show);
			menu.addAction(&showItem);
		}
	}


	QAction deleteItem(tr("delete"), this);
	QAction renameItem(tr("rename"), this);
	connect(&deleteItem, &QAction::triggered, this, &QtWorkspaceWidget::onDeleteItem);
	connect(&renameItem, &QAction::triggered, this, [this](){ editItem(currentItem(), 0); });
	menu.addAction(&deleteItem);
	menu.addAction(&renameItem);


	menu.exec(QCursor::pos());
}


void QtWorkspaceWidget::onDeleteItem()
{
	deleteItem_(currentItem());
}


void QtWorkspaceWidget::deleteItem_(QTreeWidgetItem* item)
{
	if (item) {
		// Qt的object体系，会保证自动删除所属children对象
		// for (auto c : item->takeChildren())
		//	 deleteItem_(c);

		delete getObject(item);
		delete item;
	}
}


void QtWorkspaceWidget::onItemChanged(QTreeWidgetItem* item, int column)
{
	assert(column == 0);

	auto name = item->text(0);
	auto obj = getObject(item);
	if (obj && obj->name() != name)
		obj->setName(name);
}

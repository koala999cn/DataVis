#include "QtDataTreeView.h"
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include "KvInputSource.h"
#include "dsp/KvData.h"
#include "kddockwidgets/DockWidget.h"
#include "KcDockPlotter.h"
#include "gui/QtComboBoxDelegate.h"
#include <assert.h>
#include "QtAppEventHub.h"
#include "KvInputStream.h"


QtDataTreeView::QtDataTreeView(QWidget* parent)
	: QTreeWidget(parent)
{
	setFrameStyle(QFrame::NoFrame);
	setHeaderLabels( { u8"名称", u8"维度", u8"类型", u8"显示"} );
	root_ = new QTreeWidgetItem(this, { u8"数据源" });
	auto comboBoxDelegate = new QtComboBoxDelegate(this);
	QStringList plotTypes = KcDockPlotter::supportedTypeStrs();
	plotTypes.push_front(u8"无");
	comboBoxDelegate->setItems(plotTypes);
	setItemDelegateForColumn(3, comboBoxDelegate);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);

	connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(onItemChanged(QTreeWidgetItem*, int)));

	connect(this, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int column) {
		if (currentItem() == item && (column == 0 || column == 3))
			editItem(item, column);
		});

	connect(kAppEventHub, &QtAppEventHub::onOutputDeviceActivated, this, &QtDataTreeView::onOutputDeviceActivated);
}


QtDataTreeView::~QtDataTreeView()
{
	// free space for KpDataSource
	for (auto i : root_->takeChildren()) {
		assert(i->childCount() == 0);
		delete getAttachedSource(i); 
		delete getAttachedPlot(i);
	}
}


void QtDataTreeView::insertItem(KvInputSource* source, KcDockPlotter* plot)
{
	QString dimText[] = { u8"一维", u8"二维", u8"三维", u8"多维" };
	auto dim = 1; //  source->data_ptr->dim(); if (dim > 3) dim = 3;

	// TODO: 根据plot初始化显示类型combox
	auto item = new QTreeWidgetItem(root_, { source->name(), dimText[dim-1], /*source->typeText()*/"TODO", u8"无"});
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); // 以便调用editItem
	attachSource_(item, source);
	attachPlot_(item, plot);
	scrollToItem(item);

	if (source->isStream()) {
		auto stream = dynamic_cast<KvInputStream*>(source);
		connect(stream, &KvInputStream::onStreamData, plot, &KcDockPlotter::streamData);
	}
}


KvInputSource* QtDataTreeView::getAttachedSource(QTreeWidgetItem* item)
{
	auto var = item->data(0, Qt::UserRole);
	auto source = reinterpret_cast<KvInputSource*>(var.toLongLong());
	return source;
}


KcDockPlotter* QtDataTreeView::getAttachedPlot(QTreeWidgetItem* item)
{
	auto var = item->data(3, Qt::UserRole);
	auto plot = reinterpret_cast<KcDockPlotter*>(var.toLongLong());
	return plot;
}


void QtDataTreeView::attachSource_(QTreeWidgetItem* item, KvInputSource* source)
{
	assert(getAttachedSource(item) == nullptr);
	item->setData(0, Qt::UserRole, reinterpret_cast<qlonglong>(source));
}


void QtDataTreeView::attachPlot_(QTreeWidgetItem* item, KcDockPlotter* plot)
{
	assert(getAttachedPlot(item) == nullptr);
	item->setData(3, Qt::UserRole, reinterpret_cast<qlonglong>(plot));
}


void QtDataTreeView::contextMenuEvent(QContextMenuEvent*)
{
	QTreeWidgetItem* curItem = currentItem();  //获取当前被点击的节点
	if (curItem == nullptr || curItem == root_) return;

	QMenu menu(this);

	QAction startItem(tr("start"), this);
	QAction stopItem(tr("stop"), this);

	auto input = getAttachedSource(curItem);
	if (input && input->isStream()) {
		auto stream = dynamic_cast<KvInputStream*>(input);
		if (stream->running()) {
			connect(&stopItem, &QAction::triggered, stream, &KvInputStream::stop);
			menu.addAction(&stopItem);
		}
		else {
			connect(&startItem, &QAction::triggered, stream, &KvInputStream::start);
			menu.addAction(&startItem);
		}
	}

	QAction showItem(tr("show"), this);
	auto typeText = curItem->text(3);
	int type = KcDockPlotter::typeFromStr(typeText);
	if (type >= 0) {
		connect(&showItem, &QAction::triggered, this, [this]() {
			auto plot = getAttachedPlot(currentItem());
			assert(plot && plot->name() == currentItem()->text(0));
			plot->show();
			});
		menu.addAction(&showItem);
	}

	QAction deleteItem(tr("delete"), this);
	QAction renameItem(tr("rename"), this);
	connect(&deleteItem, &QAction::triggered, this, &QtDataTreeView::onDeleteItem);
	connect(&renameItem, &QAction::triggered, this, [this](){ editItem(currentItem(), 0); });
	menu.addAction(&deleteItem);
	menu.addAction(&renameItem);


	menu.exec(QCursor::pos());
}


void QtDataTreeView::onDeleteItem()
{
	auto curItem = currentItem();
	auto parent = curItem->parent();
	assert(parent != nullptr);

	delete getAttachedSource(curItem);
	delete getAttachedPlot(curItem);
	parent->removeChild(curItem);
}


void QtDataTreeView::onItemChanged(QTreeWidgetItem* item, int column)
{
	if (column == 0) { // update name
		auto name = item->text(0);
		auto source = getAttachedSource(item);
		if (source && source->name() != name)
			source->setName(name);

		auto plot = getAttachedPlot(item);
		if (plot && plot->name() != name)
			plot->setName(name);	
	}
	else if (column == 3) { // update plot type
		auto typeText = item->text(3);
		int type = KcDockPlotter::typeFromStr(typeText);
		auto plot = getAttachedPlot(item);
		assert(plot && plot->name() == item->text(0));
		if (type != plot->type()) {
			plot->setType(type);
			emit outputTypeChanged(plot);
		}
	}
}


void QtDataTreeView::onOutputDeviceActivated(KvOutputDevice* output, bool active)
{
	if (active && root_) {
		auto c = root_->childCount();
		for (int i = 0; i < c; i++) {
			auto item = root_->child(i);
			auto plot = getAttachedPlot(item);
			if (plot && static_cast<KvOutputDevice*>(plot) == output && currentItem() != item)
				setCurrentItem(item);
		}
	}
}
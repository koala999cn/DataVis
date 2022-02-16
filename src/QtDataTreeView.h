#pragma once
#include <QTreeWidget>
#include <string>
#include <memory>


class KvInputSource;
class KcDockPlotter;
class KvOutputDevice;
class QContextMenuEvent;


class QtDataTreeView : public QTreeWidget
{
	Q_OBJECT

public:
	QtDataTreeView(QWidget* parent = nullptr);
	virtual ~QtDataTreeView();

	void insertItem(KvInputSource* source, KcDockPlotter* plot);

	KvInputSource* getAttachedSource(QTreeWidgetItem* item);
	KcDockPlotter* getAttachedPlot(QTreeWidgetItem* item);

protected:
	void attachSource_(QTreeWidgetItem* item, KvInputSource* source);
	void attachPlot_(QTreeWidgetItem* item, KcDockPlotter* plot);

private slots:
	void onDeleteItem();
	void onItemChanged(QTreeWidgetItem*, int);
	void onOutputDeviceActivated(KvOutputDevice* output, bool active);

signals:
	void outputTypeChanged(KcDockPlotter* plot); // 当用户选择不同的显示类型，发送此信号

private:
	void contextMenuEvent(QContextMenuEvent*) override;

private:
	QTreeWidgetItem* root_;
};


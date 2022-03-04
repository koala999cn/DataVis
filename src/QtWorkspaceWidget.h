#pragma once
#include <QTreeWidget>
#include <string>
#include <memory>


class KvPropertiedObject;
class QContextMenuEvent;


class QtWorkspaceWidget : public QTreeWidget
{
	Q_OBJECT

public:
	QtWorkspaceWidget(QWidget* parent = nullptr);
	virtual ~QtWorkspaceWidget();

	// @asRootChild: 若为true，则将obj作为root的子节点，否则obj将作为currentItem的子节点
	void insertObject(KvPropertiedObject* obj, bool asRootChild);

	KvPropertiedObject* getObject(const QTreeWidgetItem* item) const;
	KvPropertiedObject* currentObject() const { return getObject(currentItem()); }
	KvPropertiedObject* rootObject() const { return getObject(rootItem()); }

	void selectObject(const KvPropertiedObject* obj);


	QTreeWidgetItem* rootItem() const { return root_; }

protected:

	// insert a new item that's child of @parent, and attach @obj to it
	void insertItem_(QTreeWidgetItem* parent, KvPropertiedObject* obj);

	// delete @item and its children
	void deleteItem_(QTreeWidgetItem* item);

	// find item whose attached object equals to @obj
	QTreeWidgetItem* findItem_(const KvPropertiedObject* obj);

	// find @parent's descendant item whose attached object equals to @obj
	QTreeWidgetItem* findChildItem_(const QTreeWidgetItem* parent, const KvPropertiedObject* obj);

	void connectSignals_();

private slots:
	void onDeleteItem();
	void onItemChanged(QTreeWidgetItem*, int);


private:
	void contextMenuEvent(QContextMenuEvent*) override;

private:
	QTreeWidgetItem* root_;
};


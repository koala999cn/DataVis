#pragma once
#include <QItemDelegate>
#include <QComboBox>


// 用于QTreeWidget，以支持ComboBox组件

/* USAGE
 *
 *	auto comboBoxDelegate = new QtComboBoxDelegate(this);
 *	QStringList items = { "item1", "item2", "item3" };
 *	comboBoxDelegate->setItems(item);
 *	setItemDelegateForColumn(col, comboBoxDelegate);
 * 
 */

class QtComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	QtComboBoxDelegate(QObject* parent = nullptr)
		: QItemDelegate(parent) {}

	void setItems(const QStringList& texts) { _texts = texts; }

public:
	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override {
		QComboBox* editor = new QComboBox(parent);
		editor->addItems(_texts);
		return editor;
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const override {
		QString text = index.model()->data(index, Qt::EditRole).toString();
		QComboBox* comboBox = static_cast<QComboBox*>(editor);
		int tindex = comboBox->findText(text);
		comboBox->setCurrentIndex(tindex);
	}

	void setModelData(QWidget* editor,
		QAbstractItemModel* model,
		const QModelIndex& index) const override {
		QComboBox* comboBox = static_cast<QComboBox*>(editor);
		QString text = comboBox->currentText();
		model->setData(index, text, Qt::EditRole);
	}

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override {
		editor->setGeometry(option.rect);
	}

private:
	QStringList _texts;
};
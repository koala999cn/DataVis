#pragma once
#include "QtnProperty/PropertyWidget.h"
#include "QtnProperty/Auxiliary/PropertyAux.h" // for QtnPropertyChangeReason
#include "KvPropertiedObject.h"


// 根据KvPropertiedObject动态构建Qt属性窗口元素
// 提供基于属性id的取值、赋值等操作
class QtnPropertyWidgetX : public QtnPropertyWidget
{
	Q_OBJECT

public:
	QtnPropertyWidgetX(QWidget* parent = nullptr);
	~QtnPropertyWidgetX();

	// 根据propObj构建QtnPropertyWidget各元素
	void sync(KvPropertiedObject* propObj);

	// 根据id获取属性值
	QVariant value(int id) const;

	// 根据id设置属性值
	void setValue(int id, const QVariant& val);


private slots:
	void onPropertyDidChange(QtnPropertyChangeReason reason);

signals:
	void propertyChanged(int id, const QVariant& val);

private:
	QtnPropertyBase* createProperty_(const KvPropertiedObject::KpProperty& prop);
	void addProperties_(QtnPropertySet* parent, const KvPropertiedObject::kPropertySet& ps);
	QtnPropertyBase* findProperty_(const QtnPropertyBase* parent, int id) const;
	void setDelegateAttributes_(QtnPropertyBase* qtn, const KvPropertiedObject::KpProperty& prop);
	QVariantMap collectDelegateAttributes_(const KvPropertiedObject::KpProperty& prop);

private:
	std::vector<void*> bufs_; // 用于保存临时对象
	KvPropertiedObject* obj_; // 当前显示的obj
};




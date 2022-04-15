#pragma once
#include <QObject>
#include <QString>
#include <QVariant>
#include <vector>


// 具属性对象的抽象接口，用来桥接业务与Qt属性组件
class KvPropertiedObject : public QObject
{
	Q_OBJECT

public:

	KvPropertiedObject(const QString& name, KvPropertiedObject* parent = nullptr) 
	    : QObject(parent) { 
		setName(name); 
	}

	virtual ~KvPropertiedObject() {}

	QString name() const { return objectName(); }
	void setName(const QString& newName) { setObjectName(newName); }

	KvPropertiedObject* rootParent() const {
		auto p = parent();
		if (p == nullptr)
			return nullptr;
		while (p->parent())
			p = p->parent();

		return dynamic_cast<KvPropertiedObject*>(p);
	}


public slots:

	// 属性id的值在属性页中被用户编辑更改了，同步指定的属性
	void setProperty(int id, const QVariant& newVal) {
		setPropertyImpl_(id, newVal);
	}

private:
	virtual void setPropertyImpl_(int id, const QVariant& newVal) = 0;

public:

	struct KpProperty
	{
		int id;
		QString name;
		QString disp; // display name, 为空时使用name
		QVariant val;
		QString desc;
		int flag;

		QVariant minVal, maxVal, step;

		union {
			int colorFlags; // 
			int penFlags; // 取值KePenFlag的组合，适用于QPen类型属性
			bool showAllBrushStyle; // 默认为false
		} attr;

		std::vector<KpProperty> children;

		KpProperty() : id(-1), flag(0), attr{0} {}

		void reset() {
			id = -1, flag = 0;
			name.clear(), desc.clear();
			val.clear(), minVal.clear(), maxVal.clear();
			attr.penFlags = 0;
			children.clear();
		}
	};

	using kPropertySet = std::vector<KpProperty>;

	virtual kPropertySet propertySet() const = 0; // 返回对象的属性集合


public:

	enum KePropertyFlag
	{
		k_readonly = 0x01, // 属性只读，默认为可编辑
		k_collapsed = 0x02, // 属性处于折叠状态，不展开子属性，默认为展开状态
		k_restrict = 0x04  // 属性的最大最小值相互制约，仅对QPoint, QPointF类型有效
	};

	enum KeColorFlag
	{
		k_show_color_items = 0x01,
		k_show_alpha_channel = 0x02
	};

	enum KePenFlag
	{
		k_pen_none = 0x01, // 缺省为false
		k_pen_color = 0x02, // 缺省为true
		k_pen_style = 0x04, // 缺省为true
		k_pen_width = 0x08, // 缺省为true
		k_pen_cap_style = 0x10, // 缺省为true
		k_pen_join_style = 0x20, // 缺省为true
		k_pen_color_alpha = 0x40,
		k_pen_all = k_pen_color | k_pen_style | k_pen_width | k_pen_cap_style 
		          | k_pen_join_style | k_pen_color_alpha
	};

	static constexpr int kInvalidId = -1;
};


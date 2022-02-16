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

	enum KePropertyFlag
	{
		k_none,
		k_readonly = 0x01, // 属性只读，默认为可编辑
		k_collapsed = 0x02, // 属性处于折叠状态，不展开子属性，默认为展开状态
		k_restrict = 0x04  // 属性的最大最小值相互制约，仅对QPoint, QPointF类型有效
	};

	enum KePenFlag
	{
		k_pen_none,
		k_pen_color = 0x01,
		k_pen_style = 0x02,
		k_pen_width = 0x04,
		k_pen_all = k_pen_color | k_pen_style | k_pen_width
	};

	static constexpr int kInvalidId = -1;

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
			int penFlags; // 适用于QPen类型属性
			int enumValue; //适用于Enum类型属性，对应于short类型
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

	KvPropertiedObject(const QString& name) : name_(name) {}
	virtual ~KvPropertiedObject() {}

	// 属性名字不能含有" "和"."
	const QString& name() const { return name_; }
	virtual void setName(const QString& newName) { name_ = newName; }

	virtual const QString& displayName() const { return name(); }
	virtual kPropertySet propertySet() const = 0; // 返回对象的属性集合

	// 属性id赋值变化的notify函数
	virtual void onPropertyChanged(int id, const QVariant& newVal) {}

private:
	QString name_; // 对象名字
};


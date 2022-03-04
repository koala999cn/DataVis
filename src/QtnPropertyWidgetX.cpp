#include "QtnPropertyWidgetX.h"
#include "KvDataProvider.h"
#include "QtnProperty/PropertyCore.h"
#include "QtnProperty/PropertyGUI.h"
#include "QtnProperty/PropertyQVariant.h"
#include "QtnProperty/PropertyDelegateAttrs.h"
#include "QtnPropertySetX.h"


QtnPropertyWidgetX::QtnPropertyWidgetX(QWidget* parent)
    : QtnPropertyWidget(parent)
    , obj_(nullptr)
{
    auto propSet = new QtnPropertySet(this);
    setPropertySet(propSet);
}


QtnPropertyWidgetX::~QtnPropertyWidgetX()
{
    delete propertySet();
    setPropertySet(nullptr);

    for (auto p : bufs_)
        free(p);
}


void QtnPropertyWidgetX::sync(KvPropertiedObject* propObj)
{
    if (obj_ == propObj)
        return;

    obj_ = propObj;
    propertySet()->clearChildProperties();

    for (auto p : bufs_) free(p);
    bufs_.clear();

   if (propObj) 
       addProperties_(propertySet(), propObj->propertySet());
}


void QtnPropertyWidgetX::onPropertyDidChange(QtnPropertyChangeReason reason)
{
    if (reason & QtnPropertyChangeReasonValue) {
        auto prop = dynamic_cast<QtnPropertyBase*>(sender());
        assert(prop != nullptr);
        auto var = prop->valueAsVariant();
        if (var.type() == QMetaType::QPointF) {
            auto val = var.value<QPointF>();
            QVariantMap attr;
            attr[qtnMinAttr()] = std::numeric_limits<qreal>::lowest();
            attr[qtnMaxAttr()] = val.y(); // 设置x的最大值为y
            prop->setDelegateAttribute("x", QVariant::fromValue(attr)); 

            attr.clear();
            attr[qtnMinAttr()] = val.x(); // 设置y的最小值为x
            attr[qtnMaxAttr()] = std::numeric_limits<qreal>::max();
            prop->setDelegateAttribute("y", QVariant::fromValue(attr), QtnPropertyChangeReasonNewAttribute);
        }

        emit propertyChanged(prop->id(), var);
    }
}


QtnPropertyBase* QtnPropertyWidgetX::createProperty_(const KvPropertiedObject::KpProperty& prop)
{
    QtnPropertyBase* qtn(nullptr);

    switch (prop.val.type()) {
    case QMetaType::Bool:        qtn = new QtnPropertyBool(nullptr);        break;
    case QMetaType::UInt:        qtn = new QtnPropertyUInt(nullptr);        break;
    case QMetaType::Float:       qtn = new QtnPropertyFloat(nullptr);       break;
    case QMetaType::Double:      qtn = new QtnPropertyDouble(nullptr);      break;
    case QMetaType::QString:     qtn = new QtnPropertyQString(nullptr);     break;
    case QMetaType::QPoint:      qtn = new QtnPropertyQPoint(nullptr);      break;
    case QMetaType::QPointF:     qtn = new QtnPropertyQPointF(nullptr);     break;
    case QMetaType::QSize:       qtn = new QtnPropertyQSize(nullptr);       break;
    case QMetaType::QSizeF:      qtn = new QtnPropertyQSizeF(nullptr);      break;
    case QMetaType::QRect:       qtn = new QtnPropertyQRect(nullptr);       break;
    case QMetaType::QRectF:      qtn = new QtnPropertyQRectF(nullptr);      break;
    case QMetaType::QColor:      qtn = new QtnPropertyQColor(nullptr);      break;
    case QMetaType::QPen:        qtn = new QtnPropertyQPen(nullptr);        break;
    case QMetaType::QBrush:      qtn = new QtnPropertyQBrushStyle(nullptr); break;
    case QMetaType::QFont:       qtn = new QtnPropertyQFont(nullptr);       break;
    case QMetaType::QVector3D:   qtn = new QtnPropertyQVector3D(nullptr);   break;
    case QMetaType::UnknownType: qtn = new QtnPropertySet();                break;
    case QMetaType::Int:         
        if (prop.children.empty())
            qtn = new QtnPropertyInt(nullptr);
        else 
            qtn = new QtnPropertyEnum(nullptr);
        break;
    default:                     qtn = new QtnPropertyQVariant(nullptr);    break;
    }


    setDelegateAttributes_(qtn, prop);
    connect(qtn, &QtnPropertyBase::propertyDidChange, this, &QtnPropertyWidgetX::onPropertyDidChange);

    return qtn;
}


void QtnPropertyWidgetX::addProperties_(QtnPropertySet* parent, const KvPropertiedObject::kPropertySet& ps)
{
    for (auto& p : ps) {
        auto qtn = createProperty_(p);
        parent->addChildProperty(qtn);

        if (p.val.type() == QMetaType::UnknownType) {
            assert(qtn->asPropertySet());
            addProperties_(qtn->asPropertySet(), p.children);
        }
    }
}


QtnPropertyBase* QtnPropertyWidgetX::findProperty_(const QtnPropertyBase* parent, int id) const
{
    if (parent->id() == id)
        return const_cast<QtnPropertyBase*>(parent);

    auto propSet = parent->asPropertySet();
    if (propSet) {
        auto& children = propSet->childProperties();
        for (auto p : children) {
            auto r = findProperty_(p, id);
            if (r != nullptr)
                return r;
        }
    }

    return nullptr;
}


void QtnPropertyWidgetX::setDelegateAttributes_(QtnPropertyBase* qtn, const KvPropertiedObject::KpProperty& prop)
{
    if(prop.id != KvPropertiedObject::kInvalidId)
        qtn->setId(prop.id);

    if(!prop.name.isEmpty())
        qtn->setName(prop.name);

    if (!prop.disp.isEmpty())
        qtn->setDisplayName(prop.disp);

    if(!prop.desc.isEmpty())
        qtn->setDescription(prop.desc);

    if(!prop.val.isNull())
        qtn->fromVariant(prop.val);

    if (!prop.minVal.isNull())
        qtn->setDelegateAttribute(qtnMinAttr(), prop.minVal);

    if (!prop.maxVal.isNull())
        qtn->setDelegateAttribute(qtnMaxAttr(), prop.maxVal);

    if (!prop.step.isNull())
        qtn->setDelegateAttribute(qtnStepAttr(), prop.step);


    if (prop.flag & KvPropertiedObject::k_readonly)
        qtn->setState(QtnPropertyStateImmutable);

    if (prop.flag & KvPropertiedObject::k_collapsed)
        qtn->setState(QtnPropertyStateCollapsed);


    // 一些特殊的属性

    if (prop.val.type() == QMetaType::Int && !prop.children.empty()) { // config enum value infos
        auto qtnEnum = dynamic_cast<QtnPropertyEnum*>(qtn);
        void* buf = malloc(sizeof(QtnEnumInfo));
        bufs_.push_back(buf);
        QtnEnumInfo* enumInfo = new(buf) QtnEnumInfo;
        for (auto& c : prop.children)
            enumInfo->getVector().push_back(QtnEnumValueInfo(c.val.toInt(), c.name, c.disp));
        qtnEnum->setEnumInfo(enumInfo);
        qtnEnum->setValue(prop.val.value<int>()); // 添加enumInfo之后，需要重新设置value
    }
    else if (prop.val.type() == QMetaType::QPen) { // config pen's atrributes
        qtn->setDelegateAttribute(qtnShowNoPenAttr(),
            prop.attr.penFlags & KvPropertiedObject::k_show_no_pen);
        qtn->setDelegateAttribute(qtnEditColorAttr(), 
            prop.attr.penFlags & KvPropertiedObject::k_pen_color);
        qtn->setDelegateAttribute(qtnEditStyleAttr(), 
            prop.attr.penFlags & KvPropertiedObject::k_pen_style);
        qtn->setDelegateAttribute(qtnEditWidthAttr(), 
            prop.attr.penFlags & KvPropertiedObject::k_pen_width);
        qtn->setDelegateAttribute(qtnEditCapStyleAttr(),
            prop.attr.penFlags & KvPropertiedObject::k_pen_cap_style);
        qtn->setDelegateAttribute(qtnEditJoinStyleAttr(),
            prop.attr.penFlags & KvPropertiedObject::k_pen_join_style);
    }
    else if (prop.val.type() == QMetaType::QBrush) {
        qtn->fromVariant(static_cast<int>(prop.val.value<QBrush>().style())); // 须调整QBrush类型的赋值
        qtn->setDelegateAttribute(qtnShowAllAttr(), prop.attr.showAllBrushStyle);
    }
    else if (prop.val.type() == QMetaType::QColor) {
        qtn->setDelegateAttribute(qtnRgbSubItemsAttr(),
            prop.attr.colorFlags & KvPropertiedObject::k_show_color_items);
        qtn->setDelegateAttribute(qtnShowAlphaChannelAttr(),
            prop.attr.colorFlags & KvPropertiedObject::k_show_alpha_channel);
        //qtn->setDelegateAttribute(qtnShapeAttr(), QtnColorDelegateShapeCircle);
    }
    else if (prop.val.type() != QMetaType::UnknownType && !prop.children.empty()) { // config subProperty's attributes
        if(!prop.children[0].name.isEmpty())
            qtn->setDelegateAttribute(qtnXDisplayNameAttr(), prop.children[0].name);
        if (!prop.children[0].disp.isEmpty())
            qtn->setDelegateAttribute(qtnXDisplayNameAttr(), prop.children[0].disp);
        if (!prop.children[0].desc.isEmpty())
            qtn->setDelegateAttribute(qtnXDescriptionAttr(), prop.children[0].desc);
        auto subAttr = collectDelegateAttributes_(prop.children[0]);
        if(!subAttr.isEmpty())
            qtn->setDelegateAttribute("x", subAttr);

        if (prop.children.size() > 1) {
            if (!prop.children[1].name.isEmpty())
                qtn->setDelegateAttribute(qtnYDisplayNameAttr(), prop.children[1].name);
            if (!prop.children[1].disp.isEmpty())
                qtn->setDelegateAttribute(qtnYDisplayNameAttr(), prop.children[1].disp);
            if (!prop.children[1].desc.isEmpty())
                qtn->setDelegateAttribute(qtnYDescriptionAttr(), prop.children[1].desc);
            auto subAttr = collectDelegateAttributes_(prop.children[1]);
            if (!subAttr.isEmpty())
                qtn->setDelegateAttribute("y", subAttr);
        }

        if (prop.children.size() > 2) {
            if (!prop.children[2].name.isEmpty())
                qtn->setDelegateAttribute(qtnZDisplayNameAttr(), prop.children[2].name);
            if (!prop.children[2].disp.isEmpty())
                qtn->setDelegateAttribute(qtnZDisplayNameAttr(), prop.children[2].disp);
            if (!prop.children[2].desc.isEmpty())
                qtn->setDelegateAttribute(qtnZDescriptionAttr(), prop.children[2].desc);
            auto subAttr = collectDelegateAttributes_(prop.children[2]);
            if (!subAttr.isEmpty())
                qtn->setDelegateAttribute("z", subAttr);
        }
    }
}


QVariantMap QtnPropertyWidgetX::collectDelegateAttributes_(const KvPropertiedObject::KpProperty& prop)
{
    QVariantMap vm;

    if (!prop.minVal.isNull())
        vm[qtnMinAttr()] = prop.minVal;

    if (!prop.maxVal.isNull())
        vm[qtnMaxAttr()] = prop.maxVal;

    return vm;
}


QVariant QtnPropertyWidgetX::value(int id) const
{
    QVariant var;
    auto prop = findProperty_(propertySet(), id);
    if (prop != nullptr)
        var = prop->valueAsVariant();

    return var;
}


void QtnPropertyWidgetX::setValue(int id, const QVariant& val)
{
    auto prop = findProperty_(propertySet(), id);
    if (prop != nullptr)
        prop->fromVariant(val);
}


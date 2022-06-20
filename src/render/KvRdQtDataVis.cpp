#include "KvRdQtDataVis.h"
#include <QtDataVisualization/QAbstract3DGraph.h>
#include <QtDataVisualization/QAbstract3DAxis.h>
#include <QWidget>
#include "QtAppEventHub.h"


KvRdQtDataVis::KvRdQtDataVis(KvDataProvider* is, const QString& name)
    : KvDataRender(name, is)
{
	options_ = k_visible;
	graph3d_ = nullptr;
	widget_ = nullptr;
	xAxis_ = yAxis_ = zAxis_ = nullptr;
}


KvRdQtDataVis::~KvRdQtDataVis()
{
	setOption(k_visible, false);
	delete widget_;
	delete graph3d_;
}


void KvRdQtDataVis::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_visible);
	if (on) {
		if (widget_ == nullptr) {
			widget_ = QWidget::createWindowContainer(graph3d_);

			// ��������QWindow::activeChanged�źţ�QWindowò�Ʋ����򸸴���ת��
			connect(graph3d_, &QWindow::activeChanged, kAppEventHub, [=]() {
				if (graph3d_->isActive()) emit kAppEventHub->objectActivated(this);
				});
		}
		kAppEventHub->showDock(this, widget_);
	}
	else
		kAppEventHub->closeDock(this);
}


bool KvRdQtDataVis::getOption(KeObjectOption opt) const
{
	assert(opt == k_visible);
	assert(graph3d_);

	return graph3d_->isVisible();
}


namespace kPrivate
{
	enum KeQtDataVisProperty
	{
		k_theme,
		k_camera_preset,
		k_shadow_quality,

		k_axis_x,
		k_axis_x_range = k_axis_x,
		k_axis_y,
		k_axis_y_range = k_axis_y,
		k_axis_z,
		k_axis_z_range = k_axis_z,

		k_axis_range = k_axis_x_range - k_axis_x,
		k_axis_max = k_axis_range
	};

	// @idBase: ����������ʵid�Ļ���
	static KvPropertiedObject::kPropertySet getAxisProperties(QAbstract3DAxis* axis, int idBase)
	{
		KvPropertiedObject::kPropertySet psAxis;
		KvPropertiedObject::KpProperty prop;

		prop.id = idBase + k_axis_range;
		prop.name = QStringLiteral("Range");
		prop.flag = KvPropertiedObject::k_restrict;
		prop.val = QPointF(axis->min(), axis->max());
		KvPropertiedObject::KpProperty subProp;
		subProp.name = QStringLiteral("low");
		prop.children.push_back(subProp);
		subProp.name = QStringLiteral("high");
		prop.children.push_back(subProp);
		psAxis.push_back(prop);

		return psAxis;
	}

	static void onAxisPropertyChanged(QAbstract3DAxis* axis, int idDiff, const QVariant& newVal)
	{
		switch (idDiff) {
		case k_axis_range:
			axis->setRange(newVal.toPointF().x(), newVal.toPointF().y());
			break;

		default:
			assert(false);
			break;
		}
	}

	static const std::pair<QString, int> themeList[] = {
		{ "Qt", Q3DTheme::ThemeQt },
		{ "PrimaryColors", Q3DTheme::ThemePrimaryColors },
		{ "Digia", Q3DTheme::ThemeDigia },
		{ "StoneMoss", Q3DTheme::ThemeStoneMoss },
		{ "ArmyBlue", Q3DTheme::ThemeArmyBlue },
		{ "Retro", Q3DTheme::ThemeRetro },
		{ "Ebony", Q3DTheme::ThemeEbony },
		{ "Isabelle", Q3DTheme::ThemeIsabelle },
		{ "UserDefined", Q3DTheme::ThemeUserDefined }
	};

	static const std::pair<QString, int> presetList[] = {
		{ "FrontLow", Q3DCamera::CameraPresetFrontLow },
		{ "Front", Q3DCamera::CameraPresetFront },
		{ "FrontHigh", Q3DCamera::CameraPresetFrontHigh },
		{ "LeftLow", Q3DCamera::CameraPresetLeftLow },
		{ "Left", Q3DCamera::CameraPresetLeft },
		{ "LeftHigh", Q3DCamera::CameraPresetLeftHigh },
		{ "RightLow", Q3DCamera::CameraPresetRightLow },
		{ "Right", Q3DCamera::CameraPresetRight },
		{ "RightHigh", Q3DCamera::CameraPresetRightHigh },
		{ "BehindLow", Q3DCamera::CameraPresetBehindLow },
		{ "Behind", Q3DCamera::CameraPresetBehind },
		{ "BehindHigh", Q3DCamera::CameraPresetBehindHigh },
		{ "IsometricLeft", Q3DCamera::CameraPresetIsometricLeft },
		{ "IsometricLeftHigh", Q3DCamera::CameraPresetIsometricLeftHigh },
		{ "IsometricRight", Q3DCamera::CameraPresetIsometricRight },
		{ "IsometricRightHigh", Q3DCamera::CameraPresetIsometricRightHigh },
		{ "DirectlyAbove", Q3DCamera::CameraPresetDirectlyAbove },
		{ "DirectlyAboveCW45", Q3DCamera::CameraPresetDirectlyAboveCW45 },
		{ "DirectlyAboveCCW45", Q3DCamera::CameraPresetDirectlyAboveCCW45 },
		{ "FrontBelow", Q3DCamera::CameraPresetFrontBelow },
		{ "LeftBelow", Q3DCamera::CameraPresetLeftBelow },
		{ "LeftBelow", Q3DCamera::CameraPresetLeftBelow },
		{ "BehindBelow", Q3DCamera::CameraPresetBehindBelow },
		{ "DirectlyBelow", Q3DCamera::CameraPresetDirectlyBelow }
	};

	static const std::pair<QString, int> shadowQualityList[] = {
		{ "None", QAbstract3DGraph::ShadowQualityNone },
		{ "Low", QAbstract3DGraph::ShadowQualityLow },
		{ "Medium", QAbstract3DGraph::ShadowQualityMedium },
		{ "High", QAbstract3DGraph::ShadowQualityHigh },
		{ "SoftLow", QAbstract3DGraph::ShadowQualitySoftLow },
		{ "SoftMedium", QAbstract3DGraph::ShadowQualitySoftMedium },
		{ "SoftHigh", QAbstract3DGraph::ShadowQualitySoftHigh }
	};
}


KvRdQtDataVis::kPropertySet KvRdQtDataVis::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;
	KpProperty prop;

	assert(graph3d_ && xAxis_ && yAxis_ && zAxis_);

	prop.id = k_theme;
	prop.name = tr("Theme");
	prop.val = int(graph3d_->activeTheme()->type());
	prop.makeEnum(themeList);
	ps.push_back(prop);

	prop.id = k_camera_preset;
	prop.name = tr("CameraPreset");
	prop.val = int(graph3d_->scene()->activeCamera()->cameraPreset());
	prop.makeEnum(presetList);
	ps.push_back(prop);

	prop.id = k_shadow_quality;
	prop.name = tr("ShadowQuality");
	prop.val = int(graph3d_->shadowQuality());
	prop.makeEnum(shadowQualityList);
	ps.push_back(prop);

	prop.id = KvPropertiedObject::kInvalidId;
	prop.name = QStringLiteral("Axis");
	prop.val.clear();
	prop.flag = KvPropertiedObject::k_collapsed;
	prop.children.clear();
	KpProperty subProp;
	subProp.id = KvPropertiedObject::kInvalidId;
	subProp.flag = KvPropertiedObject::k_collapsed;
	subProp.name = QStringLiteral("X");
	subProp.children = getAxisProperties(xAxis_, k_axis_x);
	prop.children.push_back(subProp);
	subProp.name = QStringLiteral("Y");
	subProp.children = getAxisProperties(yAxis_, k_axis_y);
	prop.children.push_back(subProp);
	subProp.name = QStringLiteral("Z");
	subProp.children = getAxisProperties(zAxis_, k_axis_z);
	prop.children.push_back(subProp);
	ps.push_back(prop);

	return ps;
}


void KvRdQtDataVis::setPropertyImpl_(int id, const QVariant& newVal)
{
	using namespace kPrivate;

	switch (id)
	{
	case k_theme:
		graph3d_->activeTheme()->setType(Q3DTheme::Theme(newVal.toInt()));
		break;

	case k_camera_preset:
		graph3d_->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(newVal.toInt()));
		break;

	case k_shadow_quality:
		graph3d_->setShadowQuality(QAbstract3DGraph::ShadowQuality(newVal.toInt()));
		break;

	default:
		break;
	}

	if (id >= k_axis_x && id - k_axis_x <= k_axis_max)
		onAxisPropertyChanged(xAxis_, id - k_axis_x, newVal);
	else if (id >= k_axis_y && id - k_axis_y <= k_axis_max) 
	    onAxisPropertyChanged(yAxis_, id - k_axis_y, newVal);
	else if (id >= k_axis_z && id - k_axis_z <= k_axis_max) 
	    onAxisPropertyChanged(zAxis_, id - k_axis_z, newVal);
}


void KvRdQtDataVis::syncAxes_()
{
	auto objp = dynamic_cast<KvDataProvider*>(parent());
	assert(objp);

	auto r0 = objp->range(0);
	auto r1 = objp->range(1);
	xAxis_->setRange(r0.low(), r0.high());
	yAxis_->setRange(r1.low(), r1.high());

	if (objp->dim() == 1) { // ��x-yƽ������ʾһά����
		zAxis_->setRange(0, 0); 
	}
	else { 
		auto r2 = objp->range(2);
		zAxis_->setRange(r2.low(), r2.high());
	}
}
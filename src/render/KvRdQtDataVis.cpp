#include "KvRdQtDataVis.h"
#include <QtDataVisualization/QAbstract3DGraph.h>
#include "QtAppEventHub.h"


KvRdQtDataVis::KvRdQtDataVis(KvDataProvider* is, const QString& name)
    : KvDataRender(name, is)
{
	options_ = k_visible;
	graph3d_ = nullptr;
	theme_ = Q3DTheme::ThemeQt; // TODO:
}


KvRdQtDataVis::~KvRdQtDataVis()
{
	setOption(k_visible, false);
	delete graph3d_;
}


void KvRdQtDataVis::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_visible);
	if (on)
		kAppEventHub->showDock(this, graph3d_);
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
	};

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
}


KvRdQtDataVis::kPropertySet KvRdQtDataVis::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;
	KpProperty prop;

	prop.id = k_theme;
	prop.name = tr("Theme");
	prop.val = theme_;
	prop.makeEnum<sizeof(themeList) / sizeof(std::pair<QString, int>)>(themeList);
	ps.push_back(prop);

	prop.id = k_camera_preset;
	prop.name = tr("CameraPreset");
	prop.val = int(graph3d_->scene()->activeCamera()->cameraPreset());
	prop.makeEnum<sizeof(presetList) / sizeof(std::pair<QString, int>)>(presetList);
	ps.push_back(prop);

	return ps;
}


void KvRdQtDataVis::setPropertyImpl_(int id, const QVariant& newVal)
{
	switch (id)
	{
	case kPrivate::k_theme:
		theme_ = newVal.toInt();
		graph3d_->setActiveTheme(new Q3DTheme(Q3DTheme::Theme(theme_))); // TODO: 不需要每次都new theme
		break;

	case kPrivate::k_camera_preset:
		graph3d_->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPreset(newVal.toInt()));
		break;

	default:
		break;
	}
}

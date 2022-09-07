#include "KcVlCoordSystem.h"
#include "KgAxis.h"
#include "KcVlPlot3d.h"
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>


KcVlCoordSystem::KcVlCoordSystem()
	: KvCoordSystem(vec3(0, 0, 0), vec3(1, 1, 1))
{
	actorTree_ = new vl::ActorTree;
	
/*	vl::ref<vl::Effect> ax_effect = new vl::Effect;
	ax_effect->shader()->enable(vl::EN_LIGHTING);
	ax_effect->shader()->enable(vl::EN_DEPTH_TEST);
	ax_effect->shader()->enable(vl::EN_CULL_FACE);
	ax_effect->shader()->setRenderState(new vl::Light, 0);
	ax_effect->shader()->gocMaterial()->setDiffuse(vl::red);*/

	auto effect_ = new vl::Effect;

	// enable blend & depth-test & line-smooth
	effect_->shader()->enable(vl::EN_BLEND);
	effect_->shader()->enable(vl::EN_DEPTH_TEST);
	effect_->shader()->enable(vl::EN_LINE_SMOOTH);

	// Set up the lights
	effect_->shader()->disable(vl::EN_LIGHTING); // disable the standard OpenGL lighting
	effect_->shader()->gocLightModel()->setTwoSide(true);
	effect_->shader()->gocLightModel()->setAmbientColor(vl::white);

	// add a Light to the scene, since no Transform is associated to the Light it will follow the camera
	effect_->shader()->setRenderState(new vl::Light, 0);

	effect_->shader()->gocMaterial()->setColorMaterialEnabled(true);
	effect_->shader()->gocMaterial()->setDiffuse(vl::vec4(1.0f));
	effect_->shader()->gocMaterial()->setSpecular(vl::vec4(0.3f));
	effect_->shader()->gocMaterial()->setShininess(5.0f);

	effect_->shader()->gocLight(0)->setDiffuse(vl::vec4(1.0f));
	effect_->shader()->gocLight(0)->setSpecular(vl::vec4(1.0f));

	vl::ref<vl::Geometry> axis1 = vl::makeCylinder(vl::vec3(0, 0, 0), 4, 10);
	actorTree_->addActor(axis1.get(), effect_);
}


void KcVlCoordSystem::setVisible(bool b)
{
	actorTree_->setEnabled(b);
}


bool KcVlCoordSystem::visible() const
{
	return actorTree_->isEnabled();
}


void KcVlCoordSystem::update(KvPlot* plot) const
{
	auto plot3d = dynamic_cast<KcVlPlot3d*>(plot);
	if (!plot3d) return;

	auto applet = plot3d->applet();
	auto sm = applet->sceneManager();
	sm->tree()->addChildOnce(actorTree_.get_writable());
}

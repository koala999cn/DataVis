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

	// enable the standard OpenGL lighting
	effect_->shader()->enable(vl::EN_LIGHTING); 

	effect_->shader()->gocMaterial()->setFlatColor(vl::black);
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

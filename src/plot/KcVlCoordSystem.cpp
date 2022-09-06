#include "KcVlCoordSystem.h"
#include "KvAxis.h"
#include "KcVlPlot3d.h"
#include <vlGraphics/Applet.hpp>


KcVlCoordSystem::KcVlCoordSystem()
{
	axisX().reset(new KvAxis(KvAxis::k_x));
	axisY().reset(new KvAxis(KvAxis::k_y));
	axisZ().reset(new KvAxis(KvAxis::k_z));

	actorTree_ = new vl::ActorTree;
	
	vl::ref<vl::Effect> ax_effect = new vl::Effect;
	ax_effect->shader()->enable(vl::EN_LIGHTING);
	ax_effect->shader()->enable(vl::EN_DEPTH_TEST);
	ax_effect->shader()->enable(vl::EN_CULL_FACE);
	ax_effect->shader()->setRenderState(new vl::Light, 0);
	ax_effect->shader()->gocMaterial()->setDiffuse(vl::red);

	vl::ref<vl::Geometry> axis1 = vl::makeCylinder(vl::vec3(-360, 0, -360), 10, 360 * 2);
	actorTree_->addActor(axis1.get(), ax_effect.get());
}


void KcVlCoordSystem::update(KvPlot* plot) const
{
	auto plot3d = dynamic_cast<KcVlPlot3d*>(plot);
	if (!plot3d) return;

	auto applet = plot3d->applet();
	auto sm = applet->sceneManager();
	sm->tree()->addChildOnce(actorTree_);
}

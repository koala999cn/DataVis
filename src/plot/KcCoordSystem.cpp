#include "KcCoordSystem.h"
#include "KcAxis.h"


KcCoordSystem::KcCoordSystem()
	: KcCoordSystem(vec3(0, 0, 0), vec3(100, 100, 100))
{

}


KcCoordSystem::KcCoordSystem(const vec3& lower, const vec3& upper)
	: visible_(true)
{
	// ��ʼ��12��������
	axes_.resize(12, nullptr);
	for (int i = 0; i < 12; i++) 
		axes_[i].reset(new KcAxis);

	setRange(lower, upper);

	axes_[k_x0]->setTickOrient(KcAxis::k_neg_y);
	axes_[k_x1]->setTickOrient(KcAxis::k_y);
	axes_[k_x2]->setTickOrient(KcAxis::k_y);
	axes_[k_x3]->setTickOrient(KcAxis::k_neg_y);

	axes_[k_y0]->setTickOrient(KcAxis::k_neg_x);
	axes_[k_y1]->setTickOrient(KcAxis::k_x);
	axes_[k_y2]->setTickOrient(KcAxis::k_x);
	axes_[k_y3]->setTickOrient(KcAxis::k_neg_x);

	axes_[k_z0]->setTickOrient(KcAxis::k_neg_x);
	axes_[k_z1]->setTickOrient(KcAxis::k_x);
	axes_[k_z2]->setTickOrient(KcAxis::k_x);
	axes_[k_z3]->setTickOrient(KcAxis::k_neg_x);

	for(unsigned i = 0; i < 12; i++)
	    axes_[i]->setShowTick(false), axes_[i]->setShowLabel(false);

	axes_[k_x3]->setShowTick(true), axes_[k_x3]->setShowLabel(true);
	axes_[k_y3]->setShowTick(true), axes_[k_y3]->setShowLabel(true);
	axes_[k_z1]->setShowTick(true), axes_[k_z1]->setShowLabel(true);
}


/*                  
 *    p1 -------x1------  p6
 *     /z3           z2/|
 *  p2 --------x2-----  |y1
 *    |y3           y2| | p5
 *    |_______________|/z1
 *  p3         x3    p4
 *    
 */
void KcCoordSystem::setRange(const vec3& lower, const vec3& upper)
{
	// p0 = lower, p7 = upper
	auto p1 = vec3(lower.x, upper.y, lower.z);
	auto p2 = vec3(lower.x, upper.y, upper.z);
	auto p3 = vec3(lower.x, lower.y, upper.z);
	auto p4 = vec3(upper.x, lower.y, upper.z);
	auto p5 = vec3(upper.x, lower.y, lower.z);
	auto p6 = vec3(upper.x, upper.y, lower.z);

	axes_[k_x0]->setRange(lower.x, upper.x);
	axes_[k_x0]->setStart(lower); axes_[k_x0]->setEnd(p5);
	axes_[k_x1]->setRange(lower.x, upper.x);
	axes_[k_x1]->setStart(p1); axes_[k_x1]->setEnd(p6);
	axes_[k_x2]->setRange(lower.x, upper.x);
	axes_[k_x2]->setStart(p2); axes_[k_x2]->setEnd(upper);
	axes_[k_x3]->setRange(lower.x, upper.x);
	axes_[k_x3]->setStart(p3); axes_[k_x3]->setEnd(p4);

	axes_[k_y0]->setRange(lower.y, upper.y);
	axes_[k_y0]->setStart(lower); axes_[k_y0]->setEnd(p1);
	axes_[k_y1]->setRange(lower.y, upper.y);
	axes_[k_y1]->setStart(p5); axes_[k_y1]->setEnd(p6);
	axes_[k_y2]->setRange(lower.y, upper.y);
	axes_[k_y2]->setStart(p4); axes_[k_y2]->setEnd(upper);
	axes_[k_y3]->setRange(lower.y, upper.y);
	axes_[k_y3]->setStart(p3); axes_[k_y3]->setEnd(p2);

	axes_[k_z0]->setRange(lower.z, upper.z);
	axes_[k_z0]->setStart(lower); axes_[k_z0]->setEnd(p3);
	axes_[k_z1]->setRange(lower.z, upper.z);
	axes_[k_z1]->setStart(p5); axes_[k_z1]->setEnd(p4);
	axes_[k_z2]->setRange(lower.z, upper.z);
	axes_[k_z2]->setStart(p6); axes_[k_z2]->setEnd(upper);
	axes_[k_z3]->setRange(lower.z, upper.z);
	axes_[k_z3]->setStart(p1); axes_[k_z3]->setEnd(p2);
}


KcCoordSystem::vec3 KcCoordSystem::lower() const
{
	return { axes_[k_x0]->lower(), axes_[k_y0]->lower(), axes_[k_z0]->lower() };
}


KcCoordSystem::vec3 KcCoordSystem::upper() const
{
	return { axes_[k_x0]->upper(), axes_[k_y0]->upper(), axes_[k_z0]->upper() };
}


KcCoordSystem::vec3 KcCoordSystem::center() const
{
	return (lower() + upper()) / 2;
}


double KcCoordSystem::diag() const
{
	return (upper() - lower()).length();
}


void KcCoordSystem::setVisible(bool b)
{
	visible_ = b;;
}


bool KcCoordSystem::visible() const
{
	return visible_;
}


void KcCoordSystem::draw(KglPaint* paint) const
{
	if (visible()) {
		for (int i = 0; i < 12; i++) {
			auto ax = axis(i);
			auto rlen = diag();
			if (ax && ax->visible()) {
				ax->setRefLength(rlen);
				ax->draw(paint);
			}
		}
	}
}

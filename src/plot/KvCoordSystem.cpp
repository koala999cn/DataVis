#include "KvCoordSystem.h"
#include "KgAxis.h"


KvCoordSystem::KvCoordSystem(const vec3& lower, const vec3& upper)
{
	// 初始化12根坐标轴
	axes_.resize(12, nullptr);
	for (int i = 0; i < 12; i++) 
		axes_[i].reset(new KgAxis);

	updateRange(lower, upper);
}


/*                  
 *    p1 -------x1------  p6
 *  z3 /             z2/|
 *  p2 --------x2-----  |y1
 *  y3|             y2| | p5
 *    |_______________|/z1
 *  p3         x3    p4
 *    
 */
void KvCoordSystem::updateRange(const vec3& lower, const vec3& upper)
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


void KvCoordSystem::setVisible(bool b)
{
	visible_ = b;;
}


bool KvCoordSystem::visible() const
{
	return visible_;
}
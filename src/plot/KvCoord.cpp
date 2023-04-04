#include "KvCoord.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"


void KvCoord::resetAxisExtent_(KcAxis& axis, bool swap) const
{
	/*
	 *    p1 -------x1------  p6
	 *     /z3           z2/|
	 *  p2 --------x2-----  |y1
	 *    |y3           y2| | p5
	 *    |_______________|/z1
	 *  p3         x3    p4
	 *
	 */

	 // 定义坐标轴的extent取值
	 // 12代表KcAxis::KeAxisType标识的12根坐标轴
	 // 2代表start、end两个端点
	 // 3代表x、y、z三个维度
	constexpr static int axisExtent[12][2][3] = {
		{ {0, 0, 1}/*p3*/, {0, 1, 1}/*p2*/ }, // k_near_left, y3
		{ {1, 0, 1}/*p4*/, {1, 1, 1}/*p7*/ }, // k_near_right, y2
		{ {0, 0, 1}/*p3*/, {1, 0, 1}/*p4*/ }, // k_near_bottom, x3
		{ {0, 1, 1}/*p2*/, {1, 1, 1}/*p7*/ }, // k_near_top, x2

		{ {0, 0, 0}/*p0*/, {0, 1, 0}/*p1*/ }, // k_far_left, y0
		{ {1, 0, 0}/*p5*/, {1, 1, 0}/*p6*/ }, // k_far_right, y1
		{ {0, 0, 0}/*p0*/, {1, 0, 0}/*p5*/ }, // k_far_bottom, x0
		{ {0, 1, 0}/*p1*/, {1, 1, 0}/*p6*/ }, // k_far_top, x1

		{ {0, 0, 0}/*p0*/, {0, 0, 1}/*p3*/ }, // k_floor_left, z0
		{ {1, 0, 0}/*p5*/, {1, 0, 1}/*p4*/ }, // k_floor_right, z1
		{ {0, 1, 0}/*p1*/, {0, 1, 1}/*p2*/ }, // k_ceil_left, z3
		{ {1, 1, 0}/*p6*/, {1, 1, 1}/*p7*/ }, // k_ceil_right, z2
	};


	const static int dimOthers[][2] = {
		{ 1, 2 }, // 0维度对应的其他2个维度
		{ 0, 2 }, // 1维度对应的其他2个维度
		{ 0, 1 }  // 2维度对应的其他2个维度
	};

	const auto& loc = axisExtent[axis.type()];
	int lpos[3] = { loc[0][0], loc[0][1], loc[0][2] };
	int upos[3] = { loc[1][0], loc[1][1], loc[1][2] };
	if (swap) {
		assert(false); // NB: 坐标轴的extent始终使用局部坐标，不考虑交换
		auto d = dimOthers[axis.dim()];
		std::swap(lpos[d[0]], lpos[d[1]]);
		std::swap(upos[d[0]], upos[d[1]]);
	}

	point3 lower = { extent_[lpos[0]].x(), extent_[lpos[1]].y(), extent_[lpos[2]].z() };
	point3 upper = { extent_[upos[0]].x(), extent_[upos[1]].y(), extent_[upos[2]].z() };
	axis.setExtent(lower, upper);
}


void KvCoord::setExtents(const point3& lower, const point3& upper)
{
	extent_[0] = lower, extent_[1] = upper;

	forAxis([this](KcAxis& axis) {
		if (axis.main()) { // 只设置主坐标轴
			auto dim = axis.dim();
			axis.setRange(extent_[0][dim], extent_[1][dim]);
			resetAxisExtent_(axis, false);
		}
		return true;
		});
}


KvCoord::mat4 KvCoord::axisReflectMatrix_(int dim) const
{
	point3 vec(0);
	vec[dim] = upper()[dim] + lower()[dim]; // reflection造成2*lower()[dim]偏移，此处补偿回来，实际偏移量upper()[dim] - lower()[dim]
	auto trans  = mat4::buildTanslation(vec);

	vec3d dir(0); dir[dim] = 1; 
	return trans * mat4::buildReflection(dir);
}


void KvCoord::draw(KvPaint* paint) const
{
	if (visible()) {	
		forPlane([paint](KcCoordPlane& plane) {
			if (plane.visible())
				plane.draw(paint);
			return true;
			});

		paint->enablePolygonOffset(true); // 启动polygonOffset，防止grid与baseline冲突
		forAxis([paint, this](KcAxis& axis) {
			if (axis.visible() && axis.length() > 0) 
				axis.draw(paint);
			return true;
			});
		paint->enablePolygonOffset(false);
	}
}


KvCoord::aabb_t KvCoord::boundingBox() const
{
	aabb_t box(lower(), upper());

	forAxis([&box](KcAxis& axis) {
		if (axis.visible())
			box.merge(axis.boundingBox());
		return true;
		});

	return box;
}


KvCoord::aabb_t KvCoord::extentsInWorld() const
{
	aabb_t box(lower(), upper());

	auto& l = box.lower();
	auto& u = box.upper();

	switch (swapStatus_)
	{
	case k_axis_swap_xy:
		std::swap(l.x(), l.y()), std::swap(u.x(), u.y());
		break;

	case k_axis_swap_xz:
		std::swap(l.x(), l.z()), std::swap(u.x(), u.z());
		break;

	case k_axis_swap_yz:
		std::swap(l.y(), l.z()), std::swap(u.y(), u.z());
		break;

	default:
		break;
	}

	return box;
}


void KvCoord::zoom(float_t factor)
{
	auto c = center();
	auto delta = (upper() - lower()) * factor * 0.5;
	setExtents(c - delta, c + delta);

	assert(c.isApproxEqual(center()));
}


KvCoord::mat4 KvCoord::axisInverseMatrix() const
{
	const static float_t diag[] = { 1, -1 }; // invMat阵的对角线值

	return {
		diag[inv_[0]], 0, 0, inv_[0] ? lower().x() + upper().x() : 0,
		0, diag[inv_[1]], 0, inv_[1] ? lower().y() + upper().y() : 0,
		0, 0, diag[inv_[2]], inv_[2] ? lower().z() + upper().z() : 0,
		0, 0, 0, 1
	};
}


const KvCoord::mat4& KvCoord::axisSwapMatrix() const
{
	const static mat4 swapMat[] = {

		// swap xy
		{ 0, 1, 0, 0,
		  1, 0, 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1 },

		// sway xz
		{ 0, 0, 1, 0,
		  0, 1, 0, 0,
		  1, 0, 0, 0,
		  0, 0, 0, 1 },

		// sway yz
		{ 1, 0, 0, 0,
		  0, 0, 1, 0,
		  0, 1, 0, 0,
		  0, 0, 0, 1 },
	};

	return swapMat[swapStatus_ - 1];
}

/*
KvCoord::mat4 KvCoord::localMatrix() const
{
	// TODO: 先反转还是先交换？
	return swapStatus_ ? axisSwapMatrix_() * axisInverseMatrix_() : axisInverseMatrix_(); 
}
*/

void KvCoord::inverseAxis(int dim, bool inv)
{
	inv_[dim] = inv;

	forAxis([dim, inv](KcAxis& axis) {
		if (axis.dim() == dim/*&& axis.main()*/) // NB: 分离坐标轴与主坐标轴保持一致
			axis.setInversed(inv);
		return true;
		});
}


void KvCoord::swapAxis(KeAxisSwapStatus status)
{
	if (swapStatus_ == status)
		return;

	// NB: -1表示另外两个维度有交换
	constexpr static int swapped[][3] = {
		{ 0, 1, 2 }, // k_axis_swap_none
		{ 1, 0, -1 }, // k_axis_swap_xy
		{ 2, -1, 0 }, // k_axis_swap_xz
		{ -1, 2, 1 }  // k_axis_swap_yz
	};

	forAxis([status](KcAxis& axis) {
		axis.setSwapped_(swapped[status][axis.dim()]);
		return true;
		});

	swapStatus_ = status;
}


// 最初尝试的坐标轴交换实现方案
#if 0
namespace kPrivate
{
	static bool needSwap(int dim, KvCoord::KeAxisSwapStatus status)
	{
		static bool need[3][3] = {
			{ true, true, false }, // k_axis_swap_xy
			{ true, false, true }, // k_axis_swap_xz
			{ false, true, true }  // k_axis_swap_yz
		};

		assert(status > 0 && status < 4);
		assert(dim >= 0 && dim < 3);

		return need[status - 1][dim];
	}

	static void swapType(KcAxis& axis, KvCoord::KeAxisSwapStatus status)
	{
		// 可逆的类型交换数组
		static int swapType[][4] = {
			    /* swap_none */          /* swap_xy */           /* swap_xz */        /* swap_yz */
			{ KcAxis::k_near_left,   KcAxis::k_near_bottom, -1,                    KcAxis::k_ceil_left   },
			{ KcAxis::k_near_right,  KcAxis::k_near_top ,   -1,                    KcAxis::k_ceil_right  },
			{ KcAxis::k_near_bottom, KcAxis::k_near_left,   KcAxis::k_floor_right, -1                    },
			{ KcAxis::k_near_top,    KcAxis::k_near_right,  KcAxis::k_ceil_right,  -1                    },

			{ KcAxis::k_far_left,    KcAxis::k_far_bottom, -1,                     KcAxis::k_floor_left  },
			{ KcAxis::k_far_right,   KcAxis::k_far_top,    -1,                     KcAxis::k_floor_right },
			{ KcAxis::k_far_bottom,  KcAxis::k_far_left,   KcAxis::k_floor_left,   -1                    },
			{ KcAxis::k_far_top,     KcAxis::k_far_right,  KcAxis::k_ceil_left,    -1                    },

			{ KcAxis::k_floor_left,  -1,                   KcAxis::k_far_bottom,   KcAxis::k_far_left   },
			{ KcAxis::k_floor_right, -1,                   KcAxis::k_near_bottom,  KcAxis::k_far_right  },
			{ KcAxis::k_ceil_left,   -1,                   KcAxis::k_far_top,      KcAxis::k_near_left  },
			{ KcAxis::k_ceil_right,  -1,                   KcAxis::k_near_top,     KcAxis::k_near_right }
		};

		int t = swapType[axis.type()][status];
		assert(t != -1);
		axis.setType(KcAxis::KeType(t));
	}

	static void swapStartAndEnd(KcAxis& axis, KvCoord::KeAxisSwapStatus status)
	{
		static int swapDim[][2] = {
			{ 0, 1 }, { 0, 2 }, { 1, 2 }
		};

		auto st = axis.start(), ed = axis.end();
		auto& s = swapDim[status - 1];
		std::swap(st[s[0]], st[s[1]]);
		std::swap(ed[s[0]], ed[s[1]]);
		axis.setStart(st), axis.setEnd(ed);
	}

	static void swapTickAndLabelOrient(KcAxis& axis, KvCoord::KeAxisSwapStatus status)
	{

	}

	static void swapAxis(KcAxis& axis, KvCoord::KeAxisSwapStatus status)
	{
		assert(status != KvCoord::k_axis_swap_none);

		swapStartAndEnd(axis, status);

		if (needSwap(axis.dim(), status)) {
			swapType(axis, status);
			swapTickAndLabelOrient(axis, status);
		}
	}
}
#endif
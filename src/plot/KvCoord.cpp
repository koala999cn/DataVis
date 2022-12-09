#include "KvCoord.h"
#include "KcAxis.h"
#include "KcCoordPlane.h"
#include "KvPaint.h"


KvCoord::mat4 KvCoord::axisReflectMatrix_(int dim) const
{
	point3 vec(0);
	vec[dim] = upper()[dim] + lower()[dim]; // reflection���2*lower()[dim]ƫ�ƣ��˴�����������ʵ��ƫ����upper()[dim] - lower()[dim]
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

		forAxis([paint, this](KcAxis& axis) {
			if (axis.visible() && axis.length() > 0) {
	
				const static int otherDim[][2] = {
					{ 1, 2 }, { 0, 2 }, { 0, 1 }
				};

				const static bool otherSwapped[][4] = {
					{ false, false, false, true },
					{ false, false, true, false },
					{ false, true, false, false }
				};

				int dim = axis.dim();
				int localPushed(0);

				if (otherSwapped[dim][swapStatus_]) {
					//++localPushed;
					//paint->pushLocal(axisSwapMatrix_());
				}

				for (int i = 0; i < 2; i++) {
					if (axisInversed(otherDim[dim][i])) {
						++localPushed;
						paint->pushLocal(axisReflectMatrix_(otherDim[dim][i]));
					}
				}

				axis.draw(paint);

				for(int i = 0; i < localPushed; i++)
					paint->popLocal();
			}

			return true;
			});
	}
}


KvCoord::aabb_t KvCoord::boundingBox() const
{
	auto l = lower(), u = upper();

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

	return { l, u };
}


void KvCoord::zoom(float_t factor)
{
	auto c = center();
	auto delta = (upper() - lower()) * factor * 0.5;
	setExtents(c - delta, c + delta);

	assert(c.isApproxEqual(center()));
}


KvCoord::mat4 KvCoord::axisInverseMatrix_() const
{
	const static float_t diag[] = { 1, -1 }; // invMat��ĶԽ���ֵ

	return {
		diag[inv_[0]], 0, 0, inv_[0] ? lower().x() + upper().x() : 0,
		0, diag[inv_[1]], 0, inv_[1] ? lower().y() + upper().y() : 0,
		0, 0, diag[inv_[2]], inv_[2] ? lower().z() + upper().z() : 0,
		0, 0, 0, 1
	};
}


const KvCoord::mat4& KvCoord::axisSwapMatrix_() const
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


KvCoord::mat4 KvCoord::localMatrix() const
{
	// TODO: �ȷ�ת�����Ƚ�����
	return swapStatus_ ? axisSwapMatrix_() * axisInverseMatrix_() : axisInverseMatrix_(); 
}


void KvCoord::inverseAxis(int dim, bool inv)
{
	inv_[dim] = inv;

	forAxis([dim, inv](KcAxis& axis) {
		if (axis.dim() == dim && axis.main())
			axis.setInversed(inv);
		return true;
		});
}


namespace kPrivate
{
	static void swapAxis(KcAxis& axis, KvCoord::KeAxisSwapStatus status);
}


void KvCoord::swapAxis(KeAxisSwapStatus status)
{
	if (swapStatus_ == status)
		return;

	// �Ȼָ��ѽ�����������
	if (swapStatus_ != k_axis_swap_none) {
		forAxis([this](KcAxis& axis) {
			//kPrivate::swapAxis(axis, swapStatus_);
			return true;
			});
	}

	// �������status��������������
	if (status != k_axis_swap_none) {
		forAxis([status](KcAxis& axis) {
			//kPrivate::swapAxis(axis, status);
			return true;
			});
	}

	swapStatus_ = status;
}


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
		// ��������ͽ�������
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
		axis.setType(KcAxis::KeAxisType(t));
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
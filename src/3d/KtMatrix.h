#pragma once
#include "KtPoint.h"

namespace k3d
{

	template<class KReal, int ROWS, int COLS, bool ROW_MAJOR = true>
	class KtMatrix : public KtPoint<KReal, ROWS* COLS>
	{
		using super_ = KtPoint<KReal, ROWS* COLS>;

	public:

		constexpr static bool rowMajor() { return ROW_MAJOR; }

		constexpr static int location(int r, int c) {
			if constexpr (ROW_MAJOR)
				return r * COLS + c;
			else
				return c * ROWS + r;
		}

		KReal at(unsigned row, unsigned col) const {
			return super_::at(location(row, col));
		}
		KReal& at(unsigned row, unsigned col) {
			return super_::at(location(row, col));
		}

		KReal operator()(unsigned row, unsigned col) const { return at(row, col); }
		KReal& operator()(unsigned row, unsigned col) { return at(row, col); }

	};

}
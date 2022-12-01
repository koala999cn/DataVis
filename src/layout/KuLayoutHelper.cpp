#include "KuLayoutHelper.h"
#include "KcLayoutGrid.h"
#include "KcLayoutOverlay.h"


namespace kPrivate
{
	enum
	{
		k_side_left = 0x01,
		k_side_right = 0x02,
		k_side_top = 0x04,
		k_side_bottom = 0x08,

		k_side_backword = k_side_left | k_side_top,
		k_side_foreward = k_side_right | k_side_bottom,

		k_side_horz = k_side_left | k_side_right,
		k_side_vert = k_side_top | k_side_bottom
	};

	static bool placeSide(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side);
}


void KuLayoutHelper::take(KvLayoutElement* ele)
{
	auto parent = ele->parent();
	if (dynamic_cast<KcLayoutGrid*>(parent))
		dynamic_cast<KcLayoutGrid*>(parent)->take(ele);
	else if (dynamic_cast<KvLayoutContainer*>(parent))
		dynamic_cast<KvLayoutContainer*>(parent)->take(ele);
	else 
		ele->setParent(nullptr);
}


bool KuLayoutHelper::placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_left);
}


bool KuLayoutHelper::placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_right);
}


bool KuLayoutHelper::placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_top);
}


bool KuLayoutHelper::placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_bottom);
}


namespace kPrivate
{
	static unsigned stepBackward(KcLayout1d* lay1d, unsigned pos, unsigned dist)
	{
		assert(lay1d->getAt(pos));

		if (dist == -1)
			dist = pos;

		while (dist > 0 && pos != 0) {
			if (lay1d->getAt(--pos))
				dist--;
		}

		if (dist > 0) {
			assert(pos == 0);
			while (lay1d->getAt(++pos) == nullptr);
		}

		if (pos > 0 && lay1d->getAt(pos - 1) == nullptr)
			--pos;

		return pos;
	}

	static unsigned stepForeward(KcLayout1d* lay1d, unsigned pos, unsigned dist)
	{
		assert(lay1d->getAt(pos));

		if (dist == -1)
			dist = lay1d->size() - pos;

		// 确保pos至少执行一次加法
		while (++pos != lay1d->size() && dist > 0) {
			if (lay1d->getAt(pos))
				dist--;
		}

		if (dist > 0) {
			assert(pos == lay1d->size());
			while (lay1d->getAt(--pos) == nullptr);
			++pos;
		}

		return pos;
	}

	using pair_u = std::array<unsigned, 2>;

	// @dim: 0表示调整pos[0], 1表示调整pos[1]
	static pair_u stepBackward(KcLayoutGrid* grid, pair_u pos, unsigned dist, int dim)
	{
		assert(grid->getAt(pos[0], pos[1]));

		if (dist == -1)
			dist = pos[dim];

		while (dist > 0 && pos[dim] != 0) {
			--pos[dim];
			if (grid->getAt(pos[0], pos[1]))
				dist--;
		}

		if (dist > 0) {
			assert(pos[dim] == 0);
			++pos[dim];
			while (grid->getAt(pos[0], pos[1]) == nullptr)
				++pos[dim];
		}

		if (pos[dim] > 0 && grid->getAt(dim ? pos[0] : pos[0] - 1, dim ? pos[1] - 1 : pos[1]) == nullptr)
			--pos[dim];

		return pos;
	}

	static pair_u stepForeward(KcLayoutGrid* grid, pair_u pos, unsigned dist, int dim)
	{
		assert(grid->getAt(pos[0], pos[1]));

		unsigned sz[] = { grid->rows(), grid->cols() };
		if (dist == -1)
			dist = sz[dim] - pos[dim];

		// 确保pos至少执行一次加法
		while (++pos[dim] != sz[dim] && dist > 0) {
			if (grid->getAt(pos[0], pos[1]))
				dist--;
		}

		if (dist > 0) {
			assert(pos[dim] == sz[dim]);
			--pos[dim];
			while (grid->getAt(pos[0], pos[1]) == nullptr)
				--pos[dim];
			++pos[dim];
		}

		return pos;
	}

	static pair_u stepDist(KcLayoutGrid* grid, pair_u pos, unsigned dist, int side)
	{
		int dim = side & k_side_horz ? 1 : 0;
		if (side & k_side_foreward)
			pos = stepForeward(grid, pos, dist, dim);
		else
			pos = stepBackward(grid, pos, dist, dim);

		return pos;
	}

	bool placeSide(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side)
	{
		auto par = who->parent();
		assert(par);

		// 先尝试grid
		auto grid = dynamic_cast<KcLayoutGrid*>(par);
		assert(grid);
		//if (grid) {
			auto pos = grid->find(who); // 定位who的位置
			assert(pos.first != -1);

			std::array<unsigned, 2> arPos{ pos.first, pos.second };
			arPos = kPrivate::stepDist(grid, arPos, dist, side);
			if (arPos[0] < grid->rows() && arPos[1] < grid->cols() &&
				grid->getAt(arPos[0], arPos[1]) == nullptr)
				; // 有空余位置，不增加新行/新列
			else if (side & k_side_horz)
				grid->insertColAt(arPos[1]);
			else
				grid->insertRowAt(arPos[0]);

			assert(grid->getAt(arPos[0], arPos[1]) == nullptr);
			grid->putAt(arPos[0], arPos[1], ele);

			return true;
		//}

#if 0
		auto vect = dynamic_cast<KcLayout1d*>(par);
		if (vect) {

			auto idx = vect->find(who);

			if (vect->rowMajor() == bool(side & k_side_horz)) {
				auto pos = (side & k_side_backword) ? kPrivate::stepBackward(vect, idx, dist)
					: kPrivate::stepForeward(vect, idx, dist);
				vect->insertAt(pos, ele);
			}
			else { // 创建grid
				auto grid = makeGridFrom1d(vect);

				point2i pos;
				int dim = (side & k_side_horz) ? 0 : 1;
				pos[dim] = idx;
				pos[!dim] = (side & k_side_foreward) ? 1 : 0;

				grid->putAt(pos.x(), pos.y(), ele);
				substitute_(vect, grid);
			}

			return true;
		}
#endif

		return false;
	}

#if 0
	static KcLayoutGrid* makeGridFrom1d(KcLayout1d* v)
	{
		auto grid = new KcLayoutGrid(v->parent());
		int dim = v->rowMajor() ? 1 : 0;
		point2i szGrid;
		szGrid[dim] = v->size(), szGrid[!dim] = 1;
		grid->resize(szGrid[0], szGrid[1]);

		point2i pos(0);
		for (; pos[dim] < v->size(); pos[dim]++) {
			auto e = v->takeAt(pos[dim]);
			grid->setAt(pos[0], pos[1], e);
			assert(e == nullptr || e->parent() == grid);
		}

		return grid;
	}

	// 递归帮助函数
	// 在container中递归查找who，并将其替换为ele
	// 成功返回true
	static bool substitute(KvLayoutContainer* container, KvLayoutElement* who, KvLayoutElement* ele)
	{
		for (unsigned i = 0; i < container->size(); i++) {
			auto e = container->getAt(i);
			if (e == nullptr)
				continue;
			
			if (e == who) { // bingo
				container->setAt(i, ele);
				return true;
			}

			auto v = dynamic_cast<KvLayoutContainer*>(e);
			if (v && substitute(v, who, ele)) // 递归查找
				return true;
		}

		return false;
	}
#endif
}

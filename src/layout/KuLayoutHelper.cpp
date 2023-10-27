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

	static void placeSide(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side);
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


void KuLayoutHelper::placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_left);
}


void KuLayoutHelper::placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_right);
}


void KuLayoutHelper::placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_top);
}


void KuLayoutHelper::placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return kPrivate::placeSide(who, ele, dist, kPrivate::k_side_bottom);
}


void KuLayoutHelper::align(KcLayoutOverlay* target, KvLayoutElement* ele)
{
	auto loc = ele->align();

	assert(!target->isAncestorOf(ele));

	if (loc.inner() || target->parent() == nullptr) {
		target->insert(ele);
	}
	else {
		if (loc & KeAlignment::k_horz_first) {
			if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(target, ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(target, ele, -1);
			else if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(target, ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(target, ele, -1);
			else
				assert(false);
		}
		else {
			if (loc & KeAlignment::k_top)
				KuLayoutHelper::placeTop(target, ele, -1);
			else if (loc & KeAlignment::k_bottom)
				KuLayoutHelper::placeBottom(target, ele, -1);
			else if (loc & KeAlignment::k_left)
				KuLayoutHelper::placeLeft(target, ele, -1);
			else if (loc & KeAlignment::k_right)
				KuLayoutHelper::placeRight(target, ele, -1);
			else
				assert(false);
		}
	}
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

	void placeSide(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side)
	{
		auto par = who->parent();
		assert(par);

		// 先尝试grid
		auto grid = dynamic_cast<KcLayoutGrid*>(par);
		assert(grid);
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
	}
}

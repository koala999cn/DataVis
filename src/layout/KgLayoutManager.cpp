#include "KgLayoutManager.h"


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

	KcLayoutGrid* createGridFromVector(KcLayoutVector* v)
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

	// 返回从vect的位置pos开始倒退dist个有效元素（非nullptr）的位置
	unsigned stepBackward(KcLayoutVector* vect, unsigned pos, unsigned dist)
	{
		if (dist == -1)
			dist = pos;

		while (dist > 0 && pos != 0) {
			if (vect->getAt(--pos))
				dist--;
		}

		return pos;
	}

	unsigned stepForeward(KcLayoutVector* vect, unsigned pos, unsigned dist)
	{
		if (dist == -1)
			dist = vect->size() - pos;

		// 确保pos至少执行一次加法
		while (++pos != vect->size() && dist > 0) {
			if (vect->getAt(pos))
				dist--;
		}

		return pos;
	}

	using pair_u = std::pair<unsigned, unsigned>;
	pair_u stepDist(KcLayoutGrid* grid, pair_u pos, unsigned dist, int side)
	{
		if (side & k_side_horz) {
			if (side & k_side_foreward)
				pos.second = stepForeward(grid->rowAt(pos.first), pos.second, dist);
			else 
				pos.second = stepBackward(grid->rowAt(pos.first), pos.second, dist);
		}
		else {
			if (side & k_side_foreward) {
				if (dist == -1)
					dist = grid->rows() - pos.first;

				while (++pos.first != grid->rows() && dist > 0) {
					if (grid->getAt(pos.first, pos.second))
						dist--;
				}
			}
			else {
				if (dist == -1)
					dist = pos.first;

				while (dist > 0 && pos.first != 0) {
					if (grid->getAt(--pos.first, pos.second))
						dist--;
				}
			}
		}

		return pos;
	}

}


void KgLayoutManager::setRoot(KvLayoutElement* ele)
{
	root_.reset(ele);
}


void KgLayoutManager::take(KvLayoutElement* ele)
{
	auto parent = ele->parent();
	if (parent == nullptr) {
		if (root_.get() == ele)
			root_.release();
	}
	else if (dynamic_cast<KcLayoutGrid*>(parent))
		dynamic_cast<KcLayoutGrid*>(parent)->take(ele);
	else if (dynamic_cast<KcLayoutVector*>(parent))
		dynamic_cast<KcLayoutVector*>(parent)->take(ele);
	else 
		ele->setParent(nullptr);
}


bool KgLayoutManager::placeSide_(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side)
{
	using namespace kPrivate;

	auto par = who->parent();
	if (par == nullptr) {
		assert(who == root_.get());
		auto vect = new KcLayoutVector;
		vect->setRowMajor(side & k_side_horz);
		vect->resize(2);
		int whoPos = side & k_side_backword;
		vect->setAt(whoPos, who);
		vect->setAt(!whoPos, ele);	
		root_.release();
		root_.reset(vect);
		return true;
	}

	// 先尝试grid
	auto grid = dynamic_cast<KcLayoutGrid*>(par);
	if (grid) {
		auto pos = grid->find(who); // 定位who的位置
		assert(pos.first != -1);

		pos = kPrivate::stepDist(grid, pos, dist, side);
		if (pos.first < grid->rows() && pos.second < grid->cols() &&
			grid->getAt(pos.first, pos.second) == nullptr)
			; // 有空余位置，不增加新行/新列
		else if (side & k_side_horz)
			grid->insertColAt(pos.second);
		else 
			grid->insertRowAt(pos.first);

		assert(grid->getAt(pos.first, pos.second) == nullptr);
		grid->putAt(pos.first, pos.second, ele);

		return true;
	}

	auto vect = dynamic_cast<KcLayoutVector*>(par);
	if (vect) {

		auto idx = vect->find(who);

		if (vect->rowMajor() == bool(side & k_side_horz)) {
			auto pos = (side & k_side_backword) ? kPrivate::stepBackward(vect, idx, dist)
				                                : kPrivate::stepForeward(vect, idx, dist);
			vect->insertAt(pos, ele);
		}
		else { // 创建grid
			auto grid = kPrivate::createGridFromVector(vect);

			point2i pos;
			int dim = (side & k_side_horz) ? 0 : 1;
			pos[dim] = idx;
			pos[!dim] = (side & k_side_foreward) ? 1 : 0;

			grid->putAt(pos.x(), pos.y(), ele);
			substitute_(vect, grid);
		}

		return true;
	}

	return false;
}


bool KgLayoutManager::placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return placeSide_(who, ele, dist, kPrivate::k_side_left);
}


bool KgLayoutManager::placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return placeSide_(who, ele, dist, kPrivate::k_side_right);
}


bool KgLayoutManager::placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return placeSide_(who, ele, dist, kPrivate::k_side_top);
}


bool KgLayoutManager::placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	return placeSide_(who, ele, dist, kPrivate::k_side_bottom);
}


namespace kPrivate
{
	// 递归帮助函数
	// 在container中递归查找who，并将其替换为ele
	// 成功返回true
	static bool substitute_(KcLayoutVector* container, KvLayoutElement* who, KvLayoutElement* ele)
	{
		for (unsigned i = 0; i < container->size(); i++) {
			auto e = container->getAt(i);
			if (e == nullptr)
				continue;
			
			if (e == who) { // bingo
				container->setAt(i, ele);
				return true;
			}

			auto v = dynamic_cast<KcLayoutVector*>(e);
			if (v && substitute_(v, who, ele)) // 递归查找
				return true;
		}

		return false;
	}
}


bool KgLayoutManager::substitute_(KvLayoutElement* who, KvLayoutElement* ele)
{
	if (root_.get() == who) {
		assert(ele->parent() == nullptr);
		root_.reset(ele);
		return true;
	}

	auto container = dynamic_cast<KcLayoutVector*>(root_.get());
	if (container)
		return kPrivate::substitute_(container, who, ele);

	return false;
}

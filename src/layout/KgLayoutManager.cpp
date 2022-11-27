#include "KgLayoutManager.h"


namespace kPrivate
{
	KcLayoutGrid* createGridFromVector(KcLayoutVector* v)
	{
		auto grid = new KcLayoutGrid(v->parent());

		if (v->rowMajor()) {
			grid->resize(v->size(), 1);
			for (unsigned i = 0; i < v->size(); i++) {
				auto e = v->takeAt(0);
				grid->setAt(i, 0, e);
			}
		}
		else {
			grid->resize(1, v->size());
			for (unsigned i = 0; i < v->size(); i++) {
				auto e = v->takeAt(0);
				grid->setAt(0, i, e);
			}
		}

		return grid;
	}

	// ���ش�vect��λ��pos��ʼ����dist����ЧԪ�أ���nullptr����λ��
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

		while (dist > 0 && ++pos != vect->size()) {
			if (vect->getAt(pos))
				dist--;
		}

		return pos;
	}
}


bool KgLayoutManager::placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist)
{
	auto p = who->parent();
	if (p == nullptr) {
		assert(who == root_.get());
		auto v = new KcLayoutVector;
		v->setRowMajor(true);
		v->resize(2);
		v->setAt(0, ele);
		v->setAt(1, who);
		root_.reset(v);
		return true;
	}

	// �ȳ���grid
	auto grid = dynamic_cast<KcLayoutGrid*>(p);
	if (grid) {
		auto pos = grid->find(who); // ��λwho��λ��
		assert(pos.first != -1);

		pos.second = kPrivate::stepBackward(grid->rowAt(pos.first), pos.second, dist);
		grid->insertAt(pos.first, pos.second, ele);
		return true;
	}

	auto vect = dynamic_cast<KcLayoutVector*>(p);
	if (vect) {

		auto idx = vect->find(who);

		if (vect->rowMajor()) {
			auto pos = kPrivate::stepBackward(vect, idx, dist);
			vect->insertAt(pos, ele);
		}
		else { // ����grid
			auto grid = kPrivate::createGridFromVector(vect);
			grid->insertAt(idx, 0, ele);
			substitute_(vect, grid);
		}

		return true;
	}

	return false;
}


namespace kPrivate
{
	// �ݹ��������
	// ��container�еݹ����who���������滻Ϊele
	// �ɹ�����true
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
			if (v && substitute_(v, who, ele)) // �ݹ����
				return true;
		}

		return false;
	}
}


bool KgLayoutManager::substitute_(KvLayoutElement* who, KvLayoutElement* ele)
{
	if (root_.get() == who) {
		root_.reset(ele);
		return true;
	}

	auto container = dynamic_cast<KcLayoutVector*>(root_.get());
	if (container)
		return kPrivate::substitute_(container, who, ele);

	return false;
}

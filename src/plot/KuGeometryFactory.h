#pragma once
#include <memory>
#include "KtGeometry.h"
#include "KtAABB.h"


class KuGeometryFactory
{
public:
	template<typename VT, typename IT>
	using geom_ptr = std::shared_ptr<KtGeometry<VT, IT>>;

	template<typename VT>
	using point3 = KtPoint<VT, 3>;


	template<typename VT, typename IT = unsigned, bool CCW = false>
	geom_ptr<VT, IT> makeBox(const KtAABB<VT>& aabb);

	template<typename VT, typename IT = unsigned, bool CCW = false>
	geom_ptr<VT, IT> makeBox(const point3<VT>& lower, const point3<VT>& upper) {
		return makeBox(KtAABB<VT>(lower, upper));
	}

	template<typename VT, typename IT = unsigned, bool CCW = false>
	geom_ptr<VT, IT> makeBox() {
		return makeBox({ 0, 0, 0 }, { 1, 1, 1 });
	}

private:
	KuGeometryFactory() = delete;
};


template<typename VT, typename IT, bool CCW> KuGeometryFactory::geom_ptr<VT, IT>
	KuGeometryFactory::makeBox(const KtAABB<VT>& aabb)
{
	auto corns = aabb.allCorners();

	auto geom = std::make_shared<KtGeometry<VT, IT>>();
	geom->reserve(8, 36);
	

	/*
		1-----2
	   /|    /|
	  / |   / |
	 5-----4  |
	 |  0--|--3
	 | /   | /
	 |/    |/
	 6-----7
	*/

	for(auto& i : const) geom->pushVertex(i);

	if constexpr (CCW) {
		geom->pushIndex(3); geom->pushIndex(0); geom->pushIndex(1);
		geom->pushIndex(1); geom->pushIndex(2); geom->pushIndex(3);

		geom->pushIndex(7); geom->pushIndex(6); geom->pushIndex(0);
		geom->pushIndex(0); geom->pushIndex(3); geom->pushIndex(7);

		geom->pushIndex(4); geom->pushIndex(5); geom->pushIndex(6);
		geom->pushIndex(6); geom->pushIndex(7); geom->pushIndex(4);

		geom->pushIndex(2); geom->pushIndex(1); geom->pushIndex(5);
		geom->pushIndex(5); geom->pushIndex(4); geom->pushIndex(2);

		geom->pushIndex(7); geom->pushIndex(3); geom->pushIndex(2);
		geom->pushIndex(2); geom->pushIndex(4); geom->pushIndex(7);

		geom->pushIndex(0); geom->pushIndex(6); geom->pushIndex(5);
		geom->pushIndex(5); geom->pushIndex(1); geom->pushIndex(0);
	}
	else {
		geom->pushIndex(1); geom->pushIndex(0); geom->pushIndex(3);
		geom->pushIndex(3); geom->pushIndex(2); geom->pushIndex(1);

		geom->pushIndex(0); geom->pushIndex(6); geom->pushIndex(7);
		geom->pushIndex(7); geom->pushIndex(3); geom->pushIndex(0);

		geom->pushIndex(6); geom->pushIndex(5); geom->pushIndex(4);
		geom->pushIndex(4); geom->pushIndex(7); geom->pushIndex(6);

		geom->pushIndex(5); geom->pushIndex(1); geom->pushIndex(2);
		geom->pushIndex(2); geom->pushIndex(4); geom->pushIndex(5);

		geom->pushIndex(2); geom->pushIndex(3); geom->pushIndex(7);
		geom->pushIndex(7); geom->pushIndex(4); geom->pushIndex(2);

		geom->pushIndex(5); geom->pushIndex(6); geom->pushIndex(0);
		geom->pushIndex(0); geom->pushIndex(1); geom->pushIndex(5);
	}
}

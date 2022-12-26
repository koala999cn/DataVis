#pragma once
#include <array>
#include "KtAABB.h"


// 生成基本的3d构型

class KuPrimitiveFactory
{
public:
	using point3 = std::array<double, 3>;

	enum KeType
	{
		k_position,
		k_normal,
		k_texcoord,

		k_edge_index,
		k_mesh_index
	};


	template<int type, typename T = float, bool CCW = false>
	static int makeBox(const point3& lower, const point3& upper, void* obuf, unsigned stride = 0);


private:
	KuPrimitiveFactory() = delete;
};


template<int TYPE, typename T, bool CCW>
int KuPrimitiveFactory::makeBox(const point3& lower, const point3& upper, void* obuf, unsigned stride)
{
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

	if constexpr (CCW) {
		static constexpr T MESH_IDX[] = {
			3, 0, 1, 1, 2, 3,
			7, 6, 0, 0, 3, 7,
			4, 5, 6, 6, 7, 4,
			2, 1, 5, 5, 4, 2,
			7, 3, 2, 2, 4, 7,
			0, 6, 5, 5, 1, 0
		};
	}
	else {
		static constexpr T MESH_IDX[] = {
			1, 0, 3, 3, 2, 1,
			0, 6, 7, 7, 3, 0,
			6, 5, 4, 4, 7, 6,
			5, 1, 2, 2, 4, 5,
			2, 3, 7, 7, 4, 2,
			5, 6, 0, 0, 1, 5
		};
	}

	static constexpr T EDGE_IDX[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		1, 5, 2, 4, 0, 6, 3, 7
	};

	if constexpr (TYPE == k_mesh_index) {
		assert(stride == 0 || stride == sizeof(T));

		if (obuf)
			std::copy(std::cbegin(MESH_IDX), std::cend(MESH_IDX), (T*)obuf);

		return std::size(MESH_IDX);
	}
	else if constexpr (TYPE == k_edge_index) {
		assert(stride == 0 || stride == sizeof(T));

		if (obuf)
			std::copy(std::cbegin(EDGE_IDX), std::cend(EDGE_IDX), (T*)obuf);

		return std::size(EDGE_IDX);
	}
	else {
		if (obuf == nullptr)
			return corns.size();

		char* buf = (char*)obuf;

		if constexpr (type == k_position) {
			auto aabb = KtAABB<T, 3>(lower, upper);
			auto corns = aabb.allCorners();
			constexpr int point_size = sizeof(T) * 3;
			assert(sizeof(corns[0]) == point_size);

			if (stride == 0)
				stride = point_size;

			for (unsigned i = 0; i < corns.size(); i++) {
				auto pt = (std::array<T, 3>*)buf;
				*pt = corns[i];
				buf += stride;
			}
		}
		else if constexpr (TYPE == k_normal) {
			auto aabb = KtAABB<T, 3>(lower, upper);
			auto corns = aabb.allCorners();
			constexpr int point_size = sizeof(T) * 3;
			assert(sizeof(corns[0]) == point_size);

			if (stride == 0)
				stride = point_size;

			static constexpr char diag[] = {
				4, 7, 6, 5, 0, 3, 2, 1
			};

			for (unsigned i = 0; i < corns.size(); i++) {
				auto pt = (std::array<T, 3>*)buf;
				*pt = corns[i] - corns[diag[i]];
				buf += stride;
			}
		}
		else if consteval (TYPE == k_texcoord) {
			static_assert(false, "TODO");
		}
		else {
			static_assert(false, "unknown type");
		}

		return corns.size();
	}
}

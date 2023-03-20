#pragma once
#include <array>
#include "KtAABB.h"
#include "KePrimitiveType.h"


// 生成基本的3d构型

class KuPrimitiveFactory
{
public:

	template<typename T>
	using point2 = std::array<T, 2>;

	template<typename T>
	using point3 = std::array<T, 3>;

	static inline const double SQRT_2_2 = std::sqrt(2.) / 2.;
	static inline const double SQRT_3_2 = std::sqrt(3.) / 2.;


	// 生成box的8个顶点
	// @T: 顶点的基本数据类型，每个顶点位置由T[3]构成
	// @obuf: 输出缓存，可为null
	// @stride: obuf中每个顶点位置的字节跨度. =0表示连续取址
	// 返回生成的顶点数
	template<typename T>
	static int makeBox(const point3<T>& lower, const point3<T>& upper, void* obuf, unsigned stride = 0);

	// 生成box的quads索引，返回生成的索引数
	// @obuf: 输出缓存，可为null
	template<typename IDX_TYPE = std::uint32_t, bool CCW = false>
	static int indexBox(IDX_TYPE* obuf);

	// 生成grid的quads索引，返回生成的索引数
	// @obuf: 输出缓存，可为null
	// 
	// grid由nx*ny个顶点构成，顶点排列存储顺序如下：
	// 0,  1,      ..., nx-1,
	// nx, nx + 1, ..., 2*nx-1,
	// ...
	// (ny-1)*nx, ..., nx*ny-1
	//
	template<typename IDX_TYPE = std::uint32_t, bool CCW = false>
	static int indexGrid(unsigned nx, unsigned ny, IDX_TYPE* obuf, unsigned startVtx = 0, unsigned idxBase = 0);

	// 构建10点circle
	template<typename T>
	static int makeCircle10(const point3<T>& center, T radius, void* obuf, unsigned stride = 0);

	// 返回单位长度（高为1）的上三角形的3个顶点，中心点为(0, 0)
	template<typename T>
	static const point2<T>* triangleUp() {
		static const point2<T> up[] = {
			{ T(KuMath::sqrt3 / 2), 0.5f }, { 0, -1 }, { -T(KuMath::sqrt3 / 2), 0.5f }
		};
		return up;
	}

	template<typename T>
	static const point2<T>* triangleDown() {
		static const point2<T> down[] = {
			{ T(KuMath::sqrt3 / 2), -0.5f }, { 0, 1 }, { -T(KuMath::sqrt3 / 2), -0.5f }
		};
		return down;
	}

	template<typename T>
	static const point2<T>* triangleLeft() {
		static const point2<T> left[] = {
			{ -1, 0 }, { 0.5, T(KuMath::sqrt3 / 2) }, { 0.5, -T(KuMath::sqrt3 / 2) }
		};
		return left;
	}

	template<typename T>
	static const point2<T>* triangleRight() {
		static const point2<T> right[] = {
			{ 1, 0 }, { -0.5, T(KuMath::sqrt3 / 2) }, { -0.5, -T(KuMath::sqrt3 / 2) }
		};
		return right;
	}

	template<typename T>
	static const point2<T>* diamond() {
		static const point2<T> vtx[] = {
			{ 1, 0 }, { 0, -1 }, { -1, 0 }, { 0, 1 }
		};
		return vtx;
	}

	template<typename T>
	static const point2<T>* square() {
		static const point2<T> vtx[] = {
			{ T(KuMath::sqrt2 / 2), T(KuMath::sqrt2 / 2) }, { T(KuMath::sqrt2 / 2), -T(KuMath::sqrt2 / 2) },
			{ -T(KuMath::sqrt2 / 2), -T(KuMath::sqrt2 / 2) }, { -T(KuMath::sqrt2 / 2), T(KuMath::sqrt2 / 2) }
		};
		return vtx;
	}

	template<typename T>
	static const point2<T>* circle10() {
		static constexpr point2<T> vtx[] = {
			{ 1.0f, 0.0f },
			{ 0.809017f, 0.58778524f },
			{ 0.30901697f, 0.95105654f },
			{ -0.30901703f, 0.9510565f },
			{ -0.80901706f, 0.5877852f },
			{ -1.0f, 0.0f },
			{ -0.80901694f, -0.58778536f },
			{ -0.3090171f, -0.9510565f },
			{ 0.30901712f, -0.9510565f },
			{ 0.80901694f, -0.5877853f }
		};
		return vtx;
	}

private:
	KuPrimitiveFactory() = delete;
};


template<typename T>
int KuPrimitiveFactory::makeBox(const point3<T>& lower, const point3<T>& upper, void* obuf, unsigned stride)
{
	constexpr int vtxCount = 8;

	if (obuf) {
		auto aabb = KtAABB<T>(KtPoint<T, 3>(lower.data()), KtPoint<T, 3>(upper.data()));
		auto corns = aabb.allCorners();
		assert(corns.size() == vtxCount);

		constexpr int point_size = sizeof(T) * 3;
		assert(sizeof(corns[0]) == point_size);

		if (stride == 0)
			stride = point_size;

		char* buf = (char*)obuf;

		for (unsigned i = 0; i < vtxCount; i++) {
			auto pt = (KtPoint<T, 3>*)buf;
			*pt = corns[i];
			buf += stride;
		}
	}

	return vtxCount;
}


template<typename IDX_TYPE, bool CCW>
int KuPrimitiveFactory::indexBox(IDX_TYPE* obuf)
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

	int indexCount(0);

	const IDX_TYPE* MESH_IDX;

	if constexpr (CCW) {
		static constexpr IDX_TYPE TRI_IDX_[] = {
			3, 0, 1, 1, 2, 3,
			7, 6, 0, 0, 3, 7,
			4, 5, 6, 6, 7, 4,
			2, 1, 5, 5, 4, 2,
			7, 3, 2, 2, 4, 7,
			0, 6, 5, 5, 1, 0
		};

		static constexpr IDX_TYPE QUAD_IDX_[] = {
			0, 1, 2, 3,
			0, 6, 5, 1,
			0, 3, 7, 6,
			1, 5, 4, 2,
			2, 4, 7, 3,
			4, 5, 6, 7
		};

		indexCount = std::size(QUAD_IDX_);
		MESH_IDX = QUAD_IDX_;
	}
	else {
		static constexpr IDX_TYPE TRI_IDX_[] = {
			1, 0, 3, 3, 2, 1,
			0, 6, 7, 7, 3, 0,
			6, 5, 4, 4, 7, 6,
			5, 1, 2, 2, 4, 5,
			2, 3, 7, 7, 4, 2,
			5, 6, 0, 0, 1, 5
		};

		static constexpr IDX_TYPE QUAD_IDX_[] = {
			3, 2, 1, 0,
			1, 5, 6, 0,
			6, 7, 3, 0,
			2, 4, 5, 1,
			3, 7, 4, 2,
			7, 6, 5, 4
		};

		indexCount = std::size(QUAD_IDX_);
		MESH_IDX = QUAD_IDX_;
	}

	if (obuf) {
		for (int i = 0; i < indexCount; i++)
			*obuf++ = MESH_IDX[i];
	}

	return indexCount;
}


template<typename IDX_TYPE, bool CCW>
int KuPrimitiveFactory::indexGrid(unsigned nx, unsigned ny, IDX_TYPE* obuf, unsigned startVtx, unsigned idxBase)
{
	assert(startVtx < 4);

	int indexCount = (nx - 1) * (ny - 1) * 4; // 构成grid的quads数量

	if (obuf) {

		unsigned offset[] = {
			idxBase,
			idxBase,
			idxBase,
			idxBase,
		};

		if (startVtx == 1) {
			offset[0] += nx;
			offset[1] += 1;
			offset[2] -= nx;
			offset[3] -= 1;
		}
		else if (startVtx == 2) {
			offset[0] += nx + 1;
			offset[1] -= nx - 1;
			offset[2] -= nx + 1;
			offset[3] += nx - 1;
		}
		else if (startVtx == 3) {
			offset[0] += 1;
			offset[1] -= nx;
			offset[2] -= 1;
			offset[3] += nx;
		}

		for(unsigned i = 1; i < nx; i++)
			for (unsigned j = 1; j < ny; j++) {

				if constexpr (!CCW) {
					*obuf++ = static_cast<IDX_TYPE>((i - 1) + (j - 1) * nx + offset[0]);
					*obuf++ = static_cast<IDX_TYPE>((i - 1) + j * nx + offset[1]);
					*obuf++ = static_cast<IDX_TYPE>(i + j * nx + offset[2]);
					*obuf++ = static_cast<IDX_TYPE>(i + (j - 1) * nx + offset[3]);
				}
				else {
					*obuf++ = static_cast<IDX_TYPE>((i - 1) + (j - 1) * nx + offset[0]);
					*obuf++ = static_cast<IDX_TYPE>(i + (j - 1) * nx + offset[3]);
					*obuf++ = static_cast<IDX_TYPE>(i + j * nx + offset[2]);
					*obuf++ = static_cast<IDX_TYPE>((i - 1) + j * nx + offset[1]);
				}

			}
	}

	return indexCount;
}


template<typename T>
int KuPrimitiveFactory::makeCircle10(const point3<T>& center, T radius, void* obuf, unsigned stride)
{
	constexpr int vtxCount = 10;

	if (obuf) {

		if (stride == 0)
			stride = sizeof(T) * 3;

		char* buf = (char*)obuf;

		for (unsigned i = 0; i < vtxCount; i++) {
			auto pt = (T*)buf;
			for (int j = 0; j < 2; j++)
			    pt[j] = center[j] + radius * circle10<T>()[i][j];
			pt[2] = center[2]; // 始终在x-y平面构建circle，z坐标与原点相同
			buf += stride;
		}
	}

	return vtxCount;
}
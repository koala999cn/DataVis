#pragma once
#include <array>
#include "KtAABB.h"
#include "KePrimitiveType.h"


// ���ɻ�����3d����

class KuPrimitiveFactory
{
public:

	using point3 = std::array<double, 3>;

	// ����box��8������
	// @T: ����Ļ����������ͣ�ÿ������λ����T[3]����
	// @obuf: ������棬��Ϊnull
	// @stride: obuf��ÿ������λ�õ��ֽڿ��. =0��ʾ����ȡַ
	// �������ɵĶ�����
	template<typename T>
	static int makeBox(const point3& lower, const point3& upper, void* obuf, unsigned stride = 0);

	// ����box��quads�������������ɵ�������
	// @obuf: ������棬��Ϊnull
	template<typename IDX_TYPE = unsigned, bool CCW = false>
	static int indexBox(IDX_TYPE* obuf);

	// ����grid��quads�������������ɵ�������
	// @obuf: ������棬��Ϊnull
	// 
	// grid��nx*ny�����㹹�ɣ��������д洢˳�����£�
	// 0,  1,      ..., ny-1,
	// ny, ny + 1, ..., 2*ny-1,
	// ...
	// (nx-1)*ny, ..., nx*ny-1
	//
	template<typename IDX_TYPE = unsigned, bool CCW = false>
	static int indexGrid(unsigned nx, unsigned ny, IDX_TYPE* obuf);

private:
	KuPrimitiveFactory() = delete;
};


template<typename T>
int KuPrimitiveFactory::makeBox(const point3& lower, const point3& upper, void* obuf, unsigned stride)
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

		for (unsigned i = 0; i < corns.size(); i++) {
			auto pt = (std::array<T, 3>*)buf;
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
int KuPrimitiveFactory::indexGrid(unsigned nx, unsigned ny, IDX_TYPE* obuf)
{
	int indexCount = (nx - 1) * (ny - 1) * 4; // ����grid��quads����

	if (obuf) {
		for(unsigned i = 1; i < nx; i++)
			for (unsigned j = 1; j < ny; j++) {

				if constexpr (!CCW) {
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + (j - 1));
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + j);
					*obuf++ = static_cast<IDX_TYPE>(i * ny + j);
					*obuf++ = static_cast<IDX_TYPE>(i * ny + (j - 1));
				}
				else {
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + (j - 1));
					*obuf++ = static_cast<IDX_TYPE>(i * ny + (j - 1));
					*obuf++ = static_cast<IDX_TYPE>(i * ny + j);
					*obuf++ = static_cast<IDX_TYPE>((i - 1) * ny + j);	
				}

			}
	}

	return indexCount;
}

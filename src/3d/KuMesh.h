#pragma once
#include "KtVector3.h"


class KuMesh
{
public:

	template<typename T>
	using point3 = KtPoint<T, 3>;

	template<typename T>
	using vec3 = KtVector3<T>;

	// 计算三点构成平面的法线，其中v1, v2, v3按逆时针排列
	template<typename T>
	static vec3<T> normal(const point3<T>& v1, const point3<T>& v2, const point3<T>& v3) {
		return (v2 - v1).cross(v3 - v2).normalize();
	}

	// Generate flat normals. All vertices in each triangle face get the same normal vector.
	// @vtx: 三角形各顶点的位置, 尺寸=3*@ntris
	// @normals: 计算的法线结果, 尺寸=3*@ntris
	// @ntris: 三角形的数量
	template<typename T>
	static void generateNormalsFlat(const point3<T>* vtx, vec3<T>* normals, unsigned ntris) {
		for (unsigned i = 0; i < 3 * ntris; i += 3)
			normals[i] = normals[i + 1] = normals[i + 2] = normal(vtx[i], vtx[i + 1], vtx[i + 2]);
	}


	// Generate smooth normals
	// Uses @indices array to discover adjacent triangles and then for each
	// vertex position calculates a normal averaged from all triangles that share it.
	// The normal is weighted according to adjacent triangle areaand angle at given
	// vertex; hard edges are preserved where adjacent triangles don't share vertices.
	// Triangles with zero area or triangles containing invalid positions(NaNs) don't
	// contribute to calculated vertex normals.
	// 实现参考Magnum，算法见[Weighted Vertex Normals](http://www.bytehazard.com/articles/vertnorm.html) by Martijn Buijs.
	// @indices: Triangle face indices, 尺寸=3*@ntris
	// @vtx: Triangle vertex positions, 尺寸=nvtx
	// @normals: 计算的各顶点法线结果, 尺寸=nvtx
	template<typename T, typename I>
	static void generateNormalsTri(const I* idx, unsigned ntris, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx);


	// 同generateNormalsTri，但计算的是四边形的各顶点法线
	// @indices: Quadrangle face indices, 尺寸=4*@nquads
    // @vtx: Quadrangle vertex positions, 尺寸=nvtx
    // @normals: 计算的各顶点法线结果, 尺寸=nvtx
    // TODO:
	//template<typename T, typename I>
	//static void generateNormalsQuad(const I* indices, unsigned nquads, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx);

private:
	KuMesh() = delete;
};


template<typename T, typename I>
void KuMesh::generateNormalsTri(const I* indices, unsigned ntris, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx)
{
    /* Gather count of triangles for every vertex. This abuses the output
       storage to avoid extra allocations, zero-initialize it first to avoid
       random memory getting used. */
    std::vector<unsigned> triangleCount(nvtx);
    std::fill(triangleCount.begin(), triangleCount.end(), 0);
    for (unsigned i = 0; i < ntris * 3; i++)
        ++triangleCount[i];

    /* Turn that into a running offset array:
       triangleOffset[i + 1] - triangleOffset[i] is triangle count for vertex i
       triangleOffset[i] is offset into an triangle ID array for vertex i */
    std::vector<unsigned> triangleOffset(nvtx + 1);
    triangleOffset[0] = 0;
    for (std::size_t i = 0; i != triangleCount.size(); ++i)
        triangleOffset[i + 1] = triangleOffset[i] + triangleCount[i];

    assert(triangleOffset.back() == ntris * 3);

    /* Gather triangle IDs for every vertex. For vertex i,
       triangleIds[triangleOffset[i]] until triangleIds[triangleOffset[i + 1]]
       contains IDs of triangles that contain it. */
    std::vector<I> triangleIds(ntris * 3);
    for (std::size_t i = 0; i != ntris * 3; ++i) {
        const I triangleId = i / 3;
        const I vertexId = indices[i];

        /* How many triangle IDs is still left to be written, which also means
           the offset where we put the ID. Decrement that for the next run. */
        const std::size_t triangleIdsLeftForVertex = triangleCount[vertexId]--;
        triangleIds[triangleOffset[vertexId + 1] - triangleIdsLeftForVertex] = triangleId;
    }

    /* Now, triangleCount should be all zeros, we don't need it anymore and the
       underlying `normals` array is ready to get filled with real output. */

       /* Precalculate cross product and interior angles of each face --- the loop
          below would otherwise calculate it for every vertex, which is at least
          3x as much work */
    std::vector<std::pair<vec3<T>, vec3<T>>> crossAngles(ntris);
    for (std::size_t i = 0; i != crossAngles.size(); ++i) {
        auto v0 = vtx[indices[i * 3 + 0]];
        auto v1 = vtx[indices[i * 3 + 1]];
        auto v2 = vtx[indices[i * 3 + 2]];

        /* Cross product */
        crossAngles[i].first() = vec3(v2 - v1).cross(v0 - v1);

        /* If any of the vectors is zero, the normalization would result in a
           NaN and the angle calculation will assert. This happens also when
           any of the original positions is NaN. If that's the case, skip the
           rest. Given triangle will then contribute with a zero total angle,
           effectively getting ignored for normal calculation.

           If, however, an angle
           */
        auto v10n = (v1 - v0).normalize();
        auto v20n = (v2 - v0).normalize();
        auto v21n = (v2 - v1).normalize();
        if (v10n.isNan() || v20n.isNan() || v21n.isNan()) {
            crossAngles[i].second() = vec3<T>(0);
            continue;
        }

        /* Inner angle at each vertex of the triangle. The last one can be
           calculated as a remainder to 180°. */
        crossAngles[i].second()[0] = v10n.angle(v20n);
        crossAngles[i].second()[1] = (-v10n).angle(v21n);
        crossAngles[i].second()[2] = KuMath::pi
            - crossAngles[i].second()[0] - crossAngles[i].second()[1];
    }

    /* For every vertex v, calculate normals from all faces it belongs to and
       average them */
    for (std::size_t v = 0; v != nvtx; ++v) {
        /* normals are an external memory, ensure we accumulate from zero */
        normals[v] = vec3<T>(0);

        /* Go through all triangles sharing this vertex */
        for (std::size_t t = triangleOffset[v]; t != triangleOffset[v + 1]; ++t) {
            const std::size_t baseIndex = triangleIds[t] * 3;
            auto v0i = indices[baseIndex + 0];
            auto v1i = indices[baseIndex + 1];
            auto v2i = indices[baseIndex + 2];

            /* Cross product is a vector in direction of the normal with length
               equal to size of the parallelogram */
            const auto& crossAngle = crossAngles[triangleIds[t]];

            /* Angle between two sides of the triangle that share vertex `v`.
               The shared vertex can be one of the three. */
            T angle;
            if (v == v0i) angle = crossAngle.second()[0];
            else if (v == v1i) angle = crossAngle.second()[1];
            else if (v == v2i) angle = crossAngle.second()[2];
            else assert(false); /* LCOV_EXCL_LINE */

            /* The normal is cross.normalized(), we need to multiply it it by
               surface area which is cross.length()/2. Since normalization is
               division by length, multiplying it by length again will be a
               no-op. Then, since all normals are divided by 2, it doesn't
               change their ratio for the final normalization so we can omit
               that as well. Finally we need to weight by the angle, and in
               that case only the ratio is important as well, so it doesn't
               matter if degrees or radians. */
            normals[v] += crossAngle.first() * angle;
        }

        /* Normalize the accumulated direction */
        normals[v] = normals[v].normalize();
    }
}

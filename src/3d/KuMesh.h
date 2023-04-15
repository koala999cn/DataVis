#pragma once
#include "KtVector3.h"


class KuMesh
{
public:

	template<typename T> using point3 = KtPoint<T, 3>;
	template<typename T> using vec3 = KtVector3<T>;

	// 计算三点构成平面的法线
	template<typename T, bool CCW = false>
	static vec3<T> normal(const point3<T>& v1, const point3<T>& v2, const point3<T>& v3) {
        if constexpr (CCW)
		    return  vec3<T>(v2 - v1).cross(v3 - v1).normalize(); // v1, v2, v3按逆时针排列
        else 
            return vec3<T>(v3 - v1).cross(v2 - v1).normalize(); // v1, v2, v3按顺时针排列
	}

	// @vtx: 三角形各顶点的位置, 尺寸=@nvtx
	// @normals: 计算的法线结果, 尺寸=@nvtx
	// @nvtx: 顶点数量
	template<typename T, bool CCW = false>
	static void triNormalsFlat(const point3<T>* vtx, vec3<T>* normals, unsigned nvtx, 
        unsigned vstride = 0, unsigned nstride = 0) {
        return normalsFlat_<T, 3, CCW>(vtx, normals, nvtx / 3, vstride, nstride);
	}

    template<typename T, bool CCW = false>
    static void quadNormalsFlat(const point3<T>* vtx, vec3<T>* normals, unsigned nvtx,
        unsigned vstride = 0, unsigned nstride = 0) {
        return normalsFlat_<T, 4, CCW>(vtx, normals, nvtx / 4, vstride, nstride);
    }

    template<typename T, typename I, bool CCW = false>
    static void triNormalsAve(const I* idx, unsigned nidx, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx,
        unsigned vstride = 0, unsigned nstride = 0) {
        return normalsAve_<T, I, 3, CCW>(idx, nidx / 3, vtx, normals, nvtx, vstride, nstride);
    }

    template<typename T, typename I, bool CCW = false>
    static void quadNormalsAve(const I* idx, unsigned nidx, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx,
        unsigned vstride = 0, unsigned nstride = 0) {
        return normalsAve_<T, I, 4, CCW>(idx, nidx / 4, vtx, normals, nvtx, vstride, nstride);
    }

	// Generate smooth normals
	// Uses @idx array to discover adjacent triangles and then for each
	// vertex position calculates a normal averaged from all triangles that share it.
	// The normal is weighted according to adjacent triangle areaand angle at given
	// vertex; hard edges are preserved where adjacent triangles don't share vertices.
	// Triangles with zero area or triangles containing invalid positions(NaNs) don't
	// contribute to calculated vertex normals.
	// 实现参考Magnum，算法见 http://www.bytehazard.com/articles/vertnorm.html
	// @indices: Triangle face indices, 尺寸=3*@ntris
	// @vtx: Triangle vertex positions, 尺寸=nvtx
	// @normals: 计算的各顶点法线结果, 尺寸=nvtx
	template<typename T, typename I, bool CCW = false>
	static void triNormalsSmooth(const I* idx, unsigned ntris, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx);


	// 同generateNormalsTri，但计算的是四边形的各顶点法线
	// @idx: Quadrangle face indices, 尺寸=4*@nquads
    // @vtx: Quadrangle vertex positions, 尺寸=nvtx
    // @normals: 计算的各顶点法线结果, 尺寸=nvtx
    // TODO:
	//template<typename T, typename I>
	//static void generateQuadNormals(const I* idx, unsigned nquads, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx);

private:

    // Generate flat normals. All vertices in each N-face get the same normal vector.
    // @N: 构成face的顶点数
    // @vtx: 顶点位置, 尺寸=N*@nfaces
    // @normals: 计算的法线结果, 尺寸=N*@nfaces
    // @nfaces: N平面的数量
    template<typename T, int N, bool CCW = false>
    static void normalsFlat_(const point3<T>* vtx, vec3<T>* normals, unsigned nfaces,
        unsigned vstride = 0, unsigned nstride = 0);

    // 均值法计算N平面的顶点法线
    // @N: 构成平面的顶点数
    template<typename T, typename I, int N, bool CCW = false>
    static void normalsAve_(const I* idx, unsigned nfaces, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx,
        unsigned vstride = 0, unsigned nstride = 0);


private:
	KuMesh() = delete;
};


template<typename T, int N, bool CCW>
void KuMesh::normalsFlat_(const point3<T>* vtx, vec3<T>* normals, unsigned nfaces,
    unsigned vstride, unsigned nstride) {
    if (vstride == 0) vstride = sizeof(*vtx);
    if (nstride == 0) nstride = sizeof(*normals);

    auto pvtx = (const char*)vtx;
    auto pnorm = (char*)normals;
    for (unsigned i = 0; i < N * nfaces; i += N) {
        auto p0 = (const point3<T>*)(pvtx + i * vstride);
        auto p1 = (const point3<T>*)(pvtx + (i + 1) * vstride);
        auto p2 = (const point3<T>*)(pvtx + (i + 2) * vstride);
        auto n = normal<T, CCW>(*p0, *p1, *p2); // 取多边形的前3个点计算面法线
        for (unsigned j = 0; j < N; j++)
            *(vec3<T>*)(pnorm + j * nstride) = n;
    }
}


template<typename T, typename I, int N, bool CCW>
void KuMesh::normalsAve_(const I* idx, unsigned nfaces, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx,
    unsigned vstride, unsigned nstride)
{
    if (vstride == 0) vstride = sizeof(*vtx);
    if (nstride == 0) nstride = sizeof(*normals);
    vec3<T>* norm = normals;

    for (unsigned i = 0; i < nvtx; i++) {
        *norm = vec3<T>::zero();
        norm = (vec3<T>*)((char*)norm + nstride);
    }

    auto pvtx = (const char*)vtx;
    auto pnorm = (char*)normals;
    for (unsigned i = 0; i < N * nfaces; i += N) {
        auto p0 = (const point3<T>*)(pvtx + idx[i] * vstride);
        auto p1 = (const point3<T>*)(pvtx + idx[i + 1] * vstride);
        auto p2 = (const point3<T>*)(pvtx + idx[i + 2] * vstride);
        auto n = normal<T, CCW>(*p0, *p1, *p2); // 取多边形的前3个点计算面法线
        for (unsigned j = 0; j < N; j++)
            *(vec3<T>*)(pnorm + idx[i + j] * nstride) += n;
    }

    norm = normals;
    for (unsigned i = 0; i < nvtx; i++) {
        norm->normalize();
        norm = (vec3<T>*)((char*)norm + nstride);
    }
}


template<typename T, typename I, bool CCW>
void KuMesh::triNormalsSmooth(const I* idx, unsigned ntris, const point3<T>* vtx, vec3<T>* normals, unsigned nvtx)
{
    /* pseudo code
    for each face A in mesh
    {
     n = face A facet normal

     // loop through all vertices in face A
     for each vert in face A
     {
      for each face B in mesh
      {
       // ignore self
       if face A == face B then skip

       // criteria for hard-edges
       if face A and B smoothing groups match {

        // accumulate normal
        // v1, v2, v3 are the vertices of face A
        if face B shares v1 {
         angle = angle_between_vectors( v1 - v2 , v1 - v3 )
         n += (face B facet normal) * (face B surface area) * angle // multiply by angle
        }
        if face B shares v2 {
         angle = angle_between_vectors( v2 - v1 , v2 - v3 )
         n += (face B facet normal) * (face B surface area) * angle // multiply by angle
        }
        if face B shares v3 {
         angle = angle_between_vectors( v3 - v1 , v3 - v2 )
         n += (face B facet normal) * (face B surface area) * angle // multiply by angle
        }

       }
      }

      // normalize vertex normal
      vn = normalize(n)
     }
    }
    */

    /* Gather count of triangles for every vertex. This abuses the output
       storage to avoid extra allocations, zero-initialize it first to avoid
       random memory getting used. */
    std::vector<unsigned> triCount(nvtx);
    std::fill(triCount.begin(), triCount.end(), 0);
    for (unsigned i = 0; i < ntris * 3; i++)
        ++triCount[i];

    /* Turn that into a running offset array:
       triOffset[i + 1] - triOffset[i] is triangle count for vertex i
       triOffset[i] is offset into an triangle ID array for vertex i */
    std::vector<unsigned> triOffset(nvtx + 1);
    triOffset[0] = 0;
    for (std::size_t i = 0; i != triCount.size(); ++i)
        triOffset[i + 1] = triOffset[i] + triCount[i];

    assert(triOffset.back() == ntris * 3);

    /* Gather triangle IDs for every vertex. For vertex i,
       triIds[triOffset[i]] until triIds[triOffset[i + 1]]
       contains IDs of triangles that contain it. */
    std::vector<I> triIds(ntris * 3);
    for (std::size_t i = 0; i != ntris * 3; ++i) {
        const I triId = i / 3;
        const I vtxId = idx[i];

        /* How many triangle IDs is still left to be written, which also means
           the offset where we put the ID. Decrement that for the next run. */
        const std::size_t triIdsLeftForVertex = triCount[vtxId]--;
        triIds[triOffset[vtxId + 1] - triIdsLeftForVertex] = triId;
    }

    /* Now, triCount should be all zeros, we don't need it anymore and the
       underlying `normals` array is ready to get filled with real output. */

       /* Precalculate cross product and interior angles of each face --- the loop
          below would otherwise calculate it for every vertex, which is at least
          3x as much work */
    std::vector<std::pair<vec3<T>, vec3<T>>> crossAngles(ntris);
    for (std::size_t i = 0; i != crossAngles.size(); ++i) {
        auto v0 = vtx[idx[i * 3 + 0]];
        auto v1 = vtx[idx[i * 3 + 1]];
        auto v2 = vtx[idx[i * 3 + 2]];

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
        for (std::size_t t = triOffset[v]; t != triOffset[v + 1]; ++t) {
            const std::size_t baseIndex = triIds[t] * 3;
            auto v0i = idx[baseIndex + 0];
            auto v1i = idx[baseIndex + 1];
            auto v2i = idx[baseIndex + 2];

            /* Cross product is a vector in direction of the normal with length
               equal to size of the parallelogram */
            const auto& crossAngle = crossAngles[triIds[t]];

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

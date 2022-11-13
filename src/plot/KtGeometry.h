#pragma once
#include <vector>
#include "KtPoint.h"


template<typename VTX_ELEMENT_TYPE, typename IDX_TYPE>
class KtGeometry
{
public:
	using vertex_element_t = VTX_ELEMENT_TYPE;
	using vertex_t = KtPoint<VTX_ELEMENT_TYPE, 3>;
	using index_t = IDX_TYPE;


	void reserve(unsigned vxtCount, unsigned idxCount) {
		vtx_.reserve(vxtCount), idx_.reserve(idxCount);
	}

	void shrink_to_fit() {
		vtx_.shrink_to_fit(), idx_.shrink_to_fit();
	}

	// 压入顶点，返回顶点索引
	index_t pushVertex(const vertex_t& pt) {
		vtx_.push_back(pt);
	}

	index_t pushVertex(vertex_element_t x, vertex_element_t y, vertex_element_t z) {
		vtx_.emplace_back(x, y, z);
	}

	unsigned vertexCount() const {
		return vtx_.size();
	}

	const vertex_t& vertexAt(index_t idx) const {
		return vtx_.at(idx);
	}

	const vertex_t* vertexBuffer() const {
		return vtx_.data();
	}

	vertex_t* vertexBuffer() {
		return vtx_.data();
	}


	void pushIndex(index_t idx) {
		idx_.push_back(idx);
	}

	unsigned indexCount() const {
		return idx_.size();
	}

	index_t indexAt(unsigned idx) const {
		return idx_.at(idx);
	}

	index_t* indexBuffer() const {
		return idx_.data();
	}

	index_t* indexBuffer() {
		return idx_.data();
	}

private:
	std::vector<vertex_t> vtx_;
	std::vector<index_t> idx_;
};

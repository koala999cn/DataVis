#pragma once
#include <vector>
#include "KvGeometry.h"


template<typename VTX_TYPE, typename IDX_TYPE = std::uint32_t>
class KtGeometryImpl : public KvGeometry
{
public:
	using vertex_t = VTX_TYPE;
	using index_t = IDX_TYPE;

	KtGeometryImpl(KePrimitiveType type) : type_(type) {}

	void reserve(unsigned vxtCount, unsigned idxCount) {
		vtx_.reserve(vxtCount), idx_.reserve(idxCount);
	}

	vertex_t* newVertex(unsigned extra) {
		vtx_.resize(vtx_.size() + extra);
		return vtx_.data() + vtx_.size() - extra;
	}

	unsigned vertexCount() const override {
		return vtx_.size();
	}

	const vertex_t& vertexAt(index_t idx) const {
		return vtx_.at(idx);
	}

	void* vertexData() const override {
		return const_cast<vertex_t*>(vtx_.data());
	}

	vertex_t* vertexData() {
		return vtx_.data();
	}


	bool hasIndex() const {
		return !idx.empty();
	}

	index_t* newIndex(unsigned extra) {
		idx_.resize(idx_.size() + extra);
		return idx_.data() + idx_.size() - extra;
	}

	unsigned indexCount() const override {
		return idx_.size();
	}

	index_t indexAt(unsigned idx) const {
		return idx_.at(idx);
	}

	void* indexData() const override {
		return const_cast<index_t*>(idx_.data());
	}

	index_t* indexData() {
		return idx_.data();
	}


	KePrimitiveType type() const override { return type_; }

	unsigned vertexSize() const override { return sizeof(vertex_t); }

	unsigned indexSize() const override { return sizeof(index_t); }

private:
	KePrimitiveType type_;
	std::vector<vertex_t> vtx_;
	std::vector<index_t> idx_;
};

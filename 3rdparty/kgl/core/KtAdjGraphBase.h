#pragma once
#include <type_traits>
#include <vector>


/// 邻接矩阵图

template<typename ADJ_MATRIX, typename VERTEX_TYPE>
class KtAdjGraphBase
{
	using vertex_container = std::vector<VERTEX_TYPE>;

public:
	using vertex_type = VERTEX_TYPE;
	using edge_type = std::decay_t<decltype(*std::declval<ADJ_MATRIX>().row(0).begin())>;


	// 图的阶，即顶点数量
	unsigned order() const { return static_cast<unsigned>(adjMat_.rows()); }

	// 边的数量
	unsigned size() const { return E_; }

	decltype(auto) vertexAt(unsigned v) { return vertexes_.at(v); }
	decltype(auto) vertexAt(unsigned v) const { return vertexes_.at(v); }


	// 顶点v的出边
	auto outedges(unsigned v) { return adjMat_.row(v); }
	auto outedges(unsigned v) const { return adjMat_.row(v); }

protected:
	ADJ_MATRIX adjMat_; // 邻接矩阵
	unsigned E_{ 0 }; // 实时追踪边的数目
	vertex_container vertexes_;
};


template<typename ADJ_MATRIX>
class KtAdjGraphBase<ADJ_MATRIX, void>
{
public:
	using vertex_type = void;
	using edge_type = std::decay_t<decltype(*std::declval<ADJ_MATRIX>().row(0).begin())>;

	unsigned order() const { return static_cast<unsigned>(adjMat_.rows()); }

	unsigned size() const { return E_; }

	auto outedges(unsigned v) { return adjMat_.row(v); }
	auto outedges(unsigned v) const { return adjMat_.row(v); }

protected:
	ADJ_MATRIX adjMat_;
	unsigned E_{ 0 };
};

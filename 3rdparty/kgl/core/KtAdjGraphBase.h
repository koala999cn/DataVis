#pragma once
#include <type_traits>
#include <vector>


/// �ڽӾ���ͼ

template<typename ADJ_MATRIX, typename VERTEX_TYPE>
class KtAdjGraphBase
{
	using vertex_container = std::vector<VERTEX_TYPE>;

public:
	using vertex_type = VERTEX_TYPE;
	using edge_type = std::decay_t<decltype(*std::declval<ADJ_MATRIX>().row(0).begin())>;


	// ͼ�Ľף�����������
	unsigned order() const { return static_cast<unsigned>(adjMat_.rows()); }

	// �ߵ�����
	unsigned size() const { return E_; }

	decltype(auto) vertexAt(unsigned v) { return vertexes_.at(v); }
	decltype(auto) vertexAt(unsigned v) const { return vertexes_.at(v); }


	// ����v�ĳ���
	auto outedges(unsigned v) { return adjMat_.row(v); }
	auto outedges(unsigned v) const { return adjMat_.row(v); }

protected:
	ADJ_MATRIX adjMat_; // �ڽӾ���
	unsigned E_{ 0 }; // ʵʱ׷�ٱߵ���Ŀ
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

#pragma once
#include <string_view>
#include "KtUId.h"


// �������ֺ�Ψһ��ʶ���Ķ������

template<typename OBJ_TYPE, typename ID_TYPE = int>
class KtUniObject
{
public:

	KtUniObject(const std::string_view& name)
		: name_(name) {}

	KtUniObject(std::string&& name)
		: name_(std::move(name)) {}
	
	virtual ~KtUniObject() {} // make this polymorphism

	ID_TYPE id() const { return id_.id(); }

	const std::string& name() const { return name_; }

	template<typename T>
	T as() { return dynamic_cast<T>(this); }

protected:
	KtUId<OBJ_TYPE, ID_TYPE> id_; // ���ڱ�ʾnode��Ψһid
	std::string name_;
};

#pragma once
#include "KvPlot.h"
#include "KcCoordSystem.h"
#include "KtMatrix4.h"


class KvPlot3d : public KvPlot
{
public:

	using mat4 = mat4f<>;
	using point3 = point3f;

	KvPlot3d(std::shared_ptr<KvPaint> paint);

	/// ����ӿ�

	virtual mat4 viewMatrix() const = 0;
	virtual void setViewMatrix(const mat4&) = 0;

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

	void update() override;

	void fitData() override;

	bool isOrtho() const { return ortho_; }
	void setOrtho(bool b) { ortho_ = b; }

	bool isIsometric() const { return isometric_; }
	void setIsometric(bool b) { isometric_ = b; }

	KcCoordSystem& coordSystem() {
		return *coord_.get();
	}

protected:
	virtual void autoProject_() = 0;
	
protected:
	std::unique_ptr<KcCoordSystem> coord_; // ���ô���������
	
	bool ortho_{ true }; // ����ͶӰ vs. ͸��ͶӰ
	
	// ���²������ڵ��������modelview����
	float zoom_{ 1 };
	point3 scale_{ 1, 1, 1 }; 
	point3 shift_{ 0, 0, 0 };
	quatf orient_{ 0, 0, 0, 1 }; // ������ķ�λ

	bool isometric_{ false }; // ��true���򱣳�����ϵ�ĵȱ��ԣ�������ĵ�Ԫ���ȱ���һ��
	                          // ��false�������ȿ��ǲ��������ԣ�����ϵ�ĳ�ʼ͸�ӽ��ʼ��Ϊ�����Σ�������ͨ��scale_�����������죩
};
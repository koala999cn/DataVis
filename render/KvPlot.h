#pragma once


// ��ͼ���࣬��װplot2d/3d��ͨ�����������

class KvPlot
{
public:
	KvPlot(void* parent);
	virtual ~KvPlot();

	void* handle() const; // ������KvPlot�ҽӵ�win����ָ��

	void show(bool b);

	bool visible() const;

private:
	void* d_ptr_;
};

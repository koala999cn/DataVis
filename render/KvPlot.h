#pragma once


// 绘图基类，封装plot2d/3d的通用属性与操作

class KvPlot
{
public:
	KvPlot(void* parent);
	virtual ~KvPlot();

	void* handle() const; // 返回与KvPlot挂接的win对象指针

	void show(bool b);

	bool visible() const;

private:
	void* d_ptr_;
};

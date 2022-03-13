#pragma once
#include "KvPropertiedObject.h"
#include <memory>

class KvData;


// 数据渲染的抽象类
class KvDataRender : public KvPropertiedObject
{
public:

	// 导入基类构造函数
	using KvPropertiedObject::KvPropertiedObject;

	// 提供发生错误时的文本描述
	virtual std::string errorText() const {
		return ""; // 空字符串表示无错误
	}


public slots:

	// 渲染数据
	virtual bool render(std::shared_ptr<KvData> data) = 0;

	// 重置
	virtual void reset() = 0;

	// 显示/隐藏用于render的窗口或其他信息
	virtual void show(bool bShow) = 0;
};


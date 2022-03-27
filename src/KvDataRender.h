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

	// 该render是否可被显示
	virtual bool canShown() const = 0;

	// 该render是否可见
	virtual bool isVisible() const = 0;


public slots:

	// 渲染数据
	bool render(std::shared_ptr<KvData> data) {
		syncParent();
		return renderImpl_(data);
	}

	// 重置
	virtual void reset() = 0;

	// 显示/隐藏用于render的窗口或其他信息
	virtual void show(bool bShow) = 0;


private:
	virtual void syncParent() = 0;
	virtual bool renderImpl_(std::shared_ptr<KvData> data) = 0;
};


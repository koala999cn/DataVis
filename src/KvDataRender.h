#pragma once
#include "KvPropertiedObject.h"
#include <memory>

class KvData;


// 数据渲染的抽象类
class KvDataRender : public KvPropertiedObject
{
public:

	using KvPropertiedObject::KvPropertiedObject;


	// TODO: 返回支持的渲染数据格式
	virtual int supportDataFormat() const {
		return 0;
	}


	// 提供发生错误时的文本描述
	virtual std::string errorText() const {
		return ""; // 空字符串表示无错误
	}


public slots:

	// 执行数据渲染操作
	virtual bool render(std::shared_ptr<KvData> data) = 0;


signals:

	// 请求最新的待渲染数据 
	// 当渲染器从不可见变为可见时，或者当用户需要单独显示此渲染器效果时，
	// 以及其他需要当前渲染数据的时候，可以发送该信号
	// 响应该信号的客户，应该调用信号发送者的render方法。
	void dataRequest();
};


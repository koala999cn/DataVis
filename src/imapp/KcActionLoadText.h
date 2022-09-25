#pragma once
#include "KcActionSequence.h"


// LoadText动作: 由3个子动作构成
// 一是KcOpenFileAction，弹出对话框选取文件
// 二是KcDataFormatAction，让用户选取数据类型
// 三是KcNewNodeAction，向NodeEditor插入provider节点

class KcActionLoadText : public KcActionSequence
{
public:
	explicit KcActionLoadText();
};

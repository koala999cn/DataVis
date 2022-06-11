#pragma once
#include <QTableView>

class KvDiscreted;

// 显示与编辑各类数据
class QtDataView : public QTableView
{
	Q_OBJECT

public:
	QtDataView(QWidget* parent = nullptr);

	void fill(KvDiscreted& disc);
};

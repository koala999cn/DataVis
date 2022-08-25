#pragma once
#include <QTableWidget>
#include "QCustomPlot/qcustomplot.h"

class KvDiscreted;

// 显示与编辑各类数据
class QtDataView : public QTableWidget
{
	Q_OBJECT

public:
	QtDataView(QWidget* parent = nullptr);

	void fill(const KvDiscreted& disc);

	// 支持QCustomPlot
	template<typename T>
	void fill(const QCPDataContainer<T>& data) {
        setColumnCount(2);
        setRowCount(data.size());

        for (int r = 0; r < data.size(); r++) {
            auto iter = data.at(r);
            setItem(r, 0, new QTableWidgetItem(QString("%1").arg(iter->key)));
            setItem(r, 1, new QTableWidgetItem(QString("%1").arg(iter->value)));;
        }
	}

    void fill(const QCPColorMapData& data);
};

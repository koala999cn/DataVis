#pragma once
#include <QTableView>
#include <QStandardItemModel>
#include "QCustomPlot/qcustomplot.h"

class KvDiscreted;

// 显示与编辑各类数据
class QtDataView : public QTableView
{
	Q_OBJECT

public:
	QtDataView(QWidget* parent = nullptr);

	void fill(const KvDiscreted& disc);

	// 支持QCustomPlot
	template<typename T>
	void fill(const QCPDataContainer<T>& data) {
        auto model = new QStandardItemModel(this);

        model->setColumnCount(2);

        model->setRowCount(data.size());

        for (kIndex r = 0; r < data.size(); r++) {
            auto iter = data.at(r);
            model->setItem(r, 0, new QStandardItem(QString("%1").arg(iter->key)));
            model->setItem(r, 1, new QStandardItem(QString("%1").arg(iter->value)));;
        }

        setModel(model);
	}

    void fill(const QCPColorMapData& data);
};

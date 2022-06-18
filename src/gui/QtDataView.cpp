#include "QtDataView.h"
#include "KvDiscreted.h"


QtDataView::QtDataView(QWidget* parent)
    : QTableWidget(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void QtDataView::fill(const KvDiscreted& disc)
{
    setColumnCount(disc.dim() + disc.channels());

    setRowCount(disc.size());

    for (kIndex r = 0; r < disc.size(); r++) {
        auto pt = disc.pointAt(r, 0);
        for (kIndex col = 0; col <= disc.dim(); col++)
            setItem(r, col, new QTableWidgetItem(QString("%1").arg(pt[col])));
        for (kIndex ch = 1; ch < disc.channels(); ch++) 
            setItem(r, disc.dim() + ch, 
                new QTableWidgetItem(QString("%1").arg(disc.valueAt(r, ch))));
    }
}


void QtDataView::fill(const QCPColorMapData& data)
{
    setColumnCount(data.keySize());

    setRowCount(data.valueSize());

    for (kIndex r = 0; r < data.valueSize(); r++)
        for (kIndex c = 0; c < data.keySize(); c++)
            setItem(r, c, new QTableWidgetItem(QString("%1").
                arg(((QCPColorMapData&)data).cell(c, r))));
}
#include "QtDataView.h"
#include <QStandardItemModel>
#include "KvDiscreted.h"


QtDataView::QtDataView(QWidget* parent)
    : QTableView(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void QtDataView::fill(KvDiscreted& disc)
{
    auto model = new QStandardItemModel(this);
    
    model->setColumnCount(disc.dim() + disc.channels());

    model->setRowCount(disc.size());

    std::vector<kIndex> idx(disc.dim(), 0);
    for (kIndex r = 0; r < disc.size(); r++) {
        auto pt = disc.point(idx.data(), 0);
        for (kIndex col = 0; col <= disc.dim(); col++)
            model->setItem(r, col, new QStandardItem(QString("%1").arg(pt[col])));
        for (kIndex ch = 1; ch < disc.channels(); ch++) 
            model->setItem(r, disc.dim() + ch, 
                new QStandardItem(QString("%1").arg(disc.value(idx.data(), ch))));

        disc.nextIndex(idx.data());
    }

    setModel(model);
}


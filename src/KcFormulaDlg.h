#pragma once
#include <QDialog>
#include <memory>


class KvExprtk;
class KvData;


namespace Ui {
class formula_dlg;
}

class KcFormulaDlg : public QDialog
{
    Q_OBJECT

public:
    explicit KcFormulaDlg(QWidget *parent = nullptr);
    ~KcFormulaDlg();

    void accept() override;

    QString exprText() const;
    
    std::shared_ptr<KvData> data;
    
private:
    Ui::formula_dlg* ui;
    std::shared_ptr<KvExprtk> expr_;
};



#pragma once
#include <QDialog>
#include <memory>


class KvExprtk;
class KvData;


namespace Ui {
class formula_dlg;
}

class QtFormulaDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QtFormulaDlg(QWidget *parent = nullptr);
    ~QtFormulaDlg();

    void accept() override;

    QString exprText() const;
    
    std::shared_ptr<KvData> data;
    
private:
    Ui::formula_dlg* ui;
    std::shared_ptr<KvExprtk> expr_;
};



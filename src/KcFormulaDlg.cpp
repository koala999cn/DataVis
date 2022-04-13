#include "KcFormulaDlg.h"
#include "ui_formula_dlg.h"
#include <QDoubleValidator>
#include <QMessageBox>
#include <QPushButton>
#include "dsp/kDsp.h"
#include "dsp/KtContinuedExpr.h"
#include "exprtkX/KcExprtk1d.h"
#include "exprtkX/KcExprtk2d.h"
#include "exprtkX/KcExprtk3d.h"


KcFormulaDlg::KcFormulaDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formula_dlg)
{
    ui->setupUi(this);
    ui->expr->setFocus();
}


KcFormulaDlg::~KcFormulaDlg()
{
    delete ui;
}


QString KcFormulaDlg::exprText() const
{
    auto text = ui->expr->text();
    if (text.isEmpty())
        text = ui->expr->placeholderText();

    return text;
}


void KcFormulaDlg::accept()
{
    expr_ = std::make_shared<KcExprtk1d>();
    if(expr_->compile(exprText().toStdString())) { // 首先尝试编译一维表达式
        data = std::make_shared<KtContinuedExpr<1>>(expr_);
        QDialog::accept();
        return;
    }

    expr_ = std::make_shared<KcExprtk2d>();
    if (expr_->compile(exprText().toStdString())) { // 尝试编译二维表达式
        data = std::make_shared<KtContinuedExpr<2>>(expr_);
        QDialog::accept();
        return;
    }

    expr_ = std::make_shared<KcExprtk3d>();
    if (expr_->compile(exprText().toStdString())) { // 尝试编译三维表达式
        data = std::make_shared<KtContinuedExpr<3>>(expr_);
        QDialog::accept();
        return;
    }

    
    // error
    QMessageBox::information(this, tr("错误"), QString::fromLocal8Bit(expr_->error()));
    expr_.reset();
    ui->expr->setFocus();
}

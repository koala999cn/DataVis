#include "KcFormulaDlg.h"
#include "ui_formula_dlg.h"
#include <QDoubleValidator>
#include <QMessageBox>
#include <QPushButton>
#include "dsp/kDsp.h"
#include "dsp/KcFormulaData1d.h"
#include "exprtkX/KcExprtk1d.h"
#include "exprtkX/KcExprtk2d.h"
#include "exprtkX/KcExprtk3d.h"


KcFormulaDlg::KcFormulaDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formula_dlg)
{
    ui->setupUi(this);
    ui->xmin->setValidator(new QDoubleValidator);
    ui->xmax->setValidator(new QDoubleValidator);
    ui->rate->setValidator(new QDoubleValidator(std::numeric_limits<double>::epsilon(), 
        std::numeric_limits<double>::max(), 6, this));
    ui->nx->setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));
    ui->expr->setFocus();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}


KcFormulaDlg::~KcFormulaDlg()
{
    delete ui;
}


void KcFormulaDlg::on_nx_textEdited(const QString& text)
{
    // 根据nx同步rate
    auto xmin = ui->xmin->text().toDouble();
    auto xmax = ui->xmax->text().toDouble();
    if (xmax > xmin) {
        auto nx = text.toLong();
        if (nx > 0) {
            KtSampling<kReal> samp;
            samp.resetn(xmin, xmax, nx);
            ui->rate->setText(QString("%1").arg(samp.rate()));
        }
        else {
            ui->rate->clear();
        }
    }
}


void KcFormulaDlg::on_rate_textEdited(const QString& text)
{
    // 根据rate同步nx
    auto xmin = ui->xmin->text().toDouble();
    auto xmax = ui->xmax->text().toDouble();
    if (xmax > xmin) {
        auto rate = text.toDouble();
        if (rate > 0) {
            KtSampling<kReal> samp;
            samp.reset(xmin, xmax, 1.0/rate, xmin);
            ui->nx->setText(QString("%1").arg(samp.nx()));
        }
        else {
            ui->nx->clear();
        }
    }
}


void KcFormulaDlg::on_xmin_textEdited(const QString&)
{
    // 同步nx和rate
    on_nx_textEdited(ui->nx->text());
}


void KcFormulaDlg::on_xmax_textEdited(const QString&)
{
    // 同步nx和rate
    on_nx_textEdited(ui->nx->text());
}


void KcFormulaDlg::on_expr_textChanged(const QString& text)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(text.isEmpty());
}


QString KcFormulaDlg::exprText() const
{
    return ui->expr->text();
}


void KcFormulaDlg::accept()
{
    auto xmin = ui->xmin->text().toDouble();
    auto xmax = ui->xmax->text().toDouble();

    if (xmin >= xmax) {
        QMessageBox::information(this, tr(u8"参数错误"), tr(u8"值区间xmax必须大于xmin"));
        ui->xmax->setFocus();
        return;
    }

    auto nx = ui->nx->text().toLong();
    if (nx <= 0) {
        QMessageBox::information(this, tr(u8"参数错误"), tr(u8"样本数必须大于0。"));
        ui->nx->setFocus();
        return;
    }

    auto rate = ui->rate->text().toDouble();
    if (rate <= 0) {
        QMessageBox::information(this, tr(u8"参数错误"), tr(u8"采样率须大于0。"));
        ui->rate->setFocus();
        return;
    }


    expr_ = std::make_shared<KcExprtk1d>();
    if(expr_->compile(ui->expr->text().toStdString())) { // 首先尝试编译一维表达式
        data = std::make_shared<KcFormulaData1d>(
            ui->xmin->text().toDouble(), ui->xmax->text().toDouble(), 
            ui->nx->text().toLong(), expr_);
        QDialog::accept();
        return;
    }

    expr_ = std::make_shared<KcExprtk2d>();
    if (expr_->compile(ui->expr->text().toStdString())) { // 尝试编译二维表达式
        // TODO:
        assert(false);
        //data = std::make_shared<KcFormulaData2d>(
        //    ui->xmin->text().toDouble(), ui->xmax->text().toDouble(),
        //    ui->nx->text().toLong(), expr_);
        QDialog::accept();
        return;
    }

    expr_ = std::make_shared<KcExprtk3d>();
    if (expr_->compile(ui->expr->text().toStdString())) { // 尝试编译三维表达式
        // TODO:
        assert(false);
        //data = std::make_shared<KcFormulaData2d>(
        //    ui->xmin->text().toDouble(), ui->xmax->text().toDouble(),
        //    ui->nx->text().toLong(), expr_);
        QDialog::accept();
        return;
    }

    
    // error
    QMessageBox::information(this, tr("错误"), QString::fromLocal8Bit(expr_->error()));
    expr_.reset();
    ui->expr->setFocus();
}

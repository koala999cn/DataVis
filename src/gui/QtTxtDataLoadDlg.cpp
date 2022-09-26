#include "QtTxtDataLoadDlg.h"
#include "ui_txt_data_load_dlg.h"
#include "prov/KgTxtDataLoader.h"
#include "dsp/KuMatrixUtil.h"
#include <QButtonGroup>


QtTxtDataLoadDlg::QtTxtDataLoadDlg(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::txt_data_load_dlg)
{
    ui->setupUi(this);
    path_ = path;
    loader_ = std::make_unique<KgTxtDataLoader>();

    ui->regexDelim->setText(QString::fromStdString(loader_->delim()));
    ui->regexNA->setText(QString::fromStdString(loader_->NA()));
    ui->regexComment->setText(QString::fromStdString(loader_->comment()));
    ui->cbIllegalAs->setCurrentIndex(loader_->illegalMode());
    ui->cbEmptyAs->setCurrentIndex(loader_->emptyMode());

    auto bg = new QButtonGroup(this);
    bg->addButton(ui->rbColMajor);
    bg->addButton(ui->rbRowMajor);
    ui->rbColMajor->setChecked(true);

    ui->cbForceAlign->setChecked(true);
    ui->cbImportAs->setCurrentIndex(0);
    ui->leStartRow->setText("0");
    ui->leStartRow->setValidator(new QIntValidator);
    ui->leStartCol->setText("0");
    ui->leStartCol->setValidator(new QIntValidator);

    connect(ui->btReload, &QPushButton::click, this, &QtTxtDataLoadDlg::reload);
    connect(ui->rbRowMajor, &QRadioButton::toggled, [this](bool) {
        updateImportAs_();
        });
    reload();
}

QtTxtDataLoadDlg::~QtTxtDataLoadDlg()
{
    delete ui;
}

void QtTxtDataLoadDlg::accept()
{
    /// 清理mat_

    bool rowMajor = ui->rbRowMajor->isChecked();
    bool forceAligned = ui->cbForceAlign->isChecked();
    auto startRow = ui->leStartRow->text().toInt();
    auto startCol = ui->leStartCol->text().toInt();

    if (forceAligned)
        KuMatrixUtil::forceAligned(mat_);

    if (startRow != 0)
        mat_.erase(mat_.begin(), mat_.begin() + startRow);

    if (startCol != 0)
        for (auto& r : mat_)
            r.erase(r.begin(), r.begin() + startCol);

    // 为方便数据转换，使用rowMajor布局
    if (!rowMajor)
        mat_ = KuMatrixUtil::transpose(mat_);


    /// 生成data_, 此时mat_为rowMajor
    auto type = ui->cbImportAs->currentData().toInt();
    data = KuMatrixUtil::makeData(mat_, KuMatrixUtil::KeDataType(type));

    QDialog::accept();
}

void QtTxtDataLoadDlg::reload()
{
    // 设置loader_的参数
    loader_->setDelim(ui->regexDelim->text().toStdString());
    loader_->setNA(ui->regexNA->text().toStdString());
    loader_->setComment(ui->regexComment->text().toStdString());
    loader_->setIllegalMode(KgTxtDataLoader::KeIllegalMode(ui->cbIllegalAs->currentIndex()));
    loader_->setEmptyMode(KgTxtDataLoader::KeEmptyMode(ui->cbEmptyAs->currentIndex()));

    mat_.clear();
    bool ok = loader_->load(path_.toLocal8Bit().constData(), mat_);
    QString result;

    if (!ok) {
        result = tr("failed to load data");
        updateIntEdit_(ui->leStartRow, 0, 0);
        updateIntEdit_(ui->leStartCol, 0, 0);
    }
    else {
        auto cr = KuMatrixUtil::colsRange(mat_);
        if (cr.second == 0) {
            result = tr("no data loaded");
            ok = false;
        }
        else if (cr.first == cr.second) {
            result = QString(tr("loaded %1 rows & %2 columns")).arg(mat_.size()).arg(cr.first);
        }
        else {
            ok = ui->cbForceAlign->isChecked();
            result = QString(tr("loaded %1 rows with columns between %2 and %3")).
                arg(mat_.size()).arg(cr.first).arg(cr.second);
        }

        updateIntEdit_(ui->leStartRow, 0, mat_.size());
        updateIntEdit_(ui->leStartCol, 0, cr.first);
    }

    ui->dbBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
    ui->lbStatus->setText(result);

    if (!mat_.empty()) 
        ui->rbRowMajor->setChecked(mat_.size() < mat_[0].size());

    // 根据数据加载情况更新importAs组件
    updateImportAs_();
}


void QtTxtDataLoadDlg::updateIntEdit_(QLineEdit* edit, int low, int high)
{
    auto validator = (QIntValidator*)edit->validator();
    validator->setRange(low, high);
    auto text = edit->text();
    validator->fixup(text);
    edit->setText(text);
}


void QtTxtDataLoadDlg::updateImportAs_()
{
    int rows = mat_.size();
    int cols = ((QIntValidator*)ui->leStartCol->validator())->top();

    auto rowMajor = ui->rbRowMajor->isChecked();
    if (rowMajor)
        std::swap(rows, cols);

    // column major
    ui->cbImportAs->clear();

    auto types = KuMatrixUtil::validTypes(mat_, !rowMajor);
    for(auto t : types)
        ui->cbImportAs->addItem(KuMatrixUtil::typeStr(t), t);
    ui->cbImportAs->setCurrentIndex(0);
}


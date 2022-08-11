#include "QtTxtDataLoadDlg.h"
#include "ui_txt_data_load_dlg.h"
#include "prov/KgTxtDataLoader.h"
#include <QButtonGroup>


namespace kPrivate
{
    enum KeDataType
    {
        k_series,
        k_matrix,
        k_scattered_1d,
        k_scattered_2d,
        k_sampled_1d,
        k_sampled_2d
    };
}


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
    ui->leStartRow->setValidator(new QIntValidator);
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
        KgTxtDataLoader::forceAligned(mat_);

    if (startRow != 0)
        mat_.erase(mat_.begin(), mat_.begin() + startRow);

    if (startCol != 0)
        for (auto& r : mat_)
            r.erase(r.begin(), r.begin() + startCol);

    // 为方便数据转换，使用rowMajor布局
    if (!rowMajor)
        mat_ = KgTxtDataLoader::transpose(mat_);


    /// 生成data_, 此时mat_为rowMajor
    auto dataType = ui->cbImportAs->currentData().toInt();
    switch (dataType)
    {
    case kPrivate::k_series:
        data = makeSeries_(mat_);
        break;

    case kPrivate::k_matrix:
        data = makeMatrix_(mat_);
        break;

    case kPrivate::k_scattered_1d:
    case kPrivate::k_scattered_2d:
        data = makeScattered_(mat_);
        break;

    case kPrivate::k_sampled_1d:
        data = makeSampled1d_(mat_);
        break;

    case kPrivate::k_sampled_2d:
        data = makeSampled2d_(mat_);
        break;

    default:
        data.reset();
        assert(false);
    }

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

    bool ok = loader_->load(path_.toLocal8Bit().constData(), mat_);
    QString result;

    if (!ok) {
        result = tr("failed to load data");
        updateIntEdit_(ui->leStartRow, 0, 0);
        updateIntEdit_(ui->leStartCol, 0, 0);
    }
    else {
        auto cr = KgTxtDataLoader::colsRange(mat_);
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

    if (rows && cols) {
        ui->cbImportAs->addItem("series", kPrivate::k_series);
        ui->cbImportAs->addItem("matrix", kPrivate::k_matrix);
        
        if (cols > 1) {

            if (cols == 2)
                ui->cbImportAs->addItem("scattered-1d", kPrivate::k_scattered_1d);
            else if (cols == 3)
                ui->cbImportAs->addItem("scattered-2d", kPrivate::k_scattered_2d);
     
            if(!rowMajor && KgTxtDataLoader::isEvenlySpaced(KgTxtDataLoader::column(mat_, 0)) ||
                rowMajor && KgTxtDataLoader::isEvenlySpaced(mat_[0]))
                ui->cbImportAs->addItem("sampled-1d", kPrivate::k_sampled_1d);

            if (rows > 1 && KgTxtDataLoader::isEvenlySpaced(mat_[0]) 
                && KgTxtDataLoader::isEvenlySpaced(KgTxtDataLoader::column(mat_, 0)))
                ui->cbImportAs->addItem("sampled-2d", kPrivate::k_sampled_2d);
        }
    }
}


#include "KcSampled1d.h"
std::shared_ptr<KvData> QtTxtDataLoadDlg::makeSeries_(const std::vector<std::vector<double>>& mat)
{
    // 暂时使用sampled1d表示series数据

    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat[0].size(), mat.size());
    samp->reset(0, 0, 1);

    for(unsigned c = 0; c < samp->channels(); c++)
        samp->setChannel(0, c, mat[c].data());

    return samp;
}


#include "KcSampled2d.h"
std::shared_ptr<KvData> QtTxtDataLoadDlg::makeMatrix_(const std::vector<std::vector<double>>& mat)
{
    auto samp2d = std::make_shared<KcSampled2d>();
    samp2d->resize(mat.size() , mat[0].size());
    samp2d->reset(0, 0, 1);
    samp2d->reset(1, 0, 1);

    for (kIndex idx = 0; samp2d->size(0); idx++)
        samp2d->setChannel(&idx, 0, mat[idx].data()); // 始终单通道

    return samp2d;
}


std::shared_ptr<KvData> QtTxtDataLoadDlg::makeSampled1d_(const std::vector<std::vector<double>>& mat)
{
    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat[0].size(), mat.size() - 1);
    samp->reset(0, mat[0][0], mat[0][1] - mat[0][0]);

    for (unsigned c = 0; c < samp->channels(); c++)
        samp->setChannel(0, c, mat[c + 1].data());

    return samp;
}


std::shared_ptr<KvData> QtTxtDataLoadDlg::makeSampled2d_(const std::vector<std::vector<double>>& mat)
{
    // mat[0][0]为占位符，无效

    auto dx = mat[2][0] - mat[1][0];
    auto dy = mat[0][2] - mat[0][1];

    auto samp2d = std::make_shared<KcSampled2d>();
    samp2d->resize(mat.size() - 1, mat[0].size() - 1);
    samp2d->reset(0, mat[1][0], dx);
    samp2d->reset(1, mat[0][1], dy);

    for(kIndex idx = 0; samp2d->size(0); idx++)
        samp2d->setChannel(&idx, 0, mat[idx + 1].data() + 1); // 始终单通道, 忽略mat数据的首行首列

    return samp2d;
}


#include "KtScattered.h"
std::shared_ptr<KvData> QtTxtDataLoadDlg::makeScattered_(const std::vector<std::vector<double>>& mat)
{
    if (mat.size() == 2) {
        auto scattered = std::make_shared<KtScattered<1>>();
        scattered->reserve(mat[0].size());
        for (unsigned i = 0; i < mat[0].size(); i++)
            scattered->pushBack({ mat[0][i], mat[1][i] });

        return scattered;
    }
    else {
        assert(mat.size() == 3);
        auto scattered = std::make_shared<KtScattered<2>>();
        scattered->reserve(mat[0].size());
        for (unsigned i = 0; i < mat[0].size(); i++)
            scattered->pushBack({ mat[0][i], mat[1][i], mat[2][i] });

        return scattered;
    }
}
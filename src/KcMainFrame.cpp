#include "KcMainFrame.h"
#include <QFileDialog>
#include "KcPlotWidget.h"
#include "KcFormulaDlg.h"
#include "KcAudioCaptureDlg.h"
#include <fstream>
#include "KvData1d.h"
#include <string>
#include "base/KuStrUtil.h"
#include "KcFormulaData1d.h"
#include "KcSampled1d.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioRender.h"


KcMainFrame::KcMainFrame(const QString &uniqueName)
    : KDDockWidgets::MainWindow(uniqueName)
    //, ui(new Ui::MainFrame)
{
    //ui->setupUi(this);
    //auto cw = centralWidget();
    //if(cw != nullptr)
    //    delete cw;

    render_ = std::make_shared<KcAudioRender>();
}

KcMainFrame::~KcMainFrame()
{
    //delete ui;
    //qDeleteAll(dockWidgets_);
}

bool KcMainFrame::init()
{
    return setupMenu_();
}


bool KcMainFrame::setupMenu_()
{
    auto menubar = menuBar();

    /// 准备<文件>菜单
    auto fileMenu = new QMenu(QStringLiteral("文件(&F)"), this);
    menubar->addMenu(fileMenu);

    QAction *dataFile = fileMenu->addAction(QStringLiteral("数据(&D)..."));
    connect(dataFile, &QAction::triggered, this, &KcMainFrame::openDataFile);

    QAction *audioFile = fileMenu->addAction(QStringLiteral("音频(&A)..."));
    connect(audioFile, &QAction::triggered, this, &KcMainFrame::openAudioFile);

    QAction *audioCapture = fileMenu->addAction(QStringLiteral("录音(&R)..."));
    connect(audioCapture, &QAction::triggered, this, &KcMainFrame::openAudioCapture);

    QAction *formula = fileMenu->addAction(QStringLiteral("数学表达式(&F)..."));
    connect(formula, &QAction::triggered, this, &KcMainFrame::openFormula);


    fileMenu->addSeparator();

    QAction *dataStream = fileMenu->addAction(QStringLiteral("数据流(&A)..."));
    QAction *audioStream = fileMenu->addAction(QStringLiteral("麦克风(&M)..."));
    QAction *formulaStream = fileMenu->addAction(QStringLiteral("公式流(&O)..."));

    fileMenu->addSeparator();

    QAction *exit = fileMenu->addAction(QStringLiteral("退出(&X)"));


    /// 准备<视图>菜单
    auto viewMenu = new QMenu(QStringLiteral("视图(&F)"), this);
    menubar->addMenu(viewMenu);


    return true;
}


bool KcMainFrame::createLauout_()
{
    return true;
}


KcPlotWidget* KcMainFrame::createPlotDock_(const QString& plotName)
{
    auto dock = new KDDockWidgets::DockWidget{ plotName };
    dock->setAttribute(Qt::WA_DeleteOnClose);
    auto widget = new KcPlotWidget(this);
    dock->setWidget(widget);
    addDockWidget(dock, KDDockWidgets::Location_None);
    dock->show();

    return widget;
}


void KcMainFrame::openDataFile()
{
    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    //fd.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    fd.setWindowTitle(tr("打开数据文件"));

    //    fileDialog.setDirectory(picturesLocation());

    if (fd.exec() == QDialog::Accepted) {
        data_ = loadData_(fd.selectedFiles().constFirst());
        plotData(data_.get());
    }
}


void KcMainFrame::openAudioFile()
{
    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setWindowTitle(tr("打开音频文件"));

    if (fd.exec() == QDialog::Accepted) {
        std::shared_ptr<KcAudio> audio = std::make_shared<KcAudio>();
        if(!audio->load(fd.selectedFiles().constFirst().toStdString())) {
            QMessageBox msg(this);
            msg.setWindowTitle(tr("错误"));
            msg.setText(tr("无法打开音频文件！"));
            msg.setIcon(QMessageBox::Information);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            return;
        }

        data_ = audio;
        plotData(data_.get());

        render_->playback(audio);
    }
}


void KcMainFrame::openAudioCapture()
{
    KcAudioCaptureDlg dlg;
    if (dlg.exec() == QDialog::Accepted) {
        data_ = dlg.audio_;
        plotData(data_.get());
    }
}


void KcMainFrame::openFormula()
{
    KcFormulaDlg dlg;
    if (dlg.exec() == QDialog::Accepted) {
        data_ = std::make_shared<KcFormulaData1d>(0, 10, 1000, dlg.expr);
        plotData(data_.get());
    }
}


std::shared_ptr<KvData1d> KcMainFrame::loadData_(const QString& filePath)
{
    std::shared_ptr<KvData1d> data; // the result to returned

    std::ifstream ifs(filePath.toStdString().c_str());
    if(!ifs.is_open()) {
        QMessageBox msg(this);
        msg.setWindowTitle(tr("错误"));
        msg.setText(tr("无法打开数据文件！"));
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return nullptr;
    }

    // load data from ifs
    std::string line;

    // parse the first line
    std::getline(ifs, line);
    auto tokens = KuStrUtil::split(line, "\t ,");
    auto cols = tokens.size();
    if(cols == 0 || cols > 2) {
        QMessageBox msg(this);
        msg.setWindowTitle(tr("错误"));
        msg.setText(tr("不支持的数据文件格式！"));
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return nullptr;
    }

    if(cols == 1) {
        std::vector<kReal> v;
        do {
            v.push_back(std::stod(tokens[0]));
        } while(std::getline(ifs, line) && (tokens = KuStrUtil::split(line, "\t ,")).size() == 1);

        data = std::make_shared<KcSampled1d>(std::move(v));
    }
    else {
        assert(false); // TODO:
    }


    return data;
}


void KcMainFrame::plotData(const KvData1d* data, KcPlotWidget* widget)
{
    if(widget == nullptr)
        widget = createPlotDock_("TODO: NAME");


    widget->xAxis->setLabel("x");
    widget->yAxis->setLabel("y");

    QCPGraph* g = widget->addGraph();
    g->setAdaptiveSampling(true);
    for(kIndex i = 0; i < data->count(); i++) {
        auto pt = data->value(i);
        g->addData(pt.first, pt.second);
    }

    widget->rescaleAxes();
    widget->replot();
}

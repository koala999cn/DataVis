#include "QtAudioCaptureDlg.h"
#include "ui_audio_capture_dlg.h"
#include <assert.h>
#include <memory>
#include <QFileDialog>
#include <QMessageBox>
#include "audio/KcAudioDevice.h"
#include "audio/KcAudioRender.h"
#include "audio/KcAudioCapture.h"
#include "audio/KgAudioFile.h"
#include "audio/KcAudio.h"
#include "base/KtuMath.h"
#include "dsp/KtSampling.h"
#include "gui/QtAudioUtils.h"


namespace kPrivate
{
    // 该oberver用于监听每帧数据，并动态更新ui组件
    class KcWaveObserver : public KcAudioCapture::observer_type
    {
    public:
        KcWaveObserver(Ui::QtAudioCaptureDlg *ui, const std::shared_ptr<KcAudio>& audio)
            : ui_(ui), audio_(audio) {}

        bool update(void *data, unsigned frames, double streamTime) override {

            // 更新音量组件
            if(audio_->channels() == 1) { // 单声道
                auto volumn = KtuMath<kReal>::minmax((kReal*)data, frames*audio_->channels());
                ui_->wgVolumn->setVolumn(std::max(std::abs(volumn.first), std::abs(volumn.second)));
            }
            else { // 立体声
                assert(audio_->channels() == 2);
                kReal volLeft(0), volRight(0);
                const kReal* buf = (kReal*)data;
                for(unsigned i = 0; i < frames; i++, buf += 2) {
                    volLeft = std::max(volLeft, std::abs(buf[0]));
                    volRight = std::max(volRight, std::abs(buf[1]));
                    ui_->wgVolumn->setVolumn(volLeft, volRight);
                }
            }


            ui_->wgVolumn->update();

            // 更新计时组件
            ui_->lbTime->setText(QString(u8"%1:%2:%3").
                                    arg(int(streamTime/60)%60, 2, 10, QChar('0')).
                                    arg(int(streamTime)%60, 2, 10, QChar('0')).
                                    arg(int(streamTime*1000)%1000, 3, 10, QChar('0'))
                                    );

            // 更新波形组件
            auto bars = ui_->wgWave->getBarCount();
            std::vector<std::pair<float, float>> ranges(bars);
            KtSampling<kReal> samp;
            samp.resetn(bars, 0, frames, 0);
            for(int i = 0; i < bars; i++) {
                long x0 = std::ceil(samp.indexToX(i));
                long x1 = std::floor(samp.indexToX(i+1));
                assert(x0 < frames);
                if(x1 >= frames) x1 = frames - 1;
                const kReal* buf = (kReal*)data;
                buf += audio_->channels() * x0;
                auto r = KtuMath<kReal>::minmax(buf, audio_->channels() * (x1 - x0 + 1));
                assert(r.first >= -1 && r.first <= 1 && r.second >= -1 && r.second <= 1);
                ranges[i].first = r.first, ranges[i].second = r.second;
            }
            ui_->wgWave->setBarRanges(ranges);
            ui_->wgWave->update();

            return true;
        }


    private:
        Ui::QtAudioCaptureDlg *ui_;
        std::shared_ptr<KcAudio> audio_;
    };
}


QtAudioCaptureDlg::QtAudioCaptureDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtAudioCaptureDlg),
    timerId_(-1),
    embed_(false)
{
    ui->setupUi(this);
    audio_ = std::make_shared<KcAudio>();
    auto obs = std::make_shared<kPrivate::KcWaveObserver>(ui, audio_);

    capture_ = std::make_unique<KcAudioCapture>();
    capture_->pushBack(obs);

    render_ = std::make_unique<KcAudioRender>();
    render_->pushBack(obs);


    // 初始化设备列表
    KcAudioDevice ad;
    for(unsigned i = 0; i < ad.count(); i++) {
        auto info = ad.info(i);
        if(info.inputChannels > 0) {
            QVariant deviceId(i);
            ui->cbDeviceList->addItem(QString::fromLocal8Bit(info.name), deviceId);
        }
    }

    assert(ui->cbDeviceList->count() > 0);
    ui->cbDeviceList->setCurrentIndex(0);
    syncDeviceInfo_();


    // 连接信号处理槽
    connect(ui->cbDeviceList, qOverload<int>(&QComboBox::currentIndexChanged),
        [=](int index){
        assert(index == ui->cbDeviceList->currentIndex());
        syncDeviceInfo_();
    });

    // 界面美化：设置图标
    setWindowIcon(QApplication::style()->standardIcon((enum QStyle::StandardPixmap)0));

    ui->btStart->setIcon(QIcon(u8":/kRecorder/record"));
    ui->btStop->setIcon(QIcon(u8":/kRecorder/stop"));

    setEmbeddingMode(false); // 默认非嵌入模式
    syncUiState_(kState::ready);
}


QtAudioCaptureDlg::~QtAudioCaptureDlg()
{
    if(render_->running()) render_->stop(true);
    if(capture_->running()) capture_->stop(true);

    delete ui;
}


void QtAudioCaptureDlg::setEmbeddingMode(bool embed)
{
    embed_ = embed;

    if (embed) {
        ui->btOk->setText(tr(u8"确定"));
        ui->btOk->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
        ui->btCancel->setText(tr(u8"取消"));
        ui->btCancel->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
    }
    else {
        ui->btOk->setText(tr(u8"保存..."));
        ui->btOk->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
        ui->btCancel->setText(tr(u8"退出"));
        ui->btCancel->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
    }
}


void QtAudioCaptureDlg::syncDeviceInfo_()
{
    KcAudioDevice ad;
    auto deviceId = ui->cbDeviceList->currentData().toInt();
    auto info = ad.info(deviceId);
    assert(QString::fromLocal8Bit(info.name) == ui->cbDeviceList->currentText());

    auto rate = ui->cbRate->currentText();
    ui->cbRate->clear();
    for(unsigned i = 0; i < info.sampleRates.size(); i++)
        ui->cbRate->addItem(QString::number(info.sampleRates[i]));
    ui->cbRate->setCurrentText(rate);

    ui->rbMono->setEnabled(info.inputChannels > 0);
    ui->rbStereo->setEnabled(info.inputChannels > 1);
    if(ui->rbStereo->isChecked() && info.inputChannels > 1)
        ui->rbStereo->setChecked(true);
    else
        ui->rbMono->setChecked(true);
}


void QtAudioCaptureDlg::syncUiState_(kState state)
{
    ui->cbDeviceList->setEnabled(state == kState::ready);
    ui->cbRate->setEnabled(state == kState::ready);
    ui->rbMono->setEnabled(state == kState::ready);
    ui->rbStereo->setEnabled(state == kState::ready);

    ui->btOk->setEnabled(state == kState::ready && audio_ && audio_->count() > 0);
    ui->btCancel->setEnabled(state == kState::ready);
    ui->btPlay->setEnabled(state != kState::capture && audio_ && audio_->count() > 0); // 允许暂停录音时回放
    ui->btStart->setEnabled(state == kState::ready);
    ui->btStop->setEnabled(state == kState::capture || state == kState::pause);
    ui->btPause->setEnabled(state == kState::capture || state == kState::pause);


    if(state == kState::play) {
        ui->btPlay->setText(tr(u8"停止"));
        ui->btPlay->setIcon(QIcon(u8":/kRecorder/stop_play"));
    }
    else {
        ui->btPlay->setText(tr(u8"播放"));
        ui->btPlay->setIcon(QIcon(u8":/kRecorder/play"));
    }

    if(state == kState::pause) {
        ui->btPause->setText(tr(u8"继续"));
        ui->btPause->setIcon(QIcon(u8":/kRecorder/goon"));
    }
    else {
        ui->btPause->setText(tr(u8"暂停"));
        ui->btPause->setIcon(QIcon(u8":/kRecorder/pause"));
    }
}


// 开始录音
void QtAudioCaptureDlg::on_btStart_clicked()
{
    ui->btStart->setDisabled(true); // 防止重复点击

    auto deviceId = ui->cbDeviceList->currentData().toInt();
    auto sampleRate = ui->cbRate->currentText().toInt();
    auto channels = 0;
    if(ui->rbMono->isChecked())
        channels = 1;
    else if(ui->rbStereo->isChecked())
        channels = 2;

    audio_->reset(0, 0, 1.0 / sampleRate, 0.5);
    audio_->resizeChannel(channels);
    if(!capture_->record(audio_, deviceId)) {
        ui->btStart->setDisabled(false);
        QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(capture_->errorText())); // TODO: fromLocal8Bit???
        return;
    }

    syncUiState_(kState::capture);
}


// 停止录音
void QtAudioCaptureDlg::on_btStop_clicked()
{
    ui->btStop->setDisabled(true);

    assert(capture_ && (capture_->running() || capture_->pausing()));
    if (!capture_->stop(true)) {
        ui->btStop->setDisabled(false);
        QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(capture_->errorText())); // TODO: fromLocal8Bit???
        return;
    }

    syncUiState_(kState::ready);
}


// 暂停/继续录音
void QtAudioCaptureDlg::on_btPause_clicked()
{
    ui->btPause->setDisabled(true);

    if(capture_->running()) { // 暂停
        if (capture_->pause(true)) {
            syncUiState_(kState::pause);
            return;
        }
    }
    else { // 继续
        if (capture_->goon()) {
            syncUiState_(kState::capture);
            return;
        }
    }

    ui->btPause->setDisabled(false);
    QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(capture_->errorText())); // TODO: fromLocal8Bit???
}


// 回放/停止
void QtAudioCaptureDlg::on_btPlay_clicked()
{
    if(render_->running()) { // 停止
        if (render_->stop(true)) {
            if (timerId_ != -1) {
                killTimer(timerId_);
                timerId_ = -1;
            }

            syncUiState_(capture_->pausing() ? kState::pause : kState::ready);
            return;
        }
    }
    else { // 回放
        if(render_->playback(audio_)) {
            syncUiState_(kState::play);
            assert(timerId_ == -1);
            timerId_ = startTimer(100); // 100ms定时器
            return;
        }
    }

    QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(render_->errorText())); // TODO: fromLocal8Bit???
}


// 保存音频
void QtAudioCaptureDlg::on_btOk_clicked()
{
    if (embed_) {
        accept();
    }
    else {
        auto path = QtAudioUtils::getSavePath();
        if (path.isEmpty()) return;

        auto r = audio_->save(path.toLocal8Bit().constData()); // TODO: ???
        if (!r.empty())
            QMessageBox::information(this, u8"错误", QString::fromLocal8Bit(r)); // TODO: fromLocal8Bit???
    }
}


void QtAudioCaptureDlg::on_btCancel_clicked()
{
    reject();
}


void QtAudioCaptureDlg::timerEvent( QTimerEvent *event)
{
    if(event->timerId() == timerId_) {
        if(!render_->running()) {
            killTimer(timerId_);
            timerId_ = -1;
            syncUiState_(capture_->pausing() ? kState::pause : kState::ready);
        }
    }
}

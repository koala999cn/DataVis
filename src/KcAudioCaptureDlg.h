#pragma once
#include <QDialog>
#include <memory>
#include <vector>

class KcAudio;

namespace Ui {
class KcAudioCaptureDlg;
}

class KcAudioCapture;
class KcAudioRender;

class KcAudioCaptureDlg : public QDialog
{
    Q_OBJECT

public:
    explicit KcAudioCaptureDlg(QWidget *parent = nullptr);
    ~KcAudioCaptureDlg();

    // 设置嵌入模式
    // 嵌入模式下，录音对话框显示“确定/取消”按钮
    // 非嵌入模式下，录音对话框显示“保存/退出”按钮
    void setEmbeddingMode(bool embed);

private slots:
    void on_btStart_clicked();
    void on_btPlay_clicked();
    void on_btStop_clicked();
    void on_btPause_clicked();
    void on_btOk_clicked();
    void on_btCancel_clicked();

private:
    void syncDeviceInfo_();

    enum class kState { capture, play, pause, ready };
    void syncUiState_(kState state);

    // 用于追踪回放是否结束的定时器
    void timerEvent(QTimerEvent* event) override;

public:
    std::shared_ptr<KcAudio> audio_; // 录制的音频

private:
    Ui::KcAudioCaptureDlg *ui; // 录制界面
    std::unique_ptr<KcAudioCapture> capture_; // 录制设备
    std::unique_ptr<KcAudioRender> render_; // 播放设备，用于试听
    int timerId_; // 定时器事件id
    bool embed_; // 是否嵌入式模式
};



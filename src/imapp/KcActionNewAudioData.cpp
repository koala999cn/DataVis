#include "KcActionNewAudioData.h"
#include "KcAudio.h"
#include "KsImApp.h"
#include "KgPipeline.h"
#include "prov/KcPvData.h"
#include "KuPathUtil.h"


KcActionNewAudioData::KcActionNewAudioData()
    : KcActionShowFileDialog(KcActionShowFileDialog::KeType::k_open, 
        "Audio File",
        "wav file (*.wav){.wav}") // TODO: ��ʱֻ֧��wav�ļ�
{

}


void KcActionNewAudioData::update()
{
    super_::update();
    if (done()) {
        auto audio = std::make_shared<KcAudio>();
        auto errText = audio->load(result());
        if (!errText.empty()) {
            state_ = KeState::k_failed;

            // TODO: show the error message
        }
        else {
            auto node = std::make_shared<KcPvData>(KuPathUtil::fileName(result()), audio);
            KsImApp::singleton().pipeline().insertNode(node);
        }
    }
}
#include "KcAudio.h"
#include "KgAudioFile.h"


std::string KcAudio::load(const std::string& path)
{
    KgAudioFile sf;

    if(!sf.open(path, KgAudioFile::k_read)) return sf.errorText();

    auto dx = static_cast<kReal>(1) / sf.sampleRate();
    reset(dx, sf.channels(), sf.frames());

    if (sf.read(data_.data(), sf.frames()) != sf.frames())
        return sf.errorText();

    return "";
}


std::string KcAudio::save(const std::string& path, int quality)
{
	KgAudioFile sf(channels(), static_cast<unsigned>(samplingRate()), count());

	if (!sf.open(path, KgAudioFile::KeOpenMode(quality + 1)))
        return sf.errorText();

    assert(sf.channels() == channels());
    if (sf.write(data_.data(), count()) != count())
        return sf.errorText();

	return "";
}

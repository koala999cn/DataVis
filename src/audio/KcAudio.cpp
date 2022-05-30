#include "KcAudio.h"
#include "KgAudioFile.h"


std::string KcAudio::load(const std::string& path)
{
    KgAudioFile sf;

    if(!sf.open(path, KgAudioFile::k_read)) return sf.errorText();

    auto dx = static_cast<kReal>(1) / sf.sampleRate();
    reset(0, 0, dx, 0.5);
    auto frames = sf.frames();
    resize(frames, sf.channels());

    if (sf.read(data(), sf.frames()) != sf.frames())
        return sf.errorText();

    return "";
}


std::string KcAudio::save(const std::string& path, int quality)
{
	KgAudioFile sf(channels(), static_cast<unsigned>(sampleRate()), size());

	if (!sf.open(path, KgAudioFile::KeOpenMode(quality + 1)))
        return sf.errorText();

    assert(sf.channels() == channels());
    if (sf.write(data(), size()) != size())
        return sf.errorText();

	return "";
}

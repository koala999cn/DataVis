#pragma once
#include <string>
#include <vector>
#include "kDsp.h"


class KgAudioFile
{
public:
    KgAudioFile();
    KgAudioFile(kIndex channels, unsigned sampleRate, kIndex frames);
    ~KgAudioFile();

    enum KeOpenMode {
        k_read,
        k_write_very_low_quality = 1,
        k_write_low_quality,
        k_write_normal_quality,
        k_write_high_quality,
        k_write_very_high_quality
    };

    // @mode: 取KeOpenMode值
    bool open(const std::string& path, int mode);
    bool isOpen() const;

	/* Seek within the waveform data chunk of the SNDFILE. 
	** An offset of zero with whence set to SEEK_SET will position the
	** read / write pointer to the first data sample.
	** On success sf_seek returns the current position in (multi-channel)
	** samples from the start of the file.
	** On error all of these functions return -1.
	*/
    // @where: uses the same values for whence(SEEK_SET, SEEK_CUR and SEEK_END) as stdio.h function fseek().
    kIndex seek(kIndex frames, int where);

    void close();

    auto channels() const { return channels_; }
    auto sampleRate() const { return sampleRate_; }
    auto frames() const { return frames_; }

    // buf.size >= frames*channels
    kIndex read(kReal* buf, kIndex frames);

    kIndex write(const kReal* buf, kIndex frames);

    // bits per second
    int bps() const;


    const char* errorText() const { return error_.c_str(); }

    static int getSupportTypeCount();
    static const char* getTypeExtension(int nIndex);
    static const char* getTypeDescription(int nIndex);

private:

    // 封装重采样实现
    kIndex write_(const kReal* buf, kIndex frames);

    kIndex writeDirect_(const kReal* buf, kIndex frames);

private:
    void* snd_;
    void* resampler_;
    std::vector<void*> pstates_;

    kIndex channels_;
    unsigned sampleRate_;
    kIndex frames_; // TODO: ???

    std::string error_;
};


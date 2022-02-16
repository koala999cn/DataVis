#include "KgAudioFile.h"
#include "libsndfile/sndfile.h"
#include "smarc/smarc.h"
#include <memory.h>
#include <assert.h>
#include <algorithm>
#include "KcAudioDevice.h"


/****************************************************************
 * libsndfile支持的格式，由libsndfile库自带的list-format生成

Version : libsndfile-1.0.31

    AIFF (Apple/SGI)  (extension "aiff")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law
       IMA ADPCM
       GSM 6.10
       12 bit DWVW
       16 bit DWVW
       24 bit DWVW

    AU (Sun/NeXT)  (extension "au")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law
       32kbs G721 ADPCM
       24kbs G723 ADPCM
       40kbs G723 ADPCM

    AVR (Audio Visual Research)  (extension "avr")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Unsigned 8 bit PCM

    CAF (Apple Core Audio File)  (extension "caf")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law
       16 bit ALAC
       20 bit ALAC
       24 bit ALAC
       32 bit ALAC

    FLAC (Free Lossless Audio Codec)  (extension "flac")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM

    HTK (HMM Tool Kit)  (extension "htk")
       Signed 16 bit PCM

    IFF (Amiga IFF/SVX8/SV16)  (extension "iff")
       Signed 8 bit PCM
       Signed 16 bit PCM

    MAT4 (GNU Octave 2.0 / Matlab 4.2)  (extension "mat")
       Signed 16 bit PCM
       Signed 32 bit PCM
       32 bit float
       64 bit float

    MAT5 (GNU Octave 2.1 / Matlab 5.0)  (extension "mat")
       Signed 16 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float

    MPC (Akai MPC 2k)  (extension "mpc")
       Signed 16 bit PCM

    OGG (OGG Container format)  (extension "oga")
       Vorbis
       Opus

    PAF (Ensoniq PARIS)  (extension "paf")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM

    PVF (Portable Voice Format)  (extension "pvf")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 32 bit PCM

    RAW (header-less)  (extension "raw")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law
       GSM 6.10
       12 bit DWVW
       16 bit DWVW
       24 bit DWVW
       VOX ADPCM
       16kbs NMS ADPCM
       24kbs NMS ADPCM
       32kbs NMS ADPCM

    RF64 (RIFF 64)  (extension "rf64")
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law

    SD2 (Sound Designer II)  (extension "sd2")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM

    SDS (Midi Sample Dump Standard)  (extension "sds")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM

    SF (Berkeley/IRCAM/CARL)  (extension "sf")
       Signed 16 bit PCM
       Signed 32 bit PCM
       32 bit float
       U-Law
       A-Law

    VOC (Creative Labs)  (extension "voc")
       Signed 16 bit PCM
       Unsigned 8 bit PCM
       U-Law
       A-Law

    W64 (SoundFoundry WAVE 64)  (extension "w64")
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law
       IMA ADPCM
       Microsoft ADPCM
       GSM 6.10

    WAV (Microsoft)  (extension "wav")
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law
       IMA ADPCM
       Microsoft ADPCM
       GSM 6.10
       32kbs G721 ADPCM
       16kbs NMS ADPCM
       24kbs NMS ADPCM
       32kbs NMS ADPCM

    WAV (NIST Sphere)  (extension "wav")
       Signed 8 bit PCM
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       U-Law
       A-Law

    WAVEX (Microsoft)  (extension "wav")
       Signed 16 bit PCM
       Signed 24 bit PCM
       Signed 32 bit PCM
       Unsigned 8 bit PCM
       32 bit float
       64 bit float
       U-Law
       A-Law

    WVE (Psion Series 3)  (extension "wve")
       A-Law

    XI (FastTracker 2)  (extension "xi")
       16 bit DPCM
       8 bit DPCM

***********************************************/

namespace kPrivate
{
    constexpr kIndex k_block_size = 8192; // 一次写入的采样点数目

    struct KpTypeDesc
    {
        const char* ext;
        const char* desc;
        int channels; // 支持的最大声道数. <=0表示无限制
        int format;
    };

 
    static KpTypeDesc k_supportedTypeDesc[] =
    {
        { "wav",       "Microsoft Wave",                    0,   SF_FORMAT_WAV },
        { "opus",      "Xiph/Skype Opus encoding",          0,   SF_FORMAT_OGG | SF_FORMAT_OPUS },
        { "ogg|oga",   "Ogg Vorbis (Xiph Foundation)",      0,   SF_FORMAT_OGG | SF_FORMAT_VORBIS },
        { "flac",      "Free Lossless Audio Codec",         8,   SF_FORMAT_FLAC }, 
        { "aiff|aifc", "Apple/SGI AIFF",                    0,   SF_FORMAT_AIFF }, // TODO: 写入的文件windows播放器无法播放
        { "au|snd",    "Sun/DEC/NeXT Audio",                0,   SF_FORMAT_AU },
        { "raw",       "Headerless 32-bit IEEE float",      0,   SF_FORMAT_RAW | SF_FORMAT_FLOAT },
        { "gsm",       "Headerless GSM6.10",                0,   SF_FORMAT_RAW | SF_FORMAT_GSM610}, // GSM 6.10 data format only supports 8kHz sample rate
        { "vox",       "OKI Dialogic VOX ADPCM",            0,   SF_FORMAT_RAW | SF_FORMAT_VOX_ADPCM },
        { "paf|fap",   "Ensoniq Paris Audio",               0,   SF_FORMAT_PAF },
        { "svx|iff",   "Commodore Amiga IFF/SVX8/SV16",     1,   SF_FORMAT_SVX | SF_ENDIAN_BIG }, 
        { "nist|sph",  "NIST Sphere WAV",                   0,   SF_FORMAT_NIST },
        { "sf|ircam",  "Berkeley/IRCAM/CARL Sound",         256, SF_FORMAT_IRCAM },
        { "voc",       "Creative Labs Voice",               2,   SF_FORMAT_VOC | SF_ENDIAN_LITTLE }, 
        { "w64",       "Sonic Foundry's 64 bit RIFF/WAV",   0,   SF_FORMAT_W64 | SF_ENDIAN_LITTLE },
        { "mat|mat4",  "GNU Octave 2.0/Matlab 4.2",         0,   SF_FORMAT_MAT4 },
        { "mat5",      "GNU Octave 2.1/Matlab 5.0)",        0,   SF_FORMAT_MAT5 },
        { "pvf",       "Portable Voice Format",             0,   SF_FORMAT_PVF },
        { "xi",        "Fasttracker 2 Extended Instrument", 1,   SF_FORMAT_XI}, 
        { "htk",       "HMM Tool Kit Format",               1,   SF_FORMAT_HTK | SF_FORMAT_PCM_16 | SF_ENDIAN_BIG }, 
        { "sds",       "Midi Sample Dump Standard",         1,   SF_FORMAT_SDS | SF_ENDIAN_BIG }, 
        { "avr",       "Audio Visual Research",             2,   SF_FORMAT_AVR | SF_ENDIAN_BIG }, 
        { "caf",       "Apple Core Audio Format",           0,   SF_FORMAT_CAF },
        { "wavex",	   "Microsoft WAVEX",               	0,   SF_FORMAT_WAVEX | SF_ENDIAN_LITTLE },
        { "sd2",       "Sound Designer II",                 0,   SF_FORMAT_SD2 | SF_ENDIAN_BIG },
        { "wve|prc",   "Psion Series 3",                    1,   SF_FORMAT_WVE | SF_FORMAT_ALAW | SF_ENDIAN_LITTLE }, 
        { "mpc",       "Akai MPC 2000 sampler",             2,   SF_FORMAT_MPC2K | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE },
        { "rf64",      "RF64 WAV file",                     0,   SF_FORMAT_RF64 | SF_ENDIAN_LITTLE }
    };

    // 从文件名判定音频格式
    static int formatFromFileName(const std::string& fn)
    {
        int format(0);
        std::string::size_type i = fn.find_last_of('.');
        auto ext = (i == std::string::npos) ? "" : fn.substr(i + 1);
        if (ext.empty())
            return SF_FORMAT_RAW;

        for (auto& s : ext)
            s = std::tolower(s);

        for (int i = 0; i < KgAudioFile::getSupportTypeCount(); i++) {
            std::string exts = KgAudioFile::getTypeExtension(i);
            if(exts.find(ext) < exts.size()) {
                format = kPrivate::k_supportedTypeDesc[i].format;
                break;    
            }
        }

        // 单独处理PAF和AIFF类型
        if (format == SF_FORMAT_PAF) {
            if (ext == "paf")
                format |= SF_ENDIAN_BIG;
            else if (ext == "fap")
                format |= SF_ENDIAN_LITTLE;
        }
        else if (SF_FORMAT_AIFF) {
            if(ext == "aifc")
                format |= SF_FORMAT_FLOAT;
        }

        return format;
    }


    static const char* nameOfSubType(int format)
    {
        //return ::sfe_codec_name(format);

        switch (format & SF_FORMAT_SUBMASK)
        {
        case SF_FORMAT_PCM_S8: return "signed 8 bit PCM";
        case SF_FORMAT_PCM_16: return "16 bit PCM";
        case SF_FORMAT_PCM_24: return "24 bit PCM";
        case SF_FORMAT_PCM_32: return "32 bit PCM";
        case SF_FORMAT_PCM_U8: return "unsigned 8 bit PCM";
        case SF_FORMAT_FLOAT: return "32 bit float";
        case SF_FORMAT_DOUBLE: return "64 bit double";
        case SF_FORMAT_ULAW: return "u-law";
        case SF_FORMAT_ALAW: return "a-law";
        case SF_FORMAT_IMA_ADPCM: return "IMA ADPCM"; // channels <= 2
        case SF_FORMAT_MS_ADPCM: return "MS ADPCM"; // channels <= 2
        case SF_FORMAT_GSM610: return "gsm610"; // mono only
        case SF_FORMAT_VOX_ADPCM: return "Vox ADPCM"; // mono only
        case SF_FORMAT_G721_32: return "g721 32kbps"; // mono only
        case SF_FORMAT_G723_24: return "g723 24kbps"; // mono only
        case SF_FORMAT_G723_40: return "g723 40kbps"; // mono only
        case SF_FORMAT_DWVW_12: return "12 bit DWVW"; // mono only
        case SF_FORMAT_DWVW_16: return "16 bit DWVW"; // mono only
        case SF_FORMAT_DWVW_24: return "24 bit DWVW"; // mono only TODO: 源代码此处错误，错将24写成14
        case SF_FORMAT_DWVW_N: return "DWVW";
        case SF_FORMAT_NMS_ADPCM_16: return "16 bit ADPCM"; // mono  TODO: 
        case SF_FORMAT_NMS_ADPCM_24: return "24 bit ADPCM"; // mono only
        case SF_FORMAT_NMS_ADPCM_32: return "32 bit ADPCM"; // mono only
        case SF_FORMAT_DPCM_8: return "8 bit DPCM";
        case SF_FORMAT_DPCM_16: return "16 bit DPCM";
        case SF_FORMAT_VORBIS: return "Vorbis";
        case SF_FORMAT_ALAC_16: return "16 bit ALAC";
        case SF_FORMAT_ALAC_20: return "20 bit ALAC";
        case SF_FORMAT_ALAC_24: return "24 bit ALAC";
        case SF_FORMAT_ALAC_32: return "32 bit ALAC";
        case SF_FORMAT_OPUS: return "Opus";
        default: break;
        };

        return "unknown";
    }


    // 从音频格式format和品质参数quality，推算适合的音频子类型
    // quality从0到4，品质从低到高，数值2代表normal水平
    // TODO: 考虑子类型可支持的声道数
    static int preferredSubType(int format, int quality)
    {
        int subtype = 0;
        switch (format & SF_FORMAT_TYPEMASK)
        {
        case SF_FORMAT_WAV: // PCM非压缩
        case SF_FORMAT_RAW:
        case SF_FORMAT_AIFF:
        case SF_FORMAT_W64:
        case SF_FORMAT_CAF:
            if (quality == 0)
                subtype = (format == SF_FORMAT_WAV || format == SF_FORMAT_W64) ?
                     SF_FORMAT_PCM_U8 : SF_FORMAT_PCM_S8; // WAV和W64只支持U8，其他有的格式只支持S8
            else if (quality == 1)
                subtype = SF_FORMAT_PCM_16;
            else if (quality == 2)
                subtype = SF_FORMAT_PCM_24;
            else if (quality == 3)
                subtype = SF_FORMAT_PCM_32;
            else
                subtype = SF_FORMAT_DOUBLE;
            break;

        case SF_FORMAT_AU: // 使用压缩
            if (quality == 0)
                subtype = SF_FORMAT_ULAW;
            else if (quality == 1)
                subtype = SF_FORMAT_G723_24; 
            else if (quality == 2)
                subtype = SF_FORMAT_G721_32; 
            else
                subtype = SF_FORMAT_G723_40;
            break;

        case SF_FORMAT_PAF:
        case SF_FORMAT_SD2:
        case SF_FORMAT_FLAC:
            if (quality < 2)
                subtype = SF_FORMAT_PCM_S8;
            else if (quality == 2)
                subtype = SF_FORMAT_PCM_16;
            else 
                subtype = SF_FORMAT_PCM_24;
            break;

        case SF_FORMAT_SVX:
            if (quality < 2)
                subtype = SF_FORMAT_PCM_S8;
            else 
                subtype = SF_FORMAT_PCM_16;
            break;

        case SF_FORMAT_NIST:
            if (quality == 0)
                subtype = SF_FORMAT_PCM_S8;
            else if (quality == 1)
                subtype = SF_FORMAT_PCM_16;
            else if (quality == 2)
                subtype = SF_FORMAT_PCM_24;
            else
                subtype = SF_FORMAT_PCM_32;
            break;

        case SF_FORMAT_IRCAM:
            if (quality <= 2)
                subtype = SF_FORMAT_PCM_16;
            else
                subtype = SF_FORMAT_PCM_32;
            break;

        case SF_FORMAT_VOC:
            if (quality < 2)
                subtype = SF_FORMAT_PCM_U8;
            else
                subtype = SF_FORMAT_PCM_16;
            break;

        case SF_FORMAT_MAT4:
        case SF_FORMAT_MAT5:
            if (quality == 0)
                subtype = (format == SF_FORMAT_MAT5) ? SF_FORMAT_PCM_U8 : SF_FORMAT_PCM_16; // MAT4不支持8bit编码
            else if (quality <= 2)
                subtype = SF_FORMAT_PCM_16;
            else if (quality == 3)
                subtype = SF_FORMAT_PCM_32;
            else
                subtype = SF_FORMAT_DOUBLE;
            break;

        case SF_FORMAT_PVF:
            if (quality < 2)
                subtype = SF_FORMAT_PCM_S8;
            else if (quality == 2)
                subtype = SF_FORMAT_PCM_16;
            else
                subtype = SF_FORMAT_PCM_32;
            break;

        case SF_FORMAT_XI:
            subtype = quality < 2 ? SF_FORMAT_DPCM_8 : SF_FORMAT_DPCM_16;
            break;

        default: // subtype = 0;
            break;
        };
 
        return subtype;
    }


    // Opus only supprt sample rates of 8000, 12000, 16000, 24000, 48000
    // 该函数根据预期的采样频率rate，返回一个适合opus编码的采样频率
    static unsigned preferredOpusSampleRate(unsigned rate) 
    {
        if (rate >= KcAudioDevice::k_44k)
            return KcAudioDevice::k_48k;
        else if (rate >= KcAudioDevice::k_22k)
            return KcAudioDevice::k_24k;
        else if (rate >= KcAudioDevice::k_16k)
            return KcAudioDevice::k_16k;
        else if (rate >= KcAudioDevice::k_11k)
            return KcAudioDevice::k_12k;

        return KcAudioDevice::k_8k;
    }
}


KgAudioFile::KgAudioFile()
{
    snd_ = nullptr;
    resampler_ = nullptr;
    channels_ = 1;
    sampleRate_ = KcAudioDevice::k_44k;
    frames_ = 0;
}


KgAudioFile::KgAudioFile(kIndex channels, unsigned sampleRate, kIndex frames)
{
    snd_ = nullptr;
    resampler_ = nullptr;
    channels_ = channels;
    sampleRate_ = sampleRate;
    frames_ = frames;
}


KgAudioFile::~KgAudioFile()
{
    if(isOpen()) close();
}


bool KgAudioFile::open(const std::string& path, int mode)
{
    if (isOpen()) close();

    SF_INFO si;
    ::memset(&si, 0, sizeof(si));
    si.channels = channels_;
    si.samplerate = sampleRate_;
    si.frames = frames_;
    si.format = 0;
    if (mode != k_read) {
        si.format = kPrivate::formatFromFileName(path); // 读模式时留给sf_open自动检测
        if (si.format == 0)
            si.format = SF_FORMAT_WAV; // 缺省写入WAV格式

        if ((si.format & SF_FORMAT_SUBMASK) == 0)
            si.format |= kPrivate::preferredSubType(si.format, mode - 1);

        if ((si.format & SF_FORMAT_SUBMASK) == SF_FORMAT_OPUS)
            si.samplerate = kPrivate::preferredOpusSampleRate(sampleRate_);
    }

    snd_ = ::sf_open(path.c_str(), mode == k_read ? SFM_READ : SFM_WRITE, &si);
    if (snd_ == nullptr) {
        error_ = ::sf_strerror(0);
        return false;
    }
        
    double quality = static_cast<double>(mode - 1) / (k_write_very_high_quality - 1);
    if (quality > 1) quality = 1;

    if (mode != k_read) {
        auto major = si.format & SF_FORMAT_TYPEMASK;
        auto minor = si.format & SF_FORMAT_SUBMASK;

        if (major == SF_FORMAT_FLAC || minor == SF_FORMAT_OPUS) {
            /* Set the Variable Bit Rate encoding quality.The encoding quality value
               should be between 0.0 (lowest quality) and 1.0 (highest quality).
               Currenly this command is only implemented for FLAC and Ogg / Vorbis files.
               It has no effect on un - compressed file formats. */
            ::sf_command((SNDFILE*)snd_, SFC_SET_VBR_ENCODING_QUALITY, &quality, sizeof(quality));
        }

        if (major == SF_FORMAT_FLAC || major == SF_FORMAT_OGG) {
            /* Set the compression level.The compression level should be between
               0.0 (minimum compression level) and 1.0 (highest compression level).
               Currenly this command is only implemented for FLAC and Ogg / Vorbis files.
               It has no effect on uncompressed file formats. */
            double level = 1 - quality;
            ::sf_command((SNDFILE*)snd_, SFC_SET_COMPRESSION_LEVEL, &level, sizeof(level));
        }
    }


    // TODO: 1. sf_open失败还是会创建文件； 2. 覆盖已存在文件时宕机. 均针对Opus格式测试

    channels_ = si.channels;
    frames_ = (long)si.frames;
    if(mode == k_read)
        sampleRate_ = si.samplerate;  // 写模式下，内外采样频率可以不一致，封装缺省的resampler实现
    
    if (sampleRate_ != si.samplerate) {
        assert(resampler_ == nullptr);

        const double bandwidth = 0.95;  // bandwidth
        const double rp = 0.1; // passband ripple factor
        const double rs = 140; // stopband attenuation
        const double tol = 0.000001; // tolerance

        auto pfilt = ::smarc_init_pfilter(sampleRate_, si.samplerate, bandwidth, rp, rs, tol, nullptr, 0);
        if (pfilt == nullptr) {
            error_ = "failed to initialize resampler";
            return false;
        }

        pstates_.resize(channels_);
        for(kIndex c = 0; c < channels_; c++)
            pstates_[c] = ::smarc_init_pstate(pfilt); // 各声道分别创建自己的滤波器状态
        resampler_ = pfilt;
    }

    return true;
}


bool KgAudioFile::isOpen() const
{
    return snd_ != nullptr;
}


kIndex KgAudioFile::seek(kIndex frames, int where)
{
    return (kIndex)::sf_seek((SNDFILE*)snd_, frames, where);
}


void KgAudioFile::close()
{
    if (resampler_) {
        // flush resampler remaining values
        std::vector<kReal> outBuf(kPrivate::k_block_size * channels_);
        std::vector<kReal> mono(kPrivate::k_block_size);
        while (true) {       
            int written(0);
            for (kIndex c = 0; c < channels_; c++) {
                written = ::smarc_resample_flush((PFilter*)resampler_, (PState*)pstates_[c],
                                  &mono[0], kPrivate::k_block_size);
                for (int i = 0; i < written; i++)
                    outBuf[i * channels_ + c] = mono[i];
            }
            writeDirect_(outBuf.data(), written);
            if (written < kPrivate::k_block_size)
                break;
        }

        // release smarc filter state
        for(auto s : pstates_)
            ::smarc_destroy_pstate((PState*)s);
        pstates_.clear();

        // release smarc filter
        ::smarc_destroy_pfilter((PFilter*)resampler_);
        resampler_ = nullptr;
    }

    ::sf_close((SNDFILE*)snd_);
    snd_ = nullptr;
}


kIndex KgAudioFile::read(kReal *buf, kIndex frames)
{
    if (std::is_same<kReal, float>::value)
        return (kIndex)::sf_readf_float((SNDFILE*)snd_, (float*)buf, frames);
    else if (std::is_same<kReal, double>::value)
        return (kIndex)::sf_readf_double((SNDFILE*)snd_, (double*)buf, frames);
    else {
        assert(false);
        return 0;
    }
}


kIndex KgAudioFile::write(const kReal* buf, kIndex frames)
{
    // 分块写入，以免一次写入大量数据导致堆栈溢出
    kIndex written(0); // 已写入的采样点数
    while (written < frames) {
        auto trueBuf = std::min(kPrivate::k_block_size, frames - written);
        trueBuf = write_(buf, trueBuf); // 必要时进行重采样
        if (trueBuf == 0)
            break;
        written += trueBuf;
        buf += trueBuf * channels();
    }

    return written; // 若重采样，written可能不等于frames
}


kIndex KgAudioFile::write_(const kReal* buf, kIndex frames)
{
    const kReal* buf_ = buf;
    kIndex frames_ = frames;
    std::vector<kReal> outBuf;

    if (resampler_) { // 重采样
        auto outFrames = ::smarc_get_output_buffer_size((PFilter*)resampler_, frames);
        outBuf.resize(outFrames * channels_);
        buf_ = outBuf.data();
        if (channels_ == 1) { //对单声道优化
            frames_ = ::smarc_resample((PFilter*)resampler_, (PState*)pstates_[0], buf, frames, &outBuf[0], outFrames);
        }
        else { // 需要对多声道数据进行分解和重组
            // resample each channels. TODO: 考虑通道数据的交叉和非交叉两种组合模式
            std::vector<kReal> monoIn(frames), monoOut(outFrames);
            for (kIndex c = 0; c < channels_; c++) {
                for (kIndex i = 0; i < frames; i++)
                    monoIn[i] = buf[i * channels_ + c];
                frames_ = ::smarc_resample((PFilter*)resampler_, (PState*)pstates_[c],
                    monoIn.data(), frames, &monoOut[0], outFrames);
                //assert (frames_ == outFrames); // TODO: 每次返回值应该一致
                for (int i = 0; i < frames_; i++)
                    outBuf[i * channels_ + c] = monoOut[i];
            }
        }
    }

    return writeDirect_(buf_, frames_);
}


kIndex KgAudioFile::writeDirect_(const kReal* buf, kIndex frames)
{
    if (std::is_same<kReal, float>::value)
        return (kIndex)::sf_writef_float((SNDFILE*)snd_, (float*)buf, frames);
    else if (std::is_same<kReal, double>::value)
        return (kIndex)::sf_writef_double((SNDFILE*)snd_, (double*)buf, frames);
    else {
        assert(false);
        return 0;
    }
}


int KgAudioFile::bps() const
{
    return ::sf_current_byterate((SNDFILE*)snd_) * 8;
}

int KgAudioFile::getSupportTypeCount()
{
	return sizeof(kPrivate::k_supportedTypeDesc)/sizeof(kPrivate::KpTypeDesc);
}


const char* KgAudioFile::getTypeExtension(int index)
{
    return kPrivate::k_supportedTypeDesc[index].ext;
}


const char* KgAudioFile::getTypeDescription(int index)
{
    return kPrivate::k_supportedTypeDesc[index].desc;
}

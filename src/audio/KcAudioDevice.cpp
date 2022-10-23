#include "KcAudioDevice.h"
#include "rtaudio/RtAudio.h"
#include <thread>
#include <algorithm>
#include <assert.h>


#if defined(_MSC_VER)
    #pragma comment(lib, "dsound.lib")
#endif

#define RTAudio_ ((RtAudio*)device_)

//! RtAudio callback function prototype.
/*!
   All RtAudio clients must create a function of type RtAudioCallback
   to read and/or write data from/to the audio stream.  When the
   underlying audio system is ready for new input or output data, this
   function will be invoked.

   \param outputBuffer For output (or duplex) streams, the client
          should write \c nFrames of audio sample frames into this
          buffer.  This argument should be recast to the datatype
          specified when the stream was opened.  For input-only
          streams, this argument will be NULL.

   \param inputBuffer For input (or duplex) streams, this buffer will
          hold \c nFrames of input audio sample frames.  This
          argument should be recast to the datatype specified when the
          stream was opened.  For output-only streams, this argument
          will be NULL.

   \param frames The number of sample frames of input or output
          data in the buffers.  The actual buffer size in bytes is
          dependent on the data type and number of channels in use.

   \param streamTime The number of seconds that have elapsed since the
          stream was started.

   \param status If non-zero, this argument indicates a data overflow
          or underflow condition for the stream.  The particular
          condition can be determined by comparison with the
          RtAudioStreamStatus flags.

   \param userData A pointer to optional data provided by the client
          when opening the stream (default = NULL).

   To continue normal stream operation, the RtAudioCallback function
   should return a value of zero.  To stop the stream and drain the
   output buffer, the function should return a value of one.  To abort
   the stream immediately, the client should return a value of two.
 */
namespace kPrivate
{
    int RtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned frames,
                        double streamTime, RtAudioStreamStatus, void *userData)
    {
        return ((KcAudioDevice*)userData)->notify(outputBuffer, inputBuffer, frames, streamTime)
                    ? 0 : 1;
    }
}


KcAudioDevice::KcAudioDevice(void) : inChannels_(0), outChannels_(0), frameSamples_(0)
{
    device_ = new RtAudio();
}


KcAudioDevice::~KcAudioDevice(void)
{
    if(running())
        stop(true);

    if(opened())
        close();

    delete RTAudio_;
}


unsigned KcAudioDevice::count() const
{
    return RTAudio_->getDeviceCount();
}


KcAudioDevice::KpDeviceInfo KcAudioDevice::info(unsigned device) const
{
    RtAudio::DeviceInfo rtinfo;
    KpDeviceInfo di;

    try {
        rtinfo = RTAudio_->getDeviceInfo(device);
    }
    catch (RtAudioError& err) {
        error_ = err.getMessage();
        di.name = "null device";
        di.inputChannels = 0;
        di.outputChannels = 0;
        di.duplexChannels = 0;
        return di;
    }

    
    di.name = rtinfo.name;
    di.inputChannels = rtinfo.inputChannels;
    di.outputChannels = rtinfo.outputChannels;
    di.duplexChannels = rtinfo.duplexChannels;

    for(unsigned n = 0; n < rtinfo.sampleRates.size(); n++)
        di.sampleRates.push_back(rtinfo.sampleRates[n]);

    return di;
}


unsigned KcAudioDevice::defaultInput() const
{
    // return RTAudio_->getDefaultInputDevice(); // 不能正常工作

    unsigned firstDevice = -1;

    for (unsigned idx = 0; idx < count(); idx++) {
        RtAudio::DeviceInfo rtinfo = RTAudio_->getDeviceInfo(idx);
        if (rtinfo.probed && rtinfo.inputChannels > 0) {
            if (firstDevice == -1)
                firstDevice = idx;
            if (rtinfo.isDefaultInput)
                return idx;
        }
    }

    return firstDevice;
}


unsigned KcAudioDevice::defaultOutput() const
{
    //return RTAudio_->getDefaultOutputDevice();

    unsigned firstDevice = -1;

    for (unsigned idx = 0; idx < count(); idx++) {
        RtAudio::DeviceInfo rtinfo = RTAudio_->getDeviceInfo(idx);
        if (rtinfo.probed && rtinfo.outputChannels > 0) {
            if (firstDevice == -1)
                firstDevice = idx;
            if (rtinfo.isDefaultOutput)
                return idx;
        }
    }

    return firstDevice;
}


bool KcAudioDevice::hasInput() const
{
    for(unsigned idx = 0; idx < count(); idx++) {
        RtAudio::DeviceInfo rtinfo = RTAudio_->getDeviceInfo(idx);
        if(rtinfo.probed && rtinfo.inputChannels > 0)
            return true;
    }

    return false;
}


bool KcAudioDevice::hasOutput() const
{
    for(unsigned idx = 0; idx < count(); idx++) {
        RtAudio::DeviceInfo rtinfo = RTAudio_->getDeviceInfo(idx);
        if(rtinfo.probed && rtinfo.outputChannels > 0)
            return true;
    }

    return false;
}


unsigned KcAudioDevice::getDevice(const std::string& name) const
{
    for(unsigned idx = 0; idx < count(); idx++) {
        RtAudio::DeviceInfo rtinfo = RTAudio_->getDeviceInfo(idx);
        if(rtinfo.probed && rtinfo.name == name)
            return idx;
    }

    return -1;
}


unsigned KcAudioDevice::preferredSampleRate(unsigned deviceId) const
{
    unsigned preferredSampleRate(0);
    auto rtinfo = RTAudio_->getDeviceInfo(deviceId);
    if (rtinfo.probed) {
        preferredSampleRate = rtinfo.preferredSampleRate;
        if (preferredSampleRate == 0)
            preferredSampleRate = rtinfo.sampleRates.front();
    }

    return preferredSampleRate;
}


bool KcAudioDevice::open(const KpStreamParameters *outputParameters, const KpStreamParameters *inputParameters,
                                KeSampleFormat format, unsigned sampleRate, unsigned& bufferFrames)
{
	RtAudio::StreamParameters iParams, oParams;
	RtAudio::StreamParameters *pkiParams(0), *pkoParams(0);

    if(outputParameters) {
		pkoParams = &oParams;
		oParams.deviceId = outputParameters->deviceId;
        outChannels_ = oParams.nChannels = outputParameters->channels;
	}

	if(inputParameters)	{
		pkiParams = &iParams;
		iParams.deviceId = inputParameters->deviceId;
        inChannels_ = iParams.nChannels = inputParameters->channels;
	}

	try	{
        RTAudio_->openStream(pkoParams, pkiParams, format, sampleRate, &bufferFrames,
                             kPrivate::RtAudioCallback, this);
        frameSamples_ = bufferFrames;
	}
    catch(RtAudioError& err) {
        error_ = err.getMessage();
		return false;
	}

	return true;
}


bool KcAudioDevice::close()
{
    try	{
        RTAudio_->closeStream();
    }
    catch(RtAudioError& err) {
        error_ = err.getMessage();
        return false;
    }

    return true;
}


bool KcAudioDevice::start()
{
    reset(); // 允许向观察者发送消息

    try	{
        RTAudio_->startStream();
    }
    catch(RtAudioError& err) {
        error_ = err.getMessage();
        return false;
    }

    return true;
}


bool KcAudioDevice::stop(bool wait_)
{
    try	{
        RTAudio_->stopStream();
        if(wait_) wait();
    }
    catch(RtAudioError& err) {
        error_ = err.getMessage();
        return false;
    }

    freeze(); // 等待所有notify进程结束

    return true;
}


bool KcAudioDevice::abort(bool wait_)
{
    try	{
        RTAudio_->abortStream();
        if(wait_) wait();
    }
    catch(RtAudioError& err) {
        error_ = err.getMessage();
        return false;
    }

    freeze(); // 等待所有notify进程结束

    return true;
}


bool KcAudioDevice::opened() const
{
    return RTAudio_->isStreamOpen();
}


bool KcAudioDevice::running() const
{
    return RTAudio_->isStreamRunning();
}


void KcAudioDevice::wait() const
{
    while (running()) 
        std::this_thread::yield();
}


unsigned KcAudioDevice::sampleRate() const
{
    return RTAudio_->getStreamSampleRate();
}


double KcAudioDevice::frameDuration() const
{
    return double(frameSamples()) / sampleRate();
}


double KcAudioDevice::getStreamTime()
{
    return RTAudio_->getStreamTime();
}


void KcAudioDevice::setStreamTime(double time)
{
    RTAudio_->setStreamTime(time);
}


long KcAudioDevice::latency()
{
    return RTAudio_->getStreamLatency();
}


unsigned KcAudioDevice::bestMatch(unsigned deviceId, unsigned sampleRate, int prefer)
{
    if (deviceId > count())
        return 0;

    auto di = info(deviceId);
    auto& rates = di.sampleRates;
    std::sort(rates.begin(), rates.end());
    assert(std::unique(rates.begin(), rates.end()) == rates.end());
    auto pos = std::lower_bound(rates.begin(), rates.end(), sampleRate);
    if (pos == rates.end())
        return rates.front();

    if (*pos == sampleRate || prefer < 0)
        return *pos; // lower_bound

    auto pos1 = std::next(pos);
    if (pos1 == rates.end())
        return *pos;

    if (prefer > 0)
        return *pos1; // upper_bound

    return *pos1 - sampleRate > sampleRate - *pos ? *pos : *pos1; // nearest
}



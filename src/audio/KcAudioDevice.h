#pragma once
#include <vector>
#include <string>
#include "KtObservable.h"


// 音频设备基类
class KcAudioDevice : public KtObservable<void*, void*, unsigned, double>
{
public:

    KcAudioDevice(void);
    virtual ~KcAudioDevice(void);


	enum KeSampleRate
	{
        k_8k  =  8000,
        k_11k = 11025,
		k_12k = 12000,
        k_16k = 16000,
        k_22k = 22050,
		k_24k = 24000,
        k_32k = 32000,
        k_44k = 44100,
        k_48k = 48000,
        k_88k = 88200,
        k_96k = 96000
	};


	enum KeSampleFormat // 与RtAudio保持一致
	{
        k_sint8   = 0x01,   // 8-bit signed integer.
        k_sint16  = 0x02,   // 16-bit signed integer.
        k_sint24  = 0x04,   // 24-bit signed integer.
        k_sint32  = 0x08,   // 32-bit signed integer.
        k_float32 = 0x10,   // Normalized between plus/minus 1.0.
        k_float64 = 0x20    // Normalized between plus/minus 1.0.
	};


    // The device information structure.
	struct KpDeviceInfo 
	{
        std::string name;            /*!< Character string device identifier. */
        unsigned outputChannels;     /*!< Maximum output channels supported by device. */
        unsigned inputChannels;      /*!< Maximum input channels supported by device. */
        unsigned duplexChannels;     /*!< Maximum simultaneous input/output channels supported by device. */
        std::vector<unsigned> sampleRates;    /*!< Supported sample rates (queried from list of standard rates). */

		KpDeviceInfo() : outputChannels(0), inputChannels(0), duplexChannels(0) { }
        KpDeviceInfo(const KpDeviceInfo& di) {
			name = di.name;
			outputChannels = di.outputChannels;
			inputChannels = di.inputChannels;
			duplexChannels = di.duplexChannels;
			sampleRates = di.sampleRates;
		}
	};

	struct KpStreamParameters
	{
        unsigned deviceId;     /*!< Device index (0 to getDeviceCount() - 1). */
        unsigned channels;    /*!< Number of channels. */
	};


    // query for the number of audio devices available.
    unsigned count() const;


    // return a KpDeviceInfo structure for a specified device number.
    KpDeviceInfo info(unsigned deviceId) const;


    // query for the default input audio devices. -1 for none.
    unsigned defaultInput() const;


    // query for the default output audio devices. -1 for none.
    unsigned defaultOutput() const;


    // query the device by name. -1 for none.
    unsigned getDevice(const std::string& name) const;


    // query for the existence of input audio devices
    bool hasInput() const;

    // query for the existence of output audio devices
    bool hasOutput() const;


    // return 0 for error
    unsigned preferredSampleRate(unsigned deviceId) const;



    /* opening a stream with the specified parameters.

	\param outputParameters Specifies output stream parameters to use
			when opening a stream, including a device ID, number of channels,
			and starting channel number.  For input-only streams, this
			argument should be NULL.  The device ID is an index value between
			0 and getDeviceCount() - 1.
	\param inputParameters Specifies input stream parameters to use
			when opening a stream, including a device ID, number of channels,
			and starting channel number.  For output-only streams, this
			argument should be NULL.  The device ID is an index value between
			0 and getDeviceCount() - 1.
	\param format An RtAudioFormat specifying the desired sample data format.
	\param sampleRate The desired sample rate (sample frames per second).
    \param bufferFrames A pointer to a value indicating the desired
			internal buffer size in sample frames.  The actual value
			used by the device is returned via the same pointer.  A
			value of zero can be specified, in which case the lowest
			allowable value is determined.
	*/
    bool open(const KpStreamParameters *outputParameters, const KpStreamParameters *inputParameters,
                    KeSampleFormat format, unsigned sampleRate, unsigned& bufferFrames);


    // close a stream and frees any associated stream memory.
    bool close();


	//! A function that starts a stream.
    bool start();


	//! Stop a stream, allowing any samples remaining in the output queue to be played.
    bool stop(bool wait);


	//! Stop a stream, discarding any samples remaining in the input/output queue.
    bool abort(bool wait);


	//! Returns true if a stream is open and false if not.
    bool opened() const;


	//! Returns true if the stream is running and false if it is stopped or not open.
    bool running() const;


    //! wait until device not running
    void wait() const;


	//! Returns the number of elapsed seconds since the stream was started.
    double getStreamTime();


	//! Set the stream time to a time in seconds greater than or equal to 0.0.
    void setStreamTime( double time );


	//! Returns the internal stream latency in sample frames.
	/*!
	The stream latency refers to delay in audio input and/or output
	caused by internal buffering by the audio system and/or hardware.
	For duplex streams, the returned value will represent the sum of
	the input and output latencies.  If the API does not
	report latency, the return value will be zero.
	*/
    long latency();


	//! Returns actual sample rate in use by the stream.
	/*!
	On some systems, the sample rate used may be slightly different
	than that specified in the stream parameters.
	*/
    unsigned sampleRate() const;

    unsigned frameSamples() const { return frameSamples_; } // 每帧采样点数
    double frameDuration() const; // 每帧时长（s）

    auto inputChannels() const { return inChannels_; }
    auto outputChannels() const { return outChannels_; }

    const char* errorText() const { return error_.c_str(); }


private:
    void* device_;
    unsigned inChannels_;
    unsigned outChannels_;
    unsigned frameSamples_;
    std::string error_;
};


#pragma once
#include <math.h>


// 频域类一些常用单位的换算

class KuFreqUnit
{
public:

	/*************** Hz <--> Mel *********************/

	static double hertzToMel(double hertz);
	static double melToHertz(double mel);

	static double melToHertz3(double mel); 
	static double hertzToMel3(double hz); 

	static double melToHertz2(double mel); 
	static double hertzToMel2(double hz);



	/*************** Hz <--> BARK *********************/

	static double hertzToBark(double hertz) {
		return hertzToBark1977(hertz);
	}

	static double barkToHertz(double bark) {
		return barkToHertz1977(bark);
	}

	// Schroeder (1977) bark = 7*asinh(f/650)
	static double hertzToBark1977(double hz); 
	static double barkToHertz1977(double bark); 

	// Terhardt (1979) bark = 13.3*atan(0.75*f/1000)
	static double hertzToBark1979_1(double hz);
	static double barkToHertz1979_1(double bark);

	// Terhardt (1979) 
	// bark = 12.82*atan(0.78*f/1000)+0.17*(f/1000)^1.4
	static double hertzToBark1979_2(double hz);
	static double barkToHertz1979_2(double bark); // 该转换不支持，未实现相关代码，不可调用

	// Zwicker & Terhardt (1980) 
	// bark = 13*atan(0.76f/1000)+3.5*atan(f/7500)^2
	static double hertzToBark1980_1(double hz);
	static double barkToHertz1980_1(double bark); // 该转换不支持，未实现相关代码，不可调用

	// Zwicker & Terhardt (1980) 
	// bark = 8.7+14.2log10(f/1000)
	static double hertzToBark1980_2(double hz);
	static double barkToHertz1980_2(double bark);

	// Traunmueller (1990) 
	// bark = 26.81/(1+1960/f)-0.53
	static double hertzToBark1990(double hz); 
	static double barkToHertz1990(double bark); 

	// Wang, Sekey & Gersho (1992) 
	// bark = 6*asinh(f/600)
	static double hertzToBark1992(double hz);
	static double barkToHertz1992(double bark);

	// Hartmann于1997年提出，基于1990版的改进实现

	/**
	* Converts frequency in Hz to corresponding critical-band number in Bark.
	*
	* Hartmann, 1997 p.252. Signals, Sound and Sensation.
	*
	* @hertz: Frequency in Hz.
	*
	* @return: Critical-band number in Bark;
	*/
	static double hertzToBark1997(double hertz);

	/**
	* Converts critical-band number in Bark to frequency in Hz.
	*
	* Hartmann, 1997 p.253. Signals, Sound and Sensation.
	*
	* @brak: Critical-band number in Bark.
	*
	* @return: Frequency in Hz.
	*/
	static double barkToHertz1997(double bark);


	static double criticalBand(double fc/*Hz*/);

	/*************** Hz <--> CAM *********************/

	// ERB - Equivalent Rectangular Badnwidth，等效矩形带宽。
	// 是一种心理声学(Psychoacoustics)的度量方法，
	// 它提供一个近似于人耳听觉的对带宽的过滤方法。

	/* Moore & Glasberg 1983 JASA 74: 750 */
	// 这个近似是基于一些出版的同时掩蔽实验的结果，对于从0.1到6.5kHz的范围是有效的。
	// 适用于中等的声强和年轻的听者。
	static double erb1983(double fc); // fc为过滤器的中心频率（Hz）,返回过滤器的带宽（Hz）

	// Moore & Glasberg 于1990年提出的另一个近似，对于0.1到10kHz的范围是有效的。
	// 适用于中等的声音水平。
	static double erb1990(double fc);

	// ERBN-number scale (Moore, 2012).
	// Hz <-> Cams
	// The ERBN-number scale is similar to a scale of distance along the cochlea; 
	// each 1 - Cam step represents a distance of about 0.89mm along the basilar
	// membrane(Moore, 1986).
	static double hertzToCam(double hertz);
	static double camToHertz(double cam);

	static double hertzToNormCam(double freq) {
		return (hertzToCam(freq) / hertzToCam(1000.0)) - 1;
	}

	static double normCamToHertz(double cam) {
		return (cam + 1) * hertzToCam(1000.0);
	}

	// praat实现的转换算法
	static double hertzToCamPraat(double hertz);
	static double camToHertzPraat(double cam);



	//----------------乐音相关单位换算--------------------------

	// if fr == 2.0, return 12 semitones
	// if fr == 0.5, return -12 semitones
	static double freqRatioToSemitones(double fr);

	static double semitonesToFreqRatio(double semitones);

	// 计算hertz相比base_hertz的半音数
	// if hertz >= base_hertz, result semitones >= 0
	// if hertz <  base_hertz, result semitones <  0
	static double hertzToSemitones(double hertz, double base_hertz = 100.0);

	// 计算相比base_hertz高出semitones半音的频率值
	static double semitonesToHertz(double semitones, double base_hertz = 100.0);

	// 八度转换为半音
	static double octaveToSemitones(double octave) {
		return 12.0 * octave;
	}

	// 半音转换为八度
	static double semitonesToOctave(double semitones) {
		return semitones / 12.0;
	}

	static double octaveToFreqRatio(double octave) {
		return semitonesToFreqRatio(octaveToSemitones(octave));
	}

	static double freqRatioToOctave(double fr) {
		return semitonesToOctave(freqRatioToSemitones(fr));
	}

	// 音分数与半音的转换
	static double centsToSemitones(double cents) {
		return cents / 100.0;
	}

	static double semitonesToCents(double semitones) {
		return 100.0 * semitones;
	}


	// Convert decibels to amplitude.
	static double decibelsToAmplitude(double decibels) {
		return ::pow(10, decibels / 20.0);
	}

	// Convert amplitude to decibels.
	static double amplitudeToDecibels(double amplitude) {
		return 20 * ::log10(amplitude);
	}

	// Convert decibels to power.
	static double decibelsToPower(double decibels) {
		return ::pow(10, decibels / 10.0);
	}

	// Convert power to decibels.
	static double powerToDecibels(double power) {
		return 10 * ::log10(power);
	}


private:
	KuFreqUnit() = delete;
};


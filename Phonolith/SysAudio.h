#ifndef _SYSAUDIO_
#define _SYSAUDIO_

#include <string>

#include <exec/exec.h>
#include <devices/ahi.h>


class SysAudio
{
	public:
		static void openLib();
		static void closeLib();

		static bool requestMode(unsigned long *modeID, unsigned long *frequency, string *modeName);

		SysAudio();
		~SysAudio();

		bool openAudio(unsigned long audioModeID, long hwFrequency, long srcFrequency, int srcBytesPerSample, long bufferSize);
		void closeAudio();

		int getNumOutputs();
		void activateOutputs(int firstOut, int lastOut);
		void deactivateOutputs(int firstOut, int lastOut);

		void copyBufferToOutputs(int firstOut, int channels, void *buffer);
		void process();

	protected:
		const static int NUM_OF_BUFFERS = 2;

		static MsgPort *mAHImp;
		static AHIRequest *mAHIio;

		static Hook mSoundHook;
		AHIAudioCtrl *mAudioCtrl;
		AHISampleInfo mInfo[NUM_OF_BUFFERS];
		int mNumOutputs;
		bool *mOutputActive;
		long mBufferSize;
		long mSrcFrequency;
		int mSrcBytesPerSample;

		int mBufferNumber;

		static uint32 SoundFunc(Hook *hook, AHIAudioCtrl *audioCtrl, AHISoundMessage *msg);

};

#endif


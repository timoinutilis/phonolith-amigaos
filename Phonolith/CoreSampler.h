#ifndef _CORESAMPLER_
#define _CORESAMPLER_

#include "CoreInstrument.h"
#include "CoreProject.h"
#include "CoreSamplerUnit.h"
#include "SysTimer.h"
#include "Phonolith.h"

#include <dos/dosextens.h>

struct MidiEvent {
	unsigned long slice;
	unsigned long frame;
	char status;
	char channel;
	char data1;
	char data2;
};

class CoreSampler
{
	public:
		CoreSampler(unsigned long audioModeID, long frequency, int srcBytesPerSample, long bufferSize, int maxVoices, const char *midiPortName, CoreProject *project);
		~CoreSampler();

		void resetMidiReceived();

		CoreSamplerUnit *getSamplerUnit(int unitIndex);
		void lockUnit(int unitIndex);
		void unlockUnit(int unitIndex);
		void setUnitNumVoices(int unitIndex, int num);

		static short mPeakL[CoreProject::CHANNELS];
		static short mPeakR[CoreProject::CHANNELS];
		static short mMasterPeakL;
		static short mMasterPeakR;
		static string mLastProblem;

};

#endif


#ifndef _CORESAMPLERUNIT_
#define _CORESAMPLERUNIT_

#include <string>

#include "CoreSound.h"
#include "CoreInstrument.h"
#include "CoreVoice.h"
#include "Vector.h"

#include "CoreEnvelope.h"

class CoreSamplerUnit
{
	public:
		CoreSamplerUnit(CoreInstrument *instrument, float *mixBuffer, long bufferSize, unsigned long frequency, int maxVoices);
		~CoreSamplerUnit();

		void setNumVoices(int num);
		void abortSounds();
		void lock();
		void unlock();

		void pressedKey(char key, char velocity, unsigned long frame);
		void releasedKey(char key, unsigned long frame);

		void startTone(unsigned char key);
		void changeTone(unsigned char key);
		void stopTone();

		void process();

	protected:
		CoreInstrument *mInstrument;
		int mMaxVoices;
		int mNumVoices;
		CoreVoice **mVoices;
		short *mVoiceKeys;
		float *mMixBuffer;
		long mBufferSize;
		unsigned long mFrequency;
		bool mToneActive;
		double mTonePosition;
		double mToneFrequency;
		bool mLocked;

};

#endif


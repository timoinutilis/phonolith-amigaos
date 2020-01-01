#ifndef _COREVOICE_
#define _COREVOICE_

#include "CoreSound.h"

#include "CoreEnvelope.h"
#include "CoreEnvelopeGenerator.h"
#include "CoreInstrument.h"

class CoreVoice
{
	public:
		CoreVoice(unsigned long mixFrequency);
		~CoreVoice();

		void startSound(Zone *kg, char key, char velocity, CoreEnvelope *envelope, long frame);
		void stopSound(long frame);
		bool isFree();
		void abortSound();

		void process(float *mixBuffer, long bufferSize);

		unsigned long mPlayTime;

		CoreEnvelopeGenerator mAmpEnvelope;

	protected:
		CoreSound *mSound;
		double mMixFrequency;
		double mPosition;
		double mFrequency;
		float mLeftAmp;
		float mRightAmp;
		long mStopAtFrame;
		long mStartAtFrame;
		unsigned long mEndPosition;

		bool mInUse;

};

#endif


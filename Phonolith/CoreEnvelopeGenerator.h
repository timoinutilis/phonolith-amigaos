#ifndef _COREENVELOPEGENERATOR_
#define _COREENVELOPEGENERATOR_

#include "CoreEnvelope.h"

class CoreEnvelopeGenerator
{
	public:
		CoreEnvelopeGenerator();

		void init(CoreEnvelope *envelope, char velocity);
		float getNextVolume();
		void beginRelease();

		float mMixFrequency;

		const static int ADSR_ATTACK = 0;
		const static int ADSR_DECAY = 1;
		const static int ADSR_SUSTAIN = 2;
		const static int ADSR_RELEASE = 3;

		int mState;

	protected:
		float mAdsrVolume;

		float mAttackTime;
		float mDecayTime;
		float mSustainLevel;
		float mReleaseTime;
		float mMaxVolume;


};

#endif


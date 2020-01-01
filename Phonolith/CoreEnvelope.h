#ifndef _COREENVELOPE_
#define _COREENVELOPE_

class CoreEnvelope
{
	public:
		void setADSR(char attackTime, char decayTime, char sustainLevel, char releaseTime);

		char mAttackTime;
		char mDecayTime;
		char mSustainLevel;
		char mReleaseTime;
};

#endif

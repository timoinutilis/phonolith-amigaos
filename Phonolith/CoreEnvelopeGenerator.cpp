/*
Phonolith Software Sampler
Copyright (C) 2006-2008 Timo Kloss

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

using namespace std;

#include <iostream>
#include <cmath>

#include "CoreEnvelopeGenerator.h"


//==================================================================================
// Constructor
//==================================================================================

CoreEnvelopeGenerator::CoreEnvelopeGenerator()
{
	mMixFrequency = 1;
}

//==================================================================================
// Control
//==================================================================================

void CoreEnvelopeGenerator::init(CoreEnvelope *envelope, char velocity)
{
	mState = ADSR_ATTACK;
	mAdsrVolume = 0;

	mAttackTime = pow((float)envelope->mAttackTime, 2) / 1000;
	mDecayTime = pow((float)envelope->mDecayTime, 2) / 1000;
	mSustainLevel = envelope->mSustainLevel / 127.0;
	mReleaseTime = pow((float)envelope->mReleaseTime, 2) / 1000;
	mMaxVolume = velocity / 127.0;

}

//----------------------------------------------------------------------------------

float CoreEnvelopeGenerator::getNextVolume()
{

	if (mState == ADSR_ATTACK)
	{
		if (mAttackTime > 0)
		{
			mAdsrVolume += 1.0 / mMixFrequency / mAttackTime;
		}
		if (mAdsrVolume > 1 || mAttackTime == 0)
		{
			mAdsrVolume = 1.0;
			mState = ADSR_DECAY;
		}
	}
	if (mState == ADSR_DECAY)
	{
		if (mDecayTime > 0)
		{
			mAdsrVolume /=  1 + 4 / mMixFrequency / mDecayTime;
		}
		if (mAdsrVolume < mSustainLevel + 0.001 || mDecayTime == 0)
		{
			mAdsrVolume = mSustainLevel;
			mState = ADSR_SUSTAIN;
		}
	}
	if (mState ==  ADSR_RELEASE)
	{
		if (mReleaseTime > 0)
		{
			mAdsrVolume /=  1 + 4 / mMixFrequency / mReleaseTime;
		}
		if (mAdsrVolume < 0.001 || mReleaseTime == 0)
		{
			mAdsrVolume = 0;
		}
	}
	return mAdsrVolume * mMaxVolume;
}

//----------------------------------------------------------------------------------

void CoreEnvelopeGenerator::beginRelease()
{
	mState = ADSR_RELEASE;
}

